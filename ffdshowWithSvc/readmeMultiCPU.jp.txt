1. ffdshow ビデオ再生のマルチスレッド化について

マルチスレッド化の目的はコマ落ちを減少させることです。そのためにサンプルをビデオレンダラに送る前にキューに入れることとビデオレンダラを別スレッドで実行することを実装します。
ビデオレンダラには子スレッドでは実行不可なものがあります。Windows2000以前のデフォルトレンダラ、Zoom Playerのデフォルト(Overlay Mixer, Standard Renderer)、MPCの"Old Renderer"などがこれにあたります。この場合ビデオレンダラに関する部分のマルチスレッド化は回避され、リサイズをマルチスレッドで行います。

2.設定

Miscellaneous/Other controlsのQueue output samplesで設定します。シングルＣＰＵでも有効なことがあるようです。

3.実装の概要（開発者の方へ）
ffdshowDecVideoクラスの専用出力ピンとしてTffdshowDecVideoOutputPinを実装しています。Directshow SDKのbase classesからCOutputQueueを利用しています。前回のパッチで使用したm_csReceiveProtectorとTffdshowDecVideoInputPinは削除しました。
libmplayer.dllについては"Resize", "Sharpen/swscaler" , "Blur & NR/swscaler gaussian blur"をマルチスレッド化しています。P4HTではlibmplayerのマルチスレッドは使用されません(全然速くならないので)。

4.テスト結果

P4HTやdual coreマシンではrev2546と比較してフレームレートで10～20％(Max 35%)アップが期待できます。シングルCPUでは少しですが効果があることもあります。

5. Web links

ffdshow: http://cutka.szm.sk/ffdshow/ or http://sourceforge.net/projects/ffdshow/
ffmpeg:  http://ffmpeg.org/
mplayer: http://www.mplayerhq.hu/

and
doom9:   http://forum.doom9.org/showthread.php?t=98600

6. 再配布など

マルチスレッド化に関する部分を含めすべてのソースコードとffdshowはGNU General Public Licenseの下で再配布可能です。copying.txt（またはcopying.jp.txt）をお読みください。


Implementation of multithreading by Haruhiko Yamagata<h.yamagata@nifty.com>
ffdshow is a software written by Milan Cutka.
Swscaler is a software written by Michael Niedermayer.
