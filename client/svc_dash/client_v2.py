'''
Copyright (c) 2014 Jun Liu
Email: jun.liu@aalto.fi

Version2 info: change the layer selected algorithm to decrease switch times
		and improve users' happiness 
'''

from xml.dom.minidom import parseString
from urlparse import urlparse
from threading import Thread
import httplib, httplib2, urllib2
import subprocess, sys
import re
import os.path
from time import sleep
from pykeyboard import PyKeyboard
import time
import glob
from multiprocessing import Process
import datetime
from logger import *

if(len(sys.argv)<2):
	print "Input argv for this client!"
	print "Usage"
	print "python " + sys.argv[0] + " url_of_mpd_file" + " -detail"
	print "python " + sys.argv[0] + " url_of_mpd_file" + " -play layerID[16]"
	quit()

mpdUrl = sys.argv[1]					# for example  http://localhost/video/video_1.264.mpd
mpdUrlParser = urlparse(mpdUrl)
mpdUrlHost = mpdUrlParser.netloc		#localhost
mpdUrlPath = mpdUrlParser.path			#video/video_1.264.mpd
# global layerID
mpdName = mpdUrl.split('/')[-1]			#video_1.264.mpd
videoName = mpdName.split('.264.mpd')[0]
folderUrlPath = re.sub('\/'+mpdName+'$', '', mpdUrl)	#http://localhost/video
cacheMatch = folderUrlPath.split("//")[-1]
cacheMatch = cacheMatch.replace("/", ",")
stepList = []
stopDownload = []
speed = 1600000 #initiate the speed of the first segment, layer2
# speed = 2400000 #lay1
# speed = 6400000  #lay0
# speed = 10000 #lay0
downloadMonitor = []

def get_XML(url):
	try:
		file= urllib2.urlopen(url)
	except Exception:
		message = str(datetime.datetime.now()) + ": MPD file not founded"
		logging.error(message)
	data = file.read()
	file.close()
	dom = parseString(data)
	return dom

def download_seg(directory, layerID, dom, segID):
	if not os.path.exists(directory):
		os.makedirs(directory)
	fileList = []
	for layerdom in dom.getElementsByTagName("Representation"):
		# print int(layerID) == int(layerdom.attributes["id"].value)
		if int(layerID) >= int(layerdom.attributes["id"].value):
			segName = layerdom.getElementsByTagName("SegmentURL")[segID]
			# print mpdUrlHost
			fileName = str(segName.attributes['media'].value)
			segURL = folderUrlPath + '/'+ fileName
			# fileNamePath = directory + '/' + fileName
			# print segURL
			message = str(datetime.datetime.now()) +": Download time t0"
			logging.info(message)
			t2 = time.time()
			h = httplib2.Http(".cache")
			#print "save segURL is: " + segURL 
			try:
				resp, content = h.request(segURL, "GET")
				if resp.status ==200:
					pass
				else:
					message = str(datetime.datetime.now()) + ": Error code is: " + str(resp.status)
					logging.error(message)
					break
			except httplib2.HttpLib2Error, e:
				message = str(datetime.datetime.now()) + ": Exception type: "+ str(e)
				logging.error(message)
				break
				# print "Segment file not founded"
			fileSize = float(resp['content-length'])
			# print "file size:" + str(fileSize) + "bytes"
			t3 = time.time()
			timeInterval = float(t3-t2)
			message = str(datetime.datetime.now()) +": Download time t1"
			logging.info(message)
			# message = str(layerID)+" download delay is: " + str(timeInterval)
			# logging.info(message)
			speed = float(fileSize*8/(t3-t2))
			message = "Estimate Speed:" + str(speed/1000/8)
			logging.info(message)
			# f = open(fileNamePath, 'w')
			# f.write(content)
			# f.close()
			# print "download " + fileName + " completed"
			'''save all the .svc layer file to .cache folder'''
			fileCacheMatch = ".cache/" + cacheMatch + "," + fileName+"*"
			fileNamePath = glob.glob(fileCacheMatch)[0]
			fileList.append(fileNamePath)
	return fileList, speed, timeInterval

