#ifndef _TGLOBALSETTINGS_H_
#define _TGLOBALSETTINGS_H_

#include "TOSDsettings.h"
#include "ffcodecs.h"
#include "Toptions.h"

#define MAX_COMPATIBILITYLIST_LENGTH 5000

#define WHITELIST_EXE_FILENAME \
	_l("3wPlayer.exe;ACDSee10.exe;ACDSee5.exe;ACDSee6.exe;ACDSee7.exe;ACDSee8.exe;ACDSee8Pro.exe;ACDSee9.exe;ACDSeePro2.exe;ACDSeePro25.exe;Acer Crystal Eye webcam.exe;aegisub.exe;afreecaplayer.exe;")\
	_l("afreecastudio.exe;aim6.exe;ALLPlayer.exe;allradio.exe;AlltoaviV4.exe;ALShow.exe;ALSong.exe;AltDVB.exe;amcap.exe;amf_slv.exe;amvtransform.exe;Apollo DivX to DVD Creator.exe;Apollo3GPVideoConverter.exe;")\
	_l("Ares.exe;AsfTools.exe;ass_help3r.exe;ASUSDVD.exe;Audition.exe;AutoGK.exe;autorun.exe;avant.exe;AVerTV.exe;Avi2Dvd.exe;avi2mpg.exe;avicodec.exe;avipreview.exe;aviutl.exe;avs2avi.exe;Badak.exe;")\
	_l("BearShare.exe;BePipe.exe;bestplayer.exe;bestplayer1.0.exe;BitComet.exe;BlazeDVD.exe;BoonPlayer.exe;bplay.exe;bsplay.exe;bsplayer.exe;BTVD3DShell.exe;Camfrog Video Chat.exe;CamRecorder.exe;CamtasiaStudio.exe;")\
	_l("carom.exe;CEC_MAIN.exe;christv.exe;chrome.exe;cinemaplayer.exe;CinergyDVR.exe;CodecInstaller.exe;ConvertXtoDvd.exe;coolpro2.exe;CorePlayer.exe;Crystal.exe;crystalfree.exe;CrystalPro.exe;cscript.exe;CTCMS.exe;")\
	_l("CTCMSU.exe;CTWave.exe;CTWave32.exe;cut_assistant.exe;dashboard.exe;demo32.exe;DivX Player.exe;DivxToDVD.exe;dllhost.exe;dpgenc.exe;Dr.DivX.exe;drdivx.exe;drdivx2.exe;DreamMaker.exe;DSBrws.exe;")\
	_l("DScaler.exe;dv.exe;dvbdream.exe;dvbviewer.exe;DVD Shrink 3.2.exe;DVDAuthor.exe;dvdfab.exe;DVDMaker.exe;DVDMF.exe;dvdplay.dll;dvdplay.exe;dvdSanta.exe;DXEffectTester.exe;DXEnum.exe;Easy RealMedia Tools.exe;")\
	_l("ehExtHost.exe;ehshell.exe;Encode360.exe;explorer.exe;fenglei.exe;ffmpeg.exe;filtermanager.exe;firefox.exe;Flash.exe;FLVPlayer4Free.exe;FMRadio.exe;Fortius.exe;FreeStyle.exe;FSViewer.exe;Funshion.exe;FusionHDTV.exe;")\
	_l("gameex.exe;GDivX Player.exe;gdsmux.exe;GoldWave.exe;gom.exe;GomEnc.exe;GoogleDesktop.exe;GoogleDesktopCrawl.exe;graphedit.exe;graphedt.exe;GraphStudio.exe;GraphStudio64.exe;gspot.exe;HBP.exe;HDVSplit.exe;honestechTV.exe;")\
	_l("HPWUCli.exe;i_view32.exe;ICQ.exe;ICQLite.exe;iexplore.exe;IHT.exe;IncMail.exe;InfoTool.exe;infotv.exe;InstallChecker.exe;Internet TV.exe;iPlayer.exe;ipod_video_converter.exe;IPODConverter.exe;")\
	_l("JetAudio.exe;jwBrowser.exe;kmplayer.exe;KwMusic.exe;LA.exe;LifeCam.exe;LifeFrame.exe;Lilith.exe;makeAVIS.exe;MatroskaDiag.exe;Maxthon.exe;MDirect.exe;Media Center 12.exe;Media Jukebox.exe;Media Player Classic.exe;")\
	_l("MediaLife.exe;MediaPortal.exe;MEDIAREVOLUTION.EXE;MediaServer.exe;megui.exe;mencoder.exe;Metacafe.exe;MMPlayer.exe;MovieMaker.exe;moviemk.exe;moviethumb.exe;MP4Converter.exe;Mp4Player.exe;mpcstar.exe;MpegVideoWizard.exe;")\
	_l("mplayer2.exe;mplayerc.exe;mplayerc64.exe;msnmsgr.exe;msoobe.exe;MultimediaPlayer.exe;Munite.exe;MusicManager.exe;Muzikbrowzer.exe;Mv2PlayerPlus.exe;My Movies.exe;myplayer.exe;nero.exe;NeroHome.exe;NeroVision.exe;")\
	_l("NicoPlayer.exe;NMSTranscoder.exe;nvplayer.exe;Omgjbox.exe;OnlineTV.exe;Opera.exe;OrbStreamerClient.exe;OUTLOOK.EXE;PaintDotNet.exe;paltalk.exe;pcwmp.exe;PhotoScreensaver.scr;Photoshop.exe;Picasa2.exe;")\
	_l("playwnd.exe;PowerDirector.exe;powerdvd.exe;POWERPNT.EXE;PPLive.exe;ppmate.exe;PPStream.exe;PQDVD_PSP.exe;Procoder2.exe;Producer.exe;progdvb.exe;ProgDvbNet.exe;PVCR.exe;Qonoha.exe;QQ.exe;QQLive.exe;")\
	_l("QQMusic.exe;QQPlayerSvr.exe;QvodPlayer.exe;QzoneMusic.exe;RadLight.exe;realplay.exe;ReClockHelper.dll;Recode.exe;RecordingManager.exe;rlkernel.exe;RoxMediaDB10.exe;RoxMediaDB9.exe;rundll32.exe;")\
	_l("Safari.exe;SelfMV.exe;Shareaza.exe;sherlock2.exe;ShowTime.exe;sidebar.exe;SinkuHadouken.exe;Sleipnir.exe;smartmovie.exe;songbird.exe;SopCast.exe;SplitCam.exe;START.EXE;stillcap.exe;Studio.exe;subedit.exe;")\
	_l("SubtitleEdit.exe;SubtitleWorkshop.exe;SubtitleWorkshop4.exe;SWFConverter.exe;telewizja.exe;TheaterTek DVD.exe;time_adjuster.exe;timecodec.exe;tmc.exe;TMPGEnc.exe;TMPGEnc4XP.exe;TOTALCMD.EXE;TSPlayer.exe;Tvants.exe;")\
	_l("tvc.exe;TVersity.exe;TVPlayer.exe;TVUPlayer.exe;UCC.exe;Ultra EDIT.exe;UUSeePlayer.exe;VCD_PLAY.EXE;VeohClient.exe;VFAPIFrameServer.exe;VideoConvert.exe;videoconverter.exe;videoenc.exe;VideoManager.exe;")\
	_l("VideoSnapshot.exe;VideoSplitter.exe;VIDEOS~1.SCR;VideoWave9.exe;ViPlay.exe;ViPlay3.exe;ViPlay4.exe;virtualdub.exe;virtualdubmod.exe;vplayer.exe;WaveChk.exe;WCreator.exe;WebMediaPlayer.exe;WFTV.exe;")\
	_l("winamp.exe;WinAVI 9.0.exe;WinAVI MP4 Converter.exe;WinAVI.exe;WindowsPhotoGallery.exe;windvd.exe;WinDvr.exe;WinMPGVideoConvert.exe;WINWORD.EXE;WLXPhotoGallery.exe;wmenc.exe;wmplayer.exe;wmprph.exe;")\
	_l("wscript.exe;x264.exe;XNVIEW.EXE;Xvid4PSP.exe;YahooMessenger.exe;YahooMusicEngine.exe;YahooWidgetEngine.exe;YahooWidgets.exe;zplayer.exe;Zune.exe;")
	
