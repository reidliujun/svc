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
# layerToPlayList = [0, 32]
'''http://localhost/video5_seg5/video_5.264.mpd '''
# layerToPlayList = [0, 16, 16 ,0, 16]
'''python client.py http://localhost/video1_seg5/video_1.264.mpd'''
# layerToPlayList = [0, 32, 16, 16, 32]  ##layToPlayList is the layer id to be played for each segment
'''python client.py http://localhost/video/video_1.264.mpd'''
# layerToPlayList = [0,32]
# segCheckList = [None]*(len(layerToPlayList)-1)

def getXML():
	file= urllib2.urlopen(mpdUrl)
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
	subprocess.call(["mplayer", "-fps", "25", videoName, "-x", width, "-y", height])
	# subprocess.call(["mplayer", "-fps", "25", videoName])

stepList = []

def switch_to_highlayer(totalFrame):
	time = int(totalFrame/25)
	k = PyKeyboard()
	# if bool(stepList):
	# if value is "start":
	while bool(stepList):
		tmp = stepList.pop(0) 
		# print "switch times is:"
		# print tmp
		for j in range(time):
			sleep(1)
		if  tmp >0:
			for t in range(tmp):
				k.tap_key('b')
				print "press b "
				sleep(0.1)
	# if not stepList:
	# for i in range(len(inputList)):
	# for j in range(time):
	# 	sleep(1)
	# 	if inputList[i] >0:
	# 		for t in range(inputList[i]):
	# 			k.tap_key('b')
	# 			print "press b"
	# 			sleep(0.1)

def step_map(input):
	return {
		16: 1,
		32: 2,
		-16: -1,
		-32: -2,
		}.get(input, 0)

''' Parse MPD file 
'''
def parse_mpd():
	layerID = []
	layerBW = []
	layerList = ""
	data = getXML()
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

if(sys.argv[2]=="-play" and sys.argv[3] !=""):
	'''
	Read and parse information in mpd file 
	'''
	print "start processing!"
	parseResult = parse_mpd()
	print "========================================================"
	print "video information:"
	print "video resolution is:" + parseResult["width"] + "x" + parseResult["height"]
	print "layerID is: " + parseResult["layerList"]
	print "bandwidth requirement for each layer is: " + str(parseResult["layerBW"])
	print "Segment number is: " + str(parseResult["numberofSeg"])
	print "Duration of each segment is: " + parseResult["duration"] + " frames"
	print "========================================================"


	'''
	Dumux the segment file and use mplay to play the output file with specific layer
	'''
	layerToPlay = sys.argv[3]
	if not layerToPlay in parseResult["layerID"]:
		print "Use the -detail to check the input layerID again!"
		quit()

	'''Define segCheckList (layerID) to be downloaded for each segment'''
	# for j in range(len(layerToPlayList)-1):
	# 	tmp = layerToPlayList[j+1]-layerToPlayList[j]
	# 	segCheckList[j] = step_map(tmp)

	'''Download each segment according to segCheckList'''
	speed = 2080000 #initiate the speed of the first segment
	layerID = parseResult["layerID"]

	layerBWList = parseResult["layerBW"]

	for i in range(0, parseResult["numberofSeg"]):
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
				selectedLayer = int(layerID[j])
			elif j == 0:
				selectedLayer = int(layerID[j])
				break 
			else:
				break
		print "selectedLayer is: " + str(selectedLayer)
		# fileList = download_seg(layerToPblay, data, i)
		# fileList, speed = download_seg(videoName, layerToPlayList[i], parseResult["data"], i)
		fileList, speed = download_seg(videoName, selectedLayer, parseResult["data"], i)
		print "finish download segment " + str(i) + "\n"
		print "=================================================="

		'''Mux the downloaded files with differnt layers'''
		for j in fileList:
			command.append(j)
		subprocess.call(command)
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
		if i == 0:
			threshold1 = 0
			for j in range(0,len(layerID)):
				threshold1 = threshold1 + layerBWList[j]
				if speed >= threshold1:
					selectedLayer1 = int(layerID[j])
				elif j == 0:
					selectedLayer1 = int(layerID[j])
					break 
				else:
					break
			stepValue1 = step_map(selectedLayer1-selectedLayer)
			stepList.append(stepValue1)
			preSelectedLayer = selectedLayer1
			thread1 = Thread(target = play_video, args = (outName, parseResult["width"], parseResult["height"]))
			thread1.start()
			sleep(0.1)
			#start timer for the switch layer controler
			print "before thread2, stepList is equal to:"
			print stepList
			thread2 = Thread(target = switch_to_highlayer, args = (int(parseResult["duration"]), ))
			thread2.start()
		elif i ==1:
			continue
		else:
			stepValue = step_map(selectedLayer-preSelectedLayer)
			stepList.append(stepValue)
			preSelectedLayer = selectedLayer
		'''Generate stepmap value'''
		# stepValue = step_map(selectedLayer-preSelectedLayer)
		# stepList.append(stepValue)

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