def play_video(videoName, width, height):
	# subprocess.call(["mplayer", "-fps", "25", videoName, "-x", "640", "-y", "352"])
	#print "mplayer open"
	message = str(datetime.datetime.now()) + ": mplayer open"
	logging.info(message)
	logName = videoName + ".log"
	f = open(logName, "wb")
	p = subprocess.Popen(["mplayer", "-fps", "25", videoName, "-x", width, "-y", height], stdout=f, 
						stdin=subprocess.PIPE, stderr=subprocess.PIPE)
	i = 0
	while p.poll()==None:
		sleep(1)
	p.communicate(input="q")

def parse_frame_idx(text):
	if len(text.split("/"))==1:
		# print "error in frame index parse"
		# print str(text)
		message = (str(datetime.datetime.now()) + ": error in frame index parse\n" +
					"last line in log text is:" + str(text))
		logging.error(message)
		return 1
	else:
		tmp1 = text.split("/")[-2]
		tmp = tmp1.split(" ")
		currentFrame = tmp[-1]
	# print "currentFrame is: "+currentFrame+".\n"
		return int(currentFrame)

def mplayer_controler(outName, frameNumber, segNumber, totalSeg):
	#totalIdx = len(inputList)
	'''
	timeInterval defines the print frequency of the frame number in the terminal. 
	'''
	timeInterval = 0.4
	frameStep = timeInterval*25
	k = PyKeyboard()	
	logName = outName + ".log"
	tmpIdx = 1
	while True:
		text = subprocess.check_output(["tail", "-1", logName])
		#tmpIdx = totalIdx - len(inputList)+1
		if "PAUSE" in text and "=====  PAUSE  =====\rV:" not in text:
			sleep(timeInterval)
			continue
		elif "Exiting" in text:
			# print str(text)
			# print "Exit mplayer"
			# print "=================================="
			message = str(datetime.datetime.now()) + ": " + text
			logging.info(message)
			message = (str(datetime.datetime.now()) + ": Exit mplayer\n" + 
						"==================================")
			logging.info(message)
			# stopDownload = True
			stopDownload.append("stop")
			# print "stopDownload"
			# print stopDownload
			break 
		elif "[vdpau]" in text or "no prescaling applied" in text or "H264-ES" in text or "======================" in text:
			continue
		else:
			print str(text) 
			sleep(timeInterval)
			frameIdx = parse_frame_idx(text) 
			# print "!!!!"
			# print downloadMonitor
			'''Check if the next segment finish download, if not pause the video at the before the current segment end'''
			if (frameIdx < frameNumber*tmpIdx-3*frameStep and frameIdx >= frameNumber*tmpIdx - (4*frameStep)):
				# if len(downloadMonitor)==totalSeg:
				# 	continue
				if len(downloadMonitor)<(tmpIdx+1) and len(downloadMonitor)!=totalSeg:
					t0_pause = time.time()
					print downloadMonitor
					k.tap_key(' ') #pause the video
					message = (str(datetime.datetime.now()) + ": Pause the video and wait for segment download. (frameID: " 
							+ str(frameIdx) + ")\n" )
					logging.info(message)
					while True:
						sleep(0.1)
						if len(downloadMonitor)>=(tmpIdx+1):
							t1_pause = time.time()
							interval_pause = float(t1_pause-t0_pause)
							k.tap_key(' ') #continue the video
							print downloadMonitor
							message = (str(datetime.datetime.now()) + ": Continue play the video, "+"pause time: "
									+ str(interval_pause) + "\n")
							logging.info(message)
							break
			if frameIdx >= frameNumber*tmpIdx and frameIdx < frameNumber*tmpIdx + frameStep:
				message = (str(datetime.datetime.now()) + ":\n" + "==================================\n" + 
							"CurrentFrame is: "+str(frameIdx)+".")
				logging.info(message)
				if bool(stepList):
					tmpIdx = tmpIdx +1
					value = stepList.pop(0)
					if value >0:
						for t in range(value):
							k.tap_key('b')
							message = str(datetime.datetime.now()) + ": switch to higher layer"
							logging.info(message)
							# print "switch to higher layer"
							sleep(0.1)
				else:
					break

