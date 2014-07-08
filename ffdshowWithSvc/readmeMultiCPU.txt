### 1. About multithreading of ffdshow video playback ###

The objective of the queue is to reduce frame drops. It is effective when CPU load is high and dropping a few frames/sec. It deposits the time when CPU has finished work early and use the time
 when delayed. Because video renderer is executed on another thead, the effect is greater if you have a Pentium4HT or dual core CPU.

It queues samples just before sent to video renderer and execute video renderer on worker thread. The old video renderer cannot run on child thread. It is the default renderer of Windows 2000 or older, the default of Zoom Player (Overlay Mixer, Standard Renderer), "Old Renderer" of MPC, etc. In this case, output queue is avoided and Resize is processed on multithread.

### 2. Configuration ###

Go to "Queue & misc". You will see "Queue output samples". On single CPU it may be effective but not very much.

### 3. Overview of implementation (for developers) ###

TffdshowDecVideoOutputPin is implemented for TffdshowDecVideo's exclusive use. COutputQueue is used from baseclassed of Directshow SDK. m_csReceiveProtector and TffdshowDecVideoInputPin, which I implemented for prior version, have been deleted. As for libmplayer.dll, "Resize", "Sharpen/swscaler" and "Blur & NR/swscaler gaussian blur" is multithreaded. On P4HT, libmplayer avoids multithreading because it is not faster at all.

### 4. Result ###

On P4HT or dual core CPU, 10% to 20% (Max 35%) of frame rate improvement is expected compared to rev2546. On single CPU some effect may be seen.

### 5. Web links ###

ffdshow:
http://ffdshow.sourceforge.net/tikiwiki/
http://sourceforge.net/projects/ffdshow/

FFmpeg:
http://ffmpeg.mplayerhq.hu/

MPlayer:
http://www.mplayerhq.hu/

or at Doom9:
http://forum.doom9.org/showthread.php?t=98600

### 6. Copying ###

All used sources including multithreading related code and ffdshow itself are distributed under GPL (see copying.txt).

Implementation of multithreading by Haruhiko Yamagata <h.yamagata@nifty.com>.
ffdshow is a software written by Milan Cutka.
Swscaler is a software written by Michael Niedermayer.