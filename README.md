#  Project based on SVC
This is a project based on SVC 

# Server
Server uses the [JSVM](http://www.hhi.fraunhofer.de/de/kompetenzfelder/image-processing/research-groups/image-video-coding/svc-extension-of-h264avc/jsvm-reference-software.html) library to encode and decode the video file (mostly yuv file). 

###Building the software
Documantation file is in "/svc/server/jsvm/SoftwareManual.doc". The step of building this project in Linux is as follows:

	cd JSVM/H264AVCExtension/build/linux
	make

###Encoder and Decoder example
Configure files examples are in folder "/svc/server/jsvm/example/". You can either copy the "H264AVCEncoderLibTestStatic" fileto "/svc/server/jsvm/example/" or, copy the configuration file to "/svc/server/jsvm/bin". Take the 2nd scenario as an example, the steps of encoding a yuv file to be multiple layers are as follows:

	cd /svc/server/jsvm/bin
	./H264AVCEncoderLibTestStatic -pf encoder.cfg

After that, a .264 file will be generated, which is a SVC encoded file. 

###SVC file extraction
The output SVC file generated in the above process can be directly played in mplayer (check the detail in "Mplayer" section). And also it can be extraced to seperate .264 files representing different layers.

The extraction step is as follows:
    
    ./BitStreamExtractorStatic input_file.264  output_file.264 -e widhtxheight@frequency:bitrate 

for example:

    ./BitStreamExtractorStatic out.264 out_lay2.264 -e 704x576@60:2310


# Client 
Client uses the [Multi-Core SVC Decoder on Android](http://nmsl.cs.nthu.edu.tw/index.php/component/content/article/81-projects/101-multi-core-svc-decoder-on-android) project which based on Android NDK. After test, the project requries the linux gcc compiler, and I recommend to use linux as the development system. Run the following command to build the project:
	
	cd to /svc/client/svc_mc
	<NDK path>/ndk-build
Inside of the applicaiton code, it uses hard code method now. Therefore, the path of the video file to be played has to be defined inside of PC.c --> streamFromHttp, and Main.java --> curFilePath.


#Mplayer
###Install
Inside of [Open SVC Decoder](http://sourceforge.net/projects/opensvcdecoder/), there is a mplayer which can be installed to play the bit stream files (such as y4m and yuv format files). The step of install is as follows:
run: 

	./configure
	./configure --enable-svc
	make install

###Usage
To play a source file, for example to play a video file named "a.y4m", run:

	mplayer a.y4m

To play the yuv source file, the codec parameters have to be defined, for example to run a video file (176*144) named "akiyo_qcif.yuv", run:

	mplayer -demuxer rawvideo -rawvideo w=176:h=144:format=i420 akiyo_qcif.yuv -loop 1

To play a svc encoded file, layerId and temporalID can be set to play the exact layer of the file, check [this](http://sourceforge.net/apps/mediawiki/opensvcdecoder/index.php?title=Mplayer).
    
    mplayer -fps 25 mystream.264 -setlayer 16 -settemporalid 2

To play a extracted svc file, just run:

    mplayer -fps 25 out_lay0.264
    

# Video files
Video resources is y4m Video Sequences and some source files are in folder "/svc/server/video_data". Other source files can also be downloaded from [Xiph.org Video Test Media](http://media.xiph.org/video/derf/). 

The y4m format files can be converted to yuv format by using "ffmpeg" package, for example to convert a "akiyo_qcif.y4m" to "akiyo_qcif.yuv" can simply run:

    ffmpeg -i akiyo_qcif.y4m akiyo_qcif.yuv
    
Also, there are some online example of SVC encoded files which can be used, check [this](http://sourceforge.net/projects/opensvcdecoder/files/Video%20Streams/).