#define BLACKLIST_EXE_FILENAME \
	_l("age3.exe;AvatarMP.exe;BeachSoccer.exe;Bully.exe;cm2008.exe;cmr4.exe;cmr5.exe;conflictdeniedops.exe;conquer.exe;crazy_taxi_pc.exe;dark_horizon.exe;DaybreakDX.exe;dmc3se.exe;em3.exe;em4.exe;fable.exe;fallout3.exe;ff7.exe;fordorr.exe;game.exe;Grandia2.exe;gta-vc.exe;gta3.exe;gta_sa.exe;")\
	_l("gta-underground.exe;guildII.exe;hl.exe;hl2.exe;juiced.exe;juiced2_hin.exe;MassEffectLauncher.exe;maxpayne.exe;morrowind.exe;nations.exe;oblivion.exe;pes2008.exe;pes2009.exe;pes4.exe;pes5.exe;pes6.exe;powerdvd8.exe;premiere.exe;questviewer.exe;residentevil3.exe;")\
	_l("rf.exe;rf_online.bin;RomeGame.exe;rometw.exe;sacred.exe;sega rally.exe;sh3.exe;sh4.exe;sims.exe;tw2008.exe;twoworlds.exe;wa.exe;war3.exe;worms 4 mayhem.exe;worms3d.exe;")

