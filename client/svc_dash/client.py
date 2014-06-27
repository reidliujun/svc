'''
The Aalto License

Copyright (c) 2014 Jun Liu

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
# layerToPlayList = [0, 32]
'''http://localhost/video5_seg5/video_5.264.mpd '''
# layerToPlayList = [0, 16, 16 ,0, 16]
'''python client.py http://localhost/video1_seg5/video_1.264.mpd'''
layerToPlayList = [0, 32, 16, 16, 32]  ##layToPlayList is the layer id to be played for each segment
'''python client.py http://localhost/video/video_1.264.mpd'''
# layerToPlayList = [0,32]
segCheckList = [None]*(len(layerToPlayList)-1)


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

			h = httplib2.Http(".cache")
			resp, content = h.request(segURL, "GET")
			##download to local file
			f = open(fileNamePath, 'w')
			f.write(content)
			f.close()
			print "download " + fileName + " completed"
			fileList.append(fileNamePath)
	return fileList

def check_files(fileList):
	pass

def play_video(videoName, width, height):
	# subprocess.call(["mplayer", "-fps", "25", videoName, "-x", "640", "-y", "352"])
	print "mplayer open"
	subprocess.call(["mplayer", "-fps", "25", videoName, "-x", width, "-y", height])
	# subprocess.call(["mplayer", "-fps", "25", videoName])

def switch_to_highlayer(frame, inputList):
	time = int(frame/25)
	k = PyKeyboard()
	for i in range(len(inputList)):
		for j in range(time):
			sleep(1)
		if inputList[i] >0:
			for t in range(inputList[i]):
				k.tap_key('b')
				print "press b"
				sleep(0.1)
	# k = PyKeyboard()
	# k.tap_key('b')
	# sleep(0.1)
	# k.tap_key('b')
	# print "press b"

def step_map(input):
	return {
		16: 1,
		32: 2,
		-16: -1,
		-32: -2,
		}.get(input, 0)

if(sys.argv[2]=="-play" and sys.argv[3] !=""):
	'''
	Read and parse information in mpd file 
	'''
	print "start processing!"
	layerID = []
	layerList = ""
	data = getXML()
	layRepresentTag = data.getElementsByTagName('Representation')
	width = layRepresentTag[0].attributes['width'].value
	height = layRepresentTag[0].attributes['height'].value
	for i in layRepresentTag:
		tmp = str(i.attributes['id'].value)
		layerID.append(tmp)
		layerList = layerList + i.attributes['id'].value + " "
	segListTag = data.getElementsByTagName("SegmentList")[0]
	duration = segListTag.attributes["duration"].value
	numberofSeg = len(segListTag.getElementsByTagName("SegmentURL"))

	'''
	Dumux the segment file and use mplay to play the output file with specific layer
	'''
	layerToPlay = sys.argv[3]
	if not layerToPlay in layerID:
		print "Use the -detail to check the input layerID again!"
		quit()

	'''Define segCheckList (layerID) to be downloaded for each segment'''
	for j in range(len(layerToPlayList)-1):
		tmp = layerToPlayList[j+1]-layerToPlayList[j]
		segCheckList[j] = step_map(tmp)

	'''Download each segment according to segCheckList'''
	for i in range(0, numberofSeg):
		command = ["python", "svc_mux.py"]
		outputSegName = videoName+ "/" + "out_" + videoName + "_seg" + str(i) + ".264"
		# print outputName
		command.append(outputSegName)

		# fileList = download_seg(layerToPblay, data, i)
		fileList = download_seg(videoName, layerToPlayList[i], data, i)
		for j in fileList:
			command.append(j)
		subprocess.call(command)
		outName = videoName+ "/" + "out_" + videoName + ".264"
		'''
		check outName state at first time
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
			
			thread1 = Thread(target = play_video, args = (outName, width, height))
			thread1.start()
			sleep(0.1)
			thread2 = Thread(target = switch_to_highlayer, args = (int(duration),segCheckList))
			thread2.start()

if(sys.argv[2]=="-detail"):
	'''
	Read and parse information in mpd file 
	'''
	layerID = []
	layerList = ""
	data = getXML()
	#print xmlTag
	layRepresentTag = data.getElementsByTagName('Representation')
	width = layRepresentTag[0].attributes['width'].value
	height = layRepresentTag[0].attributes['height'].value
	print "video resolution is:" + width + "x" + height
	for i in layRepresentTag:
		tmp = str(i.attributes['id'].value)
		layerID.append(tmp)
		layerList = layerList + i.attributes['id'].value + " "
	print "layerID is: " + layerList
	segListTag = data.getElementsByTagName("SegmentList")[0]
	duration = segListTag.attributes["duration"].value
	numberofSeg = len(segListTag.getElementsByTagName("SegmentURL"))
	print "Segment number is: " + str(numberofSeg)
	print "Duration of each segment is: " + duration + " frames"