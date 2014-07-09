'''
The Aalto License

Copyright (c) 2014 Jun Liu
Email: jun.liu@aalto.fi

Version2 info: download the segment according to the speed of the network, 
			   downloaded file saved in .cache folder

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


if(len(sys.argv)<2):
	print("Input argv for this client!")
	print("Usage")
	print("python " + sys.argv[0] + " url_of_mpd_file" + " -detail")
	print("python " + sys.argv[0] + " url_of_mpd_file" + " -play layerID[16]")
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

def getXML(url):
	file= urllib2.urlopen(url)
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
			fileNamePath = directory + '/' + fileName
			# print segURL
			t1 = time.time()
			h = httplib2.Http(".cache")
			#print "save segURL is: " + segURL 
			resp, content = h.request(segURL, "GET")
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

def check_files(fileList):
	pass

def play_video(videoName, width, height):
	# subprocess.call(["mplayer", "-fps", "25", videoName, "-x", "640", "-y", "352"])
	print "mplayer open"
	logName = videoName + ".log"
	f = open(logName, "wb")
	p = subprocess.Popen(["mplayer", "-fps", "25", videoName, "-x", width, "-y", height], stdout=f, 
						stdin=subprocess.PIPE, stderr=subprocess.PIPE)
	i = 0
	while p.poll()==None:
		sleep(1)
	p.communicate(input="q")

def parse_frame_idx(text):
	tmp1 = text.split("/")[-2]
	tmp = tmp1.split(" ")
	currentFrame = tmp[-1]
	# print "currentFrame is: "+currentFrame+".\n"
	return int(currentFrame)

def switch_to_highlayer(outName, frameNumber, segNumber):
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
			print text
			print "Exit mplayer"
			print "=================================="
			stopDownload.append("stop")
			# print "stopDownload"
			# print stopDownload
			break 
		else:
			print text
			sleep(timeInterval)
			frameIdx = parse_frame_idx(text) 
			if frameIdx >= frameNumber*tmpIdx and frameIdx < frameNumber*tmpIdx + frameStep:
				print "======================================"
				print "currentFrame is: "+str(frameIdx)+".\n"
				if bool(stepList):
					tmpIdx = tmpIdx +1
					value = stepList.pop(0)
					if value >0:
						for t in range(value):
							k.tap_key('b')
							print "switch to higher layer"
							sleep(0.1)
				else:
					break

''' Parse MPD file 
'''
def parse_mpd(url):
	layerID = []
	layerBW = []
	layerList = ""
	data = getXML(url)
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
	print "start processing!"
	parseResult = parse_mpd(mpdUrl)
	print "========================================================"
	print "video information:"
	print "video resolution is:" + parseResult["width"] + "x" + parseResult["height"]
	print "layerID is: " + parseResult["layerList"]
	print "bandwidth requirement for each layer is: " + str(parseResult["layerBW"])
	print "Segment number is: " + str(parseResult["numberofSeg"])
	print "Duration of each segment is: " + parseResult["duration"] + " frames"
	print "========================================================"

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
		print "=================================================="
		print "start download segment " + str(i)
		print "previous reference speed is: " + str(speed/1000/8) + "KB/s"
		for j in range(0,len(layerID)):
			threshold = threshold + layerBWList[j]
			print "threshold of " + str(layerID[j]) + " is: " + str(threshold/1000/8) + "KB/s"
			if speed >= threshold:
				selectedLayer = layerID[j]
			elif j == 0:
				selectedLayer = layerID[j]
				break 
			else:
				break
		print "selectedLayer is: " + str(selectedLayer)
		if not bool(stopDownload):
			fileList, speed = download_seg(videoName, selectedLayer, parseResult["data"], i)
		else:
			print "Stop downloading"
			break
		print "Finish download segment " + str(i)

		'''Mux the downloaded files with differnt layers'''
		for j in fileList:
			command.append(j)
		print "start mux video file"
		subprocess.call(command)
		print "Mux finished"
		outName = videoName+ "/" + "out_" + videoName + ".264"
		'''
		check outName state for the 1st segment (status initiate)
		'''
		if i == 0:
			if os.path.isfile(outName):
				try:
					os.remove(outName)
				except OSError:
					print "Initiate error"
					quit()
		f1 = open(outName, 'a')
		f2 = open(outputSegName, 'r')
		content = f2.read()
		f1.write(content)
		f1.close()
		print "Finish handling segment" + str(i) + "\n"
		print "=================================================="
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
			#start timer for the switch layer controler
			print "before thread2, stepList is equal to:"
			print stepList
			thread2 = Thread(target = switch_to_highlayer, args = (outName, int(parseResult["duration"]), segNumber))
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
	print "video resolution is:" + parseResult["width"] + "x" + parseResult["height"]
	print "layerID is: " + parseResult["layerList"]
	print "bandwidth requirement for each layer is: " + str(parseResult["layerBW"])
	print "Segment number is: " + str(parseResult["numberofSeg"])
	print "Duration of each segment is: " + parseResult["duration"] + " frames"