struct TregOp;
struct Tconfig;
struct TglobalSettingsBase :public Toptions
{
private:
 void addToCompatiblityList(char_t *list, const char_t *exe, const char_t *delimit);
protected:
 const Tconfig *config;
 const char_t *reg_child;
 virtual void reg_op_codec(TregOp &t,TregOp *t2) {}
 void _reg_op_codec(short id,TregOp &tHKCU,TregOp *tHKLM,const char_t *name,int &val,int def);
 strings blacklistList,whitelistList;bool firstBlacklist,firstWhitelist;
public:
 TglobalSettingsBase(const Tconfig *Iconfig,int Imode,const char_t *Ireg_child,TintStrColl *Icoll);
 virtual ~TglobalSettingsBase() {}
 bool exportReg(bool all,const char_t *regflnm,bool unicode);
 int filtermode;
 int multipleInstances;
 int isBlacklist,isWhitelist;char_t blacklist[MAX_COMPATIBILITYLIST_LENGTH],whitelist[MAX_COMPATIBILITYLIST_LENGTH];
 virtual bool inBlacklist(const char_t *exe);
 virtual bool inWhitelist(const char_t *exe,IffdshowBase *Ideci);
 int addToROT;
 int allowedCPUflags;
 int compOnLoadMode;

 virtual void load(void);
 virtual void save(void);
 int trayIcon,trayIconType,trayIconExt,trayIconChanged;
 int isCompMgr,isCompMgrChanged;
 int allowDPRINTF,allowDPRINTFchanged;

 int outputdebug;
 int outputdebugfile;char_t debugfile[MAX_PATH];
 int errorbox;

 char_t dscalerPth[MAX_PATH];
};

struct TglobalSettingsDec :public TglobalSettingsBase
{
private:
 TglobalSettingsDec& operator =(const TglobalSettingsDec&);
protected:
 virtual void reg_op(TregOp &t);
 void fixMissing(int &codecId,int movie1,int movie2,int movie3);
 void fixMissing(int &codecId,int movie1,int movie2);
 void fixMissing(int &codecId,int movie);
 virtual int getDefault(int id);
 static void cleanupCodecsList(std::vector<CodecID> &ids,Tstrptrs &codecs);
 TglobalSettingsDec(const Tconfig *Iconfig,int Imode,const char_t *Ireg_child,TintStrColl *Icoll,TOSDsettings *Iosd);
public:
 char_t defaultPreset[MAX_PATH];
 int autoPreset,autoPresetFileFirst;

 int streamsMenu;
 TOSDsettings *osd;

 virtual void load(void);
 virtual void save(void);

 virtual CodecID getCodecId(DWORD fourCC,FOURCC *AVIfourCC) const =0;
 virtual const char_t** getFOURCClist(void) const=0;
 virtual void getCodecsList(Tstrptrs &codecs) const=0;
};

struct TglobalSettingsDecVideo :public TglobalSettingsDec
{
private:
 int forceInCSP;
 int needCodecFix;
 void fixNewCodecs(void);
 static const char_t *fourccs[];
 static const CodecID c_h264[IDFF_MOVIE_MAX+1], c_mpeg4[IDFF_MOVIE_MAX+1],c_mpeg1[IDFF_MOVIE_MAX+1],c_mpeg2[IDFF_MOVIE_MAX+1],c_theora[IDFF_MOVIE_MAX+1],c_wvc1[IDFF_MOVIE_MAX+1],c_wmv3[IDFF_MOVIE_MAX+1],c_wmv2[IDFF_MOVIE_MAX+1],c_wmv1[IDFF_MOVIE_MAX+1];
protected:
 virtual void reg_op(TregOp &t);
 virtual void reg_op_codec(TregOp &t,TregOp *t2);
 virtual int getDefault(int id);
public:
 int buildHistogram;
 int xvid,div3,mp4v,dx50,fvfw,mp43,mp42,mp41,h263,SVC,h264,vp5,vp6,vp6f,wmv1,wmv2,wvc1,wmv3,mss2,wvp2,cavs,rawv,mpg1,mpg2,mpegAVI,em2v,avrn,mjpg,dvsd,cdvc,hfyu,cyuv,theo,asv1,svq1,svq3,cram,rt21,iv32,cvid,rv10,ffv1,vp3,vcr1,rle,avis,mszh,zlib,flv1,_8bps,png1,qtrle,duck,tscc,qpeg,h261,loco,wnv1,cscd,zmbv,ulti,vixl,aasc,fps1,qtrpza,snow,rv40;
 int supdvddec;
 int fastMpeg2,fastH264,libtheoraPostproc;
 int alternateUncompressed;
 int autodetect24P;

 virtual void load(void);
 TOSDsettingsVideo osd;

 struct TsubtitlesSettings : Toptions
  {
  protected:
   virtual void getDefaultStr(int id,char_t *buf,size_t buflen);
  public:
   TsubtitlesSettings(TintStrColl *Icoll);
   char_t searchDir[2*MAX_PATH];char_t searchExt[2*MAX_PATH];
   int searchHeuristic;
   int watch;
   int textpin,textpinSSA;
  } sub;

 TglobalSettingsDecVideo(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll);
 virtual CodecID getCodecId(DWORD fourCC,FOURCC *AVIfourCC) const;
 virtual const char_t** getFOURCClist(void) const;
 virtual void getCodecsList(Tstrptrs &codecs) const;
};

struct TglobalSettingsDecAudio :public TglobalSettingsDec
{
private:
 static const char_t *wave_formats[];
 static const CodecID c_mp123[IDFF_MOVIE_MAX+1],c_ac3[IDFF_MOVIE_MAX+1],c_dts[IDFF_MOVIE_MAX+1],c_aac[IDFF_MOVIE_MAX+1],c_vorbis[IDFF_MOVIE_MAX+1];
protected:
 virtual void reg_op_codec(TregOp &t,TregOp *t2);
 virtual int getDefault(int id);
 virtual void getDefaultStr(int id,char_t *buf,size_t buflen);
public:
 TglobalSettingsDecAudio(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll,const char_t *Ireg_child=FFDSHOWDECAUDIO);
 TOSDsettingsAudio osd;
 int wma1,wma2,mp2,mp3,ac3,aac,amr,rawa,avis,iadpcm,msadpcm,flac,lpcm,dts,vorbis,law,gsm,tta,otherAdpcm,qdm2,truespeech,mace,ra,imc,atrac3,nellymoser,eac3,mlp;
 int dtsinwav;
 int ac3drc,dtsdrc;
 int SPDIFCompatibility;
 int showCurrentVolume;
 int showCurrentFFT;
 int firIsUserDisplayMaxFreq,firUserDisplayMaxFreq;
 int isAudioSwitcher;
 int alwaysextensible;
 int allowOutStream;
 int vorbisgain;
 char_t winamp2dir[MAX_PATH];
 virtual void load(void);
 virtual CodecID getCodecId(DWORD fourCC,FOURCC *AVIfourCC) const;
 virtual const char_t** getFOURCClist(void) const;
 virtual void getCodecsList(Tstrptrs &codecs) const;
};
struct TglobalSettingsDecAudioRaw :public TglobalSettingsDecAudio
{
 TglobalSettingsDecAudioRaw(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll):TglobalSettingsDecAudio(Iconfig,Imode,Icoll,FFDSHOWDECAUDIORAW) {}
};

struct TglobalSettingsEnc :public TglobalSettingsBase
{
protected:
 virtual int getDefault(int id);
public:
 TglobalSettingsEnc(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll);
 int psnr;
 int isDyInterlaced,dyInterlaced;
};

#endif
