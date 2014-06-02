#  Project based on SVC
This is a project based on SVC 

# Server
Server uses the [JSVM](http://www.hhi.fraunhofer.de/de/kompetenzfelder/image-processing/research-groups/image-video-coding/svc-extension-of-h264avc/jsvm-reference-software.html) library to encode and decode the video file (mostly yuv file). 

###Building the software
Documantation file is in "/svc/server/jsvm/SoftwareManual.doc". The step of building this project in Linux is as follows:

	cd JSVM/H264AVCExtension/build/linux
	make

###Encoder and Decoder example
Configure files examples are in folder "/svc/server/jsvm/example/". The steps of encoding a yuv file to be multiple layers are as follows:

	cd /svc/server/jsvm/bin
	./H264AVCEncoderLibTestStatic -pf encoder.cfg


# Client 
Client uses the [Multi-Core SVC Decoder on Android](http://nmsl.cs.nthu.edu.tw/index.php/component/content/article/81-projects/101-multi-core-svc-decoder-on-android) project which based on Android NDK. After test, the project requries the linux gcc compiler, and I recommend to use linux as the development system. Run the following command to build the project:
	
	cd to /svc/client/svc_mc
	<NDK path>/ndk-build


#Mplayer install
Inside of [Open SVC Decoder](http://sourceforge.net/projects/opensvcdecoder/), there is a mplayer which can be installed to play the bit stream files (such as y4m and yuv format files). The step of install is as follows:
###a. run: 
	./configure
###b. run:
	make install
###c. play the source file, for example to play a video file named "a.y4m", run:
	mplayer a.y4m

to play the yuv source file, the codec parameters have to be defined, for example to run a video file (176*144) named "akiyo_qcif.yuv", run:

	mplayer -demuxer rawvideo -rawvideo w=176:h=144:format=i420 akiyo_qcif.yuv -loop 1


# Video files
Video resources is y4m Video Sequences and some source files are in folder "/svc/server/video_data". Other source files can also be downloaded from [Xiph.org Video Test Media](http://media.xiph.org/video/derf/). 

The y4m format files can be converted to yuv format by using "ffmpeg" package, for example to convert a "akiyo_qcif.y4m" to "akiyo_qcif.yuv" can simply run:

    ffmpeg -i akiyo_qcif.y4m akiyo_qcif.yuv
    