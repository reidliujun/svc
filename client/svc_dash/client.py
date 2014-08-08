'''
Copyright (c) 2014 Jun Liu
Email: jun.liu@aalto.fi

Version info: the normal switch algorithm which selected layer according
		to the estimated bandwidth and switch threshold
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
speed = 10000 #initiate the speed of the first segment
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
			t1 = time.time()
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
			t2 = time.time()
			timeInterval = float(t2-t1)
			speed = float(fileSize*8/(t2-t1))
			# f = open(fileNamePath, 'w')
			# f.write(content)
			# f.close()
			# print "download " + fileName + " completed"
			'''save all the .svc layer file to .cache folder'''
			fileCacheMatch = ".cache/" + cacheMatch + "," + fileName+"*"
			fileNamePath = glob.glob(fileCacheMatch)[0]
			fileList.append(fileNamePath)
	return fileList, speed

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
		elif "[vdpau]" in text or "no prescaling applied" in text:
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
					print downloadMonitor
					k.tap_key(' ') #pause the video
					message = (str(datetime.datetime.now()) + ": Pause the video and wait for segment download.\n")
					logging.info(message)
					while True:
						sleep(0.1)
						if len(downloadMonitor)>=(tmpIdx+1):
							k.tap_key(' ') #continue the video
							print downloadMonitor
							message = (str(datetime.datetime.now()) + ": Continue play the video.\n")
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
	return {"layerID":layerID, "layerList":layerList, "data":data,
			"numberofSeg":numberofSeg, "duration":duration, "width":width, 
			"height":height, "layerBW":layerBW}

if(sys.argv[2]=="-play"):
	'''
	Read and parse information in mpd file 
	'''
	print str(datetime.datetime.now())
	logPath = os.getcwd() +"/log"
	if not os.path.exists("log"):
		os.makedirs("log")
	initialize_logger(logPath)
	parseResult = parse_mpd(mpdUrl)
	message = str(datetime.datetime.now()) + "\nStart processing!"
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

	for i in range(0, segNumber):
		command = ["python", "svc_mux.py"]
		outputSegName = videoName+ "/" + "out_" + videoName + "_seg" + str(i) + ".264"
		# print outputName
		command.append(outputSegName)
		threshold = 0
		message = (str(datetime.datetime.now()) + ":\n==================================================\n" +
					"Start handling segment " + str(i) + ", previous reference speed is: " + str(speed/1000/8) + 
					"KB/s")
		logging.info(message)
		for j in range(0,len(layerID)):
			threshold = threshold + layerBWList[j]
			# print "threshold of " + str(layerID[j]) + " is: " + str(threshold/1000/8) + "KB/s"
			message = str(datetime.datetime.now()) + ": Threshold of " + str(layerID[j]) + " is: " + str(threshold/1000/8) + "KB/s"
			logging.info(message)
			if speed >= threshold:
				selectedLayer = layerID[j]
			elif j == 0:
				selectedLayer = layerID[j]
				break 
			else:
				break
		# print "selectedLayer is: " + str(selectedLayer)
		message = str(datetime.datetime.now()) + ": SelectedLayer is: " + str(selectedLayer)
		logging.info(message)
		if bool(stopDownload):
			message = str(datetime.datetime.now()) + ": Stop downloading"
			logging.info(message)
			# print "Stop downloading"
			break
		else:
			fileList, speed = download_seg(videoName, selectedLayer, parseResult["data"], i)
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
		if i == 0:
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
		if i == 0:
			threshold1 = 0
			for j in range(0,len(layerID)):
				threshold1 = threshold1 + layerBWList[j]
				if speed >= threshold1:
					selectedLayer1 = layerID[j]
				elif j == 0:
					selectedLayer1 = layerID[j]
					break 
				else:
					break
			stepValue1 = layerID.index(selectedLayer1) - layerID.index(selectedLayer)
			stepList.append(stepValue1)
			preSelectedLayer = selectedLayer1
			thread1 = Thread(target = play_video, args = (outName, parseResult["width"], parseResult["height"]))
			thread1.start()
			sleep(0.1)
			thread2 = Thread(target = mplayer_controler, args = (outName, int(parseResult["duration"]), segNumber, int(segNumber)))
			thread2.start()
		elif i ==1:
			continue
		else:
			stepValue = layerID.index(selectedLayer) - layerID.index(preSelectedLayer)
			stepList.append(stepValue)
			preSelectedLayer = selectedLayer

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
