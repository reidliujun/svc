import subprocess, sys
import httplib
import httplib2
from urlparse import urlparse
import urllib2
from xml.dom.minidom import parseString
import re
import time
#from bs4 import BeautifulSoup
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

#print mpd_url_host
#print mpd_url_path

def getXML():
	file= urllib2.urlopen(mpdUrl)
	data = file.read()
	file.close()
	dom = parseString(data)
	return dom

def downloadSeg(layerID, dom, segID):
	fileList = []
	for layerdom in dom.getElementsByTagName("Representation"):
		# print int(layerID) == int(layerdom.attributes["id"].value)
		if int(layerID) >= int(layerdom.attributes["id"].value):
			segName = layerdom.getElementsByTagName("SegmentURL")[segID]
			# print mpdUrlHost
			fileName = str(segName.attributes['media'].value)
			segURL = folderUrlPath + '/'+ fileName
			# print segURL
			
			h = httplib2.Http(".cache")
			resp, content = h.request(segURL, "GET")
			##download to local file
			f = open(fileName, 'w')
			f.write(content)
			f.close()
			fileList.append(fileName)
	return fileList

def checkFiles(fileList):
	pass


if(sys.argv[2]=="-play" and sys.argv[3] !=""):
	'''
	read and parse information in mpd file 
	'''
	print "start processing!"
	layerID = []
	layerList = ""
	data = getXML()
	layRepresentTag = data.getElementsByTagName('Representation')
	for i in layRepresentTag:
		tmp = str(i.attributes['id'].value)
		layerID.append(tmp)
		layerList = layerList + i.attributes['id'].value + " "
	segListTag = data.getElementsByTagName("SegmentList")[0]
	numberofSeg = len(segListTag.getElementsByTagName("SegmentURL"))

	'''
	Dumux the segment file and use mplay to play the output file with specific layer
	'''
	layerToPlay = sys.argv[3]
	if not layerToPlay in layerID:
		print "Use the -detail to check the input layerID again!"
		quit()

	'''
	Condition can be add in this part to decide which layer to download for different segment
	'''
	# print downloadSeg(layerToPlay, data, 1)
	for i in range(0, numberofSeg):
		command = ["python", "svc_mux.py"]
		outputName = "out_" + videoName + "_seg" + str(i) + ".264"
		# print outputName
		command.append(outputName)
		# if i is 0:
		# 	fileList = downloadSeg(0, data, i)
		# else:
		# 	fileList = downloadSeg(16, data, i)
		fileList = downloadSeg(layerToPlay, data, i)
		for j in fileList:
			command.append(j)
		# print command
		subprocess.call(command)
		subprocess.call(["mplayer", "-fps", "25", outputName])	



if(sys.argv[2]=="-detail"):
	'''
	read and parse information in mpd file 
	'''
	layerID = []
	layerList = ""
	data = getXML()
	#print xmlTag
	layRepresentTag = data.getElementsByTagName('Representation')

	for i in layRepresentTag:
		tmp = str(i.attributes['id'].value)
		layerID.append(tmp)
		layerList = layerList + i.attributes['id'].value + " "
	print "layerID is " + layerList
	segListTag = data.getElementsByTagName("SegmentList")[0]
	numberofSeg = len(segListTag.getElementsByTagName("SegmentURL"))
	print "Segment number is " + str(numberofSeg)