''' Parse MPD file 
'''
def parse_mpd(url):
	layerID = []
	layerBW = []
	threshold = []
	layerList = ""
	data = get_XML(url)
	layRepresentTag = data.getElementsByTagName('Representation')
	width = layRepresentTag[0].attributes['width'].value
	height = layRepresentTag[0].attributes['height'].value
	for i in layRepresentTag:
		tmp = str(i.attributes['id'].value)
		layerID.append(tmp)
		layerList = layerList + i.attributes['id'].value + " "
		tempBW = float(i.attributes['bandwidth'].value)
		layerBW.append(tempBW)
	segListTag = data.getElementsByTagName("SegmentList")[0]
	duration = segListTag.attributes["duration"].value
	numberofSeg = len(segListTag.getElementsByTagName("SegmentURL"))
	tmp = 0
	for j in range(0,len(layerID)):
		tmp = tmp + layerBW[j]
		threshold.append(tmp)
	return {"layerID":layerID, "layerList":layerList, "data":data,
			"numberofSeg":numberofSeg, "duration":duration, "width":width, 
			"height":height, "layerBW":layerBW, "threshold":threshold}

if(sys.argv[2]=="-play"):
	'''
	Read and parse information in mpd file 
	'''
	logPath = os.getcwd() +"/log"
	if not os.path.exists("log"):
		os.makedirs("log")
	initialize_logger(logPath)
	parseResult = parse_mpd(mpdUrl)
	t0 = datetime.datetime.now()
	message = str(t0) + "\nStart processing!"
	logging.info(message)
	# print "start processing!"
	message = (str(datetime.datetime.now()) + ":\n========================================================\n" +
				"Video information:\n" + "Video resolution is:" + parseResult["width"] + "x" + parseResult["height"] +
				"\nLayerID is: " + parseResult["layerList"] + "\nBandwidth requirement for each layer is: " + 
				str(parseResult["layerBW"]) +" bits/s"+ "\nSegment number is: " + str(parseResult["numberofSeg"]) +
				"\nDuration of each segment is: " + parseResult["duration"] + " frames\n" + 
				"========================================================")
	logging.info(message)

	'''Download each segment according to segCheckList'''
	
	layerID = parseResult["layerID"]
	layerID = map(int, layerID)
	layerBWList = parseResult["layerBW"]
	segNumber = parseResult["numberofSeg"]
	threshold = parseResult["threshold"]
	# ratio = 0.9
	for i in range(0, segNumber):
		command = ["python", "svc_mux.py"]
		outputSegName = videoName+ "/" + "out_" + videoName + "_seg" + str(i) + ".264"
		# print outputName
		command.append(outputSegName)
		message = (str(datetime.datetime.now()) + ":\n==================================================\n" +
					"Start handling segment " + str(i) + ", previous reference speed is: " + str(speed/1000/8) + 
					"KB/s")
		logging.info(message)
		if i is 0:
			for j in range(0,len(layerID)):
				message = str(datetime.datetime.now()) + ": Threshold of " + str(layerID[j]) + " is: " + str(threshold[j]/1000/8) + "KB/s"
				logging.info(message)
				if speed >= 0.95*threshold[j]:# add ratio
					selectedLayer = layerID[j]
				elif j == 0:
					selectedLayer = layerID[0]
					break 
				else:
					break
		else:
			'''
			Use new decision algorithm to decrease switching. 
			'''
			preSelectedLayer = selectedLayer

			# print pre_speed, speed, preSelectedLayer

			# print preSelectedLayer
			for j in range(0,len(layerID)):
				message = str(datetime.datetime.now()) + ": Threshold of " + str(layerID[j]) + " is: " + str(threshold[j]/1000/8) + "KB/s"
				logging.info(message)
				if (pre_speed > 0.9*threshold[j] and pre_speed <= 1.1*threshold[j] 
					and speed > 0.9*threshold[j] and speed <= 1.1*threshold[j]):
					selectedLayer = preSelectedLayer
				else: 
					if speed >= 0.95*threshold[j]:# add ratio
						selectedLayer = layerID[j]
					elif j == 0:
						selectedLayer = layerID[0]
						break 
					else:
						break
		# print "selectedLayer is: " + str(selectedLayer)
		message = str(datetime.datetime.now()) + ": SelectedLayer is: " + str(selectedLayer)
		logging.info(message)
		'''
		If there is "exit" in the last line of the log file, stop download and stop the software.
		'''
		if bool(stopDownload):
			print stopDownload
			message = str(datetime.datetime.now()) + ": Stop downloading"
			logging.info(message)
			# print "Stop downloading"
			break
		else:
			pre_speed = speed
			fileList, speed_tmp, time_tmp = download_seg(videoName, selectedLayer, parseResult["data"], i)
			'''check for very small interval (in case the file size is very small it will mess up the bandwidth calculation)'''
			if time_tmp>0.05:
				speed = speed_tmp
			else:
				message = str(datetime.datetime.now()) + ": file size is too small!"
				logging.info(message)
			downloadMonitor.append(str(i)+" finish download")
			# print downloadMonitor
		message = str(datetime.datetime.now()) + ": Finish download segment " + str(i)
		logging.info(message)

		# print "Finish download segment " + str(i)

		'''Mux the downloaded files with differnt layers'''
		for j in fileList:
			command.append(j)
		message = str(datetime.datetime.now()) + ": Start mux video file"
		logging.info(message)
		# print "start mux video file"
		subprocess.call(command)
		message = str(datetime.datetime.now()) + ": Mux finished"
		logging.info(message)
		# print "Mux finished"
		outName = videoName+ "/" + "out_" + videoName + ".264"
		'''
		check outName state for the 1st segment (status initiate)
		'''
		if i is 0:
			if os.path.isfile(outName):
				try:
					os.remove(outName)
				except OSError:
					message = str(datetime.datetime.now()) + ": Initiate error"
					logging.error(message)
					# print "Initiate error"
					quit()
		try:
			f1 = open(outName, 'a')
			f2 = open(outputSegName, 'r')
			content = f2.read()
			f1.write(content)
		except IOError:
			message = str(datetime.datetime.now()) + ": Error: can\'t find file or read data"
			logging.error(message)
		else:
			f1.close()
		message = (str(datetime.datetime.now()) + ": Finish handling segment " + str(i) + 
					"\n==================================================")
		logging.info(message)
		if i is 0:
			'''
			Calculate the selected layer of 2nd segment to get the stepList for switching
			'''
			for j in range(0,len(layerID)):
				if speed >= threshold[j]:
					selectedLayer1 = layerID[j]
				elif j == 0:
					selectedLayer1 = layerID[0]
					break 
				else:
					break
			stepValue1 = layerID.index(selectedLayer1) - layerID.index(selectedLayer)
			stepList.append(stepValue1)
			preSelectedLayer = selectedLayer1
			t1 = datetime.datetime.now()
			message = str(t1) + "\nStart playing!"
			logging.info(message)


			thread1 = Thread(target = play_video, args = (outName, parseResult["width"], parseResult["height"]))
			thread1.start()

			message = "Starting delay: " + str(t1-t0)
			logging.info(message)

			sleep(0.1)
			thread2 = Thread(target = mplayer_controler, args = (outName, int(parseResult["duration"]), segNumber, int(segNumber)))
			thread2.start()
		elif i ==1:
			continue
		else:
			stepValue = layerID.index(selectedLayer) - layerID.index(preSelectedLayer)
			stepList.append(stepValue)
			# preSelectedLayer = selectedLayer

if(sys.argv[2]=="-detail"):
	'''
	Read and parse information in mpd file 
	'''
	parseResult = parse_mpd()
	message = (str(datetime.datetime.now()) + "Video resolution is:" + parseResult["width"] + "x" + parseResult["height"] +
				"\nLayerID is: " + parseResult["layerList"] + "\nBandwidth requirement for each layer is: " + 
				str(parseResult["layerBW"]) + " bits/s" + "\nSegment number is: " + str(parseResult["numberofSeg"]) +
				"\nDuration of each segment is: " + parseResult["duration"] + " frames\n" + 
				"========================================================")
	logging.info(message)


