# RTP Sender and Recevier for Multicast in MST Mode for SVC #

## Intro ##

Tools for Scalable Video Coding (SVC) bitstream reordering to support Dynamic Adaptive Streaming over HTTP (DASH).

* Demultiplexer for SVC-DASH. Splits the SVC bitstream into chunks, one per layer. Also generates an MPD. This tool is deployed at the server.
* Re-Multiplexer for SVC-DASH. Reorders SVC layer chunks into a single SVC bitstream. This tool is deployed at the client (after the DASH client, before the SVC decoder).

In SVC-based DASH, the SVC layers are provided in multiple representations. A normal SVC bitstream has enhancement layers located at each frame. For SVC-DASH each temporal segment is split into multiple chunks, one per layer. Each of the chunks contains several frames for one layer.

![Bitstream Reordering](https://sourceforge.net/p/svc-demux-mux/code/ci/master/tree/bitstream-reordering.jpg?format=raw "Segmentation of SVC bitstream for DASH. SVC layers in <a> original bitstream and <b> segmentation for DASH.")

    Segmentation of SVC bitstream for DASH. SVC layers in (a) original bitstream and (b) segmentation for DASH.

Both tools are implemented as Python scripts.


## Application ##

### Server Side ###

To split the SVC bitstream into its layers: 

    python svc_demux.py file.264

The output will be stored as "file.264.seg0-L0.264", "file.264.seg0-L1.264", "file.264.seg0-L2.264", etc.

The Media Presentation Description (MPD) will be stored as "file.264.mpd"


To generate segments of 2-seconds at 25 fps (i.e., 50 frames per segment):

    python svc_demux.py file.264 50

The output will be stored as "file.264.seg0-L`<`x`>`.264", "file.264.seg1-L`<`x`>`.264",  etc.


To adjust parameters such as resolution (e.g., 352x288), frame rate (e.g., 25 fps), and baseURL:

    python svc_demux.py file.264 50 352 288 25 http://example.com/

An example of server path is in "video" folder.

### Client Side ###

To run the client.py, the "PyUserInput" library is needed for python (check out [here](https://github.com/SavinaRoja/PyUserInput)).

To re-multiplex the SVC layers into a single SVC bitstream with correct NALU ordering: 

    python svc_mux.py out_file.264 file.264.seg0-L0.264 file.264.seg0-L1.264 file.264.seg0-L2.264

To use the client.py to download the serverside files, re-multiplex with required layer and play the out file:

    python client.py http://localhost/video/video_1.264.mpd -play

The detail of the online svc file can be listed with the mpd file:

    python client.py http://localhost/video/video_1.264.mpd -detail

### Two versions of client with different adaption algorithm ###
'client.py' is the normal hardswitch algorithm, which compares the estimated bandwidth with the layer thresholds.
'client_v2.py' is the improved switch algorithm, whose basic idea is to descrease the layer switch possibility of the adjacent segment, also the happiness metric is calculated in the happyDegree.m file (check also the equation in the slide)

## Notes ##

* Demultiplexer: SVC bitstreams must have constant IDR rate. MPD generation only supports quality scalability, i.e., spatial layers are currently not represented correctly in the MPD.
* Re-Multiplexer: Does currently not work with input pipes because we cannot detect the actual end of the stream (in contrast to temporary silence). 
* The tools were tested with Python 2.7 and 3.2.3.

## Credits

Author: Jun Liu


*This work was supported in part by the EC in the context of the ALICANTE project (FP7-ICT-248652). [http://www.ict-alicante.eu/](http://www.ict-alicante.eu/)*


## Copyright ##

Copyright (c) 2014 Jun Liu, Aalto University
