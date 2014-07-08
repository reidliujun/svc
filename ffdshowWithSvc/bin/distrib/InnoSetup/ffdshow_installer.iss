; Requires Inno Setup (http://www.innosetup.com) and ISPP (http://sourceforge.net/projects/ispp/)

#define tryout_revision = '2524'
#define buildyear = '2011'
#define buildmonth = '09'
#define buildday = '09'

; Build specific options
#define localize = True

#define include_x264 = True
#define include_xvidcore = True
#define include_makeavis = True
#define include_plugin_avisynth = True
#define include_plugin_virtualdub = True
#define include_plugin_dscaler = True

#define include_info_before = False
#define include_gnu_license = False
#define include_setup_icon = False

; Compiler settings
#define is64bit = False

#define sse_required = False
#define sse2_required = False

; Output settings
#define filename_suffix = ''
#define outputdir = '.'

; Location of binaries
#define bindir = '..\..'

; Other
#define cpu_detection = True

; Custom builder preferences
#define PREF_CLSID = False
#define PREF_CLSID_ICL = False
#define PREF_CLSID_X64 = False
#define PREF_YAMAGATA = False
#define PREF_XXL = False
#define PREF_XXL_X64 = False
#define PREF_ALBAIN = False
#define PREF_ALBAIN_x64 = False

#if PREF_CLSID
  #define filename_suffix = '_clsid'
  #define bindir = '..\..\x86'
  #define outputdir = '..\..\..\..\'
#elif PREF_CLSID_ICL
  #define sse_required = True
  #define filename_suffix = '_clsid_sse_icl10'
  #define bindir = '..\..\x86'
  #define outputdir = '..\..\..\..\'
#elif PREF_CLSID_X64
  #define is64bit = True
  #define include_x264 = False
  #define include_xvidcore = False
  #define include_plugin_dscaler = False
  #define filename_suffix = '_clsid_x64'
  #define bindir = '..\..\x64'
  #define outputdir = '..\..\..\..\'
#elif PREF_YAMAGATA
  #define include_xvidcore = False
  #define filename_suffix = '_Q'
#elif PREF_XXL
  #define localize = False
  #define include_info_before = True
  #define include_setup_icon = True
  #define filename_suffix = '_xxl'
#elif PREF_XXL_X64
  #define is64bit = True
  #define include_x264 = False
  #define include_plugin_dscaler = False
  #define include_info_before = True
  #define include_setup_icon = True
  #define filename_suffix = '_xxl_x64'
#elif PREF_ALBAIN
  #define sse_required = False
  #define VS2008SP1 = True
  #define filename_suffix = '_dbt'
#elif PREF_ALBAIN_X64
  #define is64bit = True
  #define include_x264 = False
  #define include_plugin_dscaler = False
  #define filename_suffix = '_dbt_x64'
#endif

; Fail if no proper settings were chosen
#if is64bit & include_plugin_dscaler
There is no 64-bit version of DScaler.
#endif

[Setup]
AllowCancelDuringInstall=no
AllowNoIcons=yes
AllowUNCPath=no
#if is64bit
AppId=ffdshow64
#else
AppId=ffdshow
#endif
AppName=ffdshow
AppVerName=ffdshow [rev {#= tryout_revision}] [{#= buildyear}-{#= buildmonth}-{#= buildday}]
AppVersion=1.0
#if is64bit
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
#endif
Compression=lzma/ultra
InternalCompressLevel=ultra
SolidCompression=true
DefaultDirName={code:GetDefaultInstallDir|}
#if is64bit
DefaultGroupName=ffdshow64
#else
DefaultGroupName=ffdshow
#endif
DirExistsWarning=no
#if is64bit
MinVersion=0,5.01
#else
MinVersion=0,5.0
#endif
OutputBaseFilename=ffdshow_rev{#= tryout_revision}_{#= buildyear}{#= buildmonth}{#= buildday}{#= filename_suffix}
OutputDir={#= outputdir}
PrivilegesRequired=admin
#if include_setup_icon
SetupIconFile=..\modern-wizard_icon.ico
#endif
#if localize
ShowLanguageDialog=yes
#else
ShowLanguageDialog=no
#endif
ShowTasksTreeLines=yes
ShowUndisplayableLanguages=no
UsePreviousTasks=no
VersionInfoCompany=ffdshow
VersionInfoCopyright=GNU
VersionInfoVersion=1.0.0.{#= tryout_revision}
WizardImageFile=MicrosoftModern01.bmp
WizardSmallImageFile=SetupModernSmall26.bmp

[Languages]
#if !include_gnu_license & !include_info_before
Name: en; MessagesFile: compiler:Default.isl
  #if localize
Name: ba; MessagesFile: compiler:Languages\Basque.isl
Name: br; MessagesFile: compiler:Languages\BrazilianPortuguese.isl
Name: ca; MessagesFile: compiler:Languages\Catalan.isl
Name: cz; MessagesFile: compiler:Languages\Czech.isl
Name: da; MessagesFile: compiler:Languages\Danish.isl
Name: du; MessagesFile: compiler:Languages\Dutch.isl
Name: fi; MessagesFile: compiler:Languages\Finnish.isl
Name: fr; MessagesFile: compiler:Languages\French.isl
Name: de; MessagesFile: compiler:Languages\German.isl
Name: hu; MessagesFile: compiler:Languages\Hungarian.isl
Name: it; MessagesFile: compiler:Languages\Italian.isl
Name: jp; MessagesFile: languages\Japanese.isl
Name: no; MessagesFile: compiler:Languages\Norwegian.isl
Name: pl; MessagesFile: compiler:Languages\Polish.isl
Name: pr; MessagesFile: compiler:Languages\Portuguese.isl
Name: ru; MessagesFile: compiler:Languages\Russian.isl
Name: sc; MessagesFile: languages\ChineseSimp.isl
Name: sk; MessagesFile: compiler:Languages\Slovak.isl
Name: sn; MessagesFile: compiler:Languages\Slovenian.isl
Name: sp; MessagesFile: compiler:Languages\Spanish.isl
  #endif
#elif include_gnu_license & !include_info_before
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: gnu_license.txt
  #if localize
Name: ba; MessagesFile: compiler:Languages\Basque.isl
Name: br; MessagesFile: compiler:Languages\BrazilianPortuguese.isl
Name: ca; MessagesFile: compiler:Languages\Catalan.isl
Name: cz; MessagesFile: compiler:Languages\Czech.isl; LicenseFile: ../../../copying.cz.txt
Name: da; MessagesFile: compiler:Languages\Danish.isl
Name: du; MessagesFile: compiler:Languages\Dutch.isl
Name: fi; MessagesFile: compiler:Languages\Finnish.isl
Name: fr; MessagesFile: compiler:Languages\French.isl
Name: de; MessagesFile: compiler:Languages\German.isl; LicenseFile: ../../../copying.de.txt
Name: hu; MessagesFile: compiler:Languages\Hungarian.isl
Name: it; MessagesFile: compiler:Languages\Italian.isl
Name: jp; MessagesFile: languages\Japanese.isl; LicenseFile: ../../../copying.jp.txt
Name: no; MessagesFile: compiler:Languages\Norwegian.isl
Name: pl; MessagesFile: compiler:Languages\Polish.isl; LicenseFile: ../../../copying.pl.txt
Name: pr; MessagesFile: compiler:Languages\Portuguese.isl
Name: ru; MessagesFile: compiler:Languages\Russian.isl; LicenseFile: ../../../copying.ru.txt
Name: sc; MessagesFile: languages\ChineseSimp.isl
Name: sk; MessagesFile: compiler:Languages\Slovak.isl; LicenseFile: ../../../copying.sk.txt
Name: sn; MessagesFile: compiler:Languages\Slovenian.isl
Name: sp; MessagesFile: compiler:Languages\Spanish.isl
  #endif
#elif !include_gnu_license & include_info_before
Name: en; MessagesFile: compiler:Default.isl; InfoBeforeFile: infobefore.rtf
  #if localize
Name: ba; MessagesFile: compiler:Languages\Basque.isl
Name: br; MessagesFile: compiler:Languages\BrazilianPortuguese.isl
Name: ca; MessagesFile: compiler:Languages\Catalan.isl
Name: cz; MessagesFile: compiler:Languages\Czech.isl
Name: da; MessagesFile: compiler:Languages\Danish.isl
Name: du; MessagesFile: compiler:Languages\Dutch.isl
Name: fi; MessagesFile: compiler:Languages\Finnish.isl
Name: fr; MessagesFile: compiler:Languages\French.isl
Name: de; MessagesFile: compiler:Languages\German.isl; InfoBeforeFile: infobefore\infobefore.de.rtf
Name: hu; MessagesFile: compiler:Languages\Hungarian.isl
Name: it; MessagesFile: compiler:Languages\Italian.isl
Name: jp; MessagesFile: languages\Japanese.isl
Name: no; MessagesFile: compiler:Languages\Norwegian.isl
Name: pl; MessagesFile: compiler:Languages\Polish.isl; InfoBeforeFile: infobefore\infobefore.pl.rtf
Name: pr; MessagesFile: compiler:Languages\Portuguese.isl
Name: ru; MessagesFile: compiler:Languages\Russian.isl
Name: sc; MessagesFile: languages\ChineseSimp.isl
Name: sk; MessagesFile: compiler:Languages\Slovak.isl
Name: sn; MessagesFile: compiler:Languages\Slovenian.isl
Name: sp; MessagesFile: compiler:Languages\Spanish.isl
  #endif
#else
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: gnu_license.txt; InfoBeforeFile: infobefore.rtf
  #if localize
Name: ba; MessagesFile: compiler:Languages\Basque.isl
Name: br; MessagesFile: compiler:Languages\BrazilianPortuguese.isl
Name: ca; MessagesFile: compiler:Languages\Catalan.isl
Name: cz; MessagesFile: compiler:Languages\Czech.isl; LicenseFile: ../../../copying.cz.txt
Name: da; MessagesFile: compiler:Languages\Danish.isl
Name: du; MessagesFile: compiler:Languages\Dutch.isl
Name: fi; MessagesFile: compiler:Languages\Finnish.isl
Name: fr; MessagesFile: compiler:Languages\French.isl
Name: de; MessagesFile: compiler:Languages\German.isl; LicenseFile: ../../../copying.de.txt; InfoBeforeFile: infobefore\infobefore.de.rtf
Name: hu; MessagesFile: compiler:Languages\Hungarian.isl
Name: it; MessagesFile: compiler:Languages\Italian.isl
Name: jp; MessagesFile: languages\Japanese.isl; LicenseFile: ../../../copying.jp.txt
Name: no; MessagesFile: compiler:Languages\Norwegian.isl
Name: pl; MessagesFile: compiler:Languages\Polish.isl; LicenseFile: ../../../copying.pl.txt; InfoBeforeFile: infobefore\infobefore.pl.rtf
Name: pr; MessagesFile: compiler:Languages\Portuguese.isl
Name: ru; MessagesFile: compiler:Languages\Russian.isl; LicenseFile: ../../../copying.ru.txt
Name: sc; MessagesFile: languages\ChineseSimp.isl
Name: sk; MessagesFile: compiler:Languages\Slovak.isl; LicenseFile: ../../../copying.sk.txt
Name: sn; MessagesFile: compiler:Languages\Slovenian.isl
Name: sp; MessagesFile: compiler:Languages\Spanish.isl
  #endif
#endif

[Types]
Name: Normal; Description: Normal; Flags: iscustom

[Components]
Name: ffdshow; Description: {cm:comp_ffdshowds}; Types: Normal
Name: ffdshow\vfw; Description: {cm:comp_vfwInterface}; Types: Normal
#if include_makeavis
Name: ffdshow\makeavis; Description: {cm:comp_makeAvis}; Flags: dontinheritcheck
#endif
#if include_plugin_avisynth | include_plugin_virtualdub | include_plugin_dscaler
Name: ffdshow\plugins; Description: {cm:comp_appPlugins}; Flags: dontinheritcheck
#endif
#if include_plugin_avisynth
Name: ffdshow\plugins\avisynth; Description: AviSynth
#endif
#if include_plugin_virtualdub
Name: ffdshow\plugins\virtualdub; Description: VirtualDub
#endif
#if include_plugin_dscaler
Name: ffdshow\plugins\dscaler; Description: DScaler
#endif

; CPU detection code
#if cpu_detection
#include "innosetup_cpu_detection.iss"
#endif

[Tasks]
Name: resetsettings; Description: {cm:tsk_resetSettings}; GroupDescription: {cm:tsk_settings}; Components: ffdshow; Flags: unchecked
Name: video; Description: {cm:tsk_videoFormatsSelect}; GroupDescription: {cm:tsk_videoFormats}; Components: ffdshow; Flags: unchecked
Name: video\h264; Description: H.264 / AVC; Check: CheckTaskVideo(  'h264',  1, True); Components: ffdshow
Name: video\h264; Description: H.264 / AVC; Check: NOT  CheckTaskVideo(  'h264',  1, True); Components: ffdshow; Flags: unchecked
Name: video\divx; Description: DivX; Check: CheckTaskVideo2( 'dx50',     True); Components: ffdshow
Name: video\divx; Description: DivX; Check: NOT  CheckTaskVideo2( 'dx50',     True); Components: ffdshow; Flags: unchecked
Name: video\xvid; Description: Xvid; Check: CheckTaskVideo2( 'xvid',     True); Components: ffdshow
Name: video\xvid; Description: Xvid; Check: NOT  CheckTaskVideo2( 'xvid',     True); Components: ffdshow; Flags: unchecked
Name: video\mpeg4; Description: {cm:tsk_genericMpeg4}; Check: CheckTaskVideo2( 'mp4v',     True); Components: ffdshow
Name: video\mpeg4; Description: {cm:tsk_genericMpeg4}; Check: NOT  CheckTaskVideo2( 'mp4v',     True); Components: ffdshow; Flags: unchecked
Name: video\flv; Description: FLV1, FLV4; Check: CheckTaskVideo(  'flv1',  1, True); Components: ffdshow
Name: video\flv; Description: FLV1, FLV4; Check: NOT  CheckTaskVideo(  'flv1',  1, True); Components: ffdshow; Flags: unchecked
Name: video\h263; Description: H.263; Check: CheckTaskVideo(  'h263',  1, True); Components: ffdshow
Name: video\h263; Description: H.263; Check: NOT  CheckTaskVideo(  'h263',  1, True); Components: ffdshow; Flags: unchecked
Name: video\mpeg1; Description: MPEG-1; Components: ffdshow; Flags: unchecked
Name: video\mpeg1\libmpeg2; Description: libmpeg2; Check: CheckTaskVideo(  'mpg1',  5, False); Components: ffdshow; Flags: exclusive
Name: video\mpeg1\libmpeg2; Description: libmpeg2; Check: NOT  CheckTaskVideo(  'mpg1',  5, False); Components: ffdshow; Flags: exclusive unchecked
Name: video\mpeg1\libavcodec; Description: libavcodec; Check: CheckTaskVideo(  'mpg1',  1, False); Components: ffdshow; Flags: exclusive
Name: video\mpeg1\libavcodec; Description: libavcodec; Check: NOT  CheckTaskVideo(  'mpg1',  1, False); Components: ffdshow; Flags: exclusive unchecked
Name: video\mpeg2; Description: MPEG-2; Components: ffdshow; Flags: unchecked
Name: video\mpeg2\libmpeg2; Description: libmpeg2; Check: CheckTaskVideo(  'mpg2',  5, False); Components: ffdshow; Flags: exclusive
Name: video\mpeg2\libmpeg2; Description: libmpeg2; Check: NOT  CheckTaskVideo(  'mpg2',  5, False); Components: ffdshow; Flags: exclusive unchecked
Name: video\mpeg2\libavcodec; Description: libavcodec; Check: CheckTaskVideo(  'mpg2',  1, False); Components: ffdshow; Flags: exclusive
Name: video\mpeg2\libavcodec; Description: libavcodec; Check: NOT  CheckTaskVideo(  'mpg2',  1, False); Components: ffdshow; Flags: exclusive unchecked
Name: video\huffyuv; Description: Huffyuv; Check: CheckTaskVideo(  'hfyu',  1, True); Components: ffdshow
Name: video\huffyuv; Description: Huffyuv; Check: NOT  CheckTaskVideo(  'hfyu',  1, True); Components: ffdshow; Flags: unchecked
Name: video\qt; Description: SVQ1, SVQ3, Cinepak, RPZA, QTRLE; Check: CheckTaskVideo(  'svq3',  1, True); Components: ffdshow
Name: video\qt; Description: SVQ1, SVQ3, Cinepak, RPZA, QTRLE; Check: NOT  CheckTaskVideo(  'svq3',  1, True); Components: ffdshow; Flags: unchecked
Name: video\vp56; Description: VP5, VP6; Check: CheckTaskVideo(   'vp6',  1, True); Components: ffdshow
Name: video\vp56; Description: VP5, VP6; Check: NOT  CheckTaskVideo(   'vp6',  1, True); Components: ffdshow; Flags: unchecked
Name: video\vc1; Description: VC-1; Components: ffdshow; Flags: unchecked
Name: video\vc1\wmv9; Description: wmv9; Check: CheckTaskVideo(  'wvc1', 12, False); Components: ffdshow; Flags: exclusive
Name: video\vc1\wmv9; Description: wmv9; Check: NOT  CheckTaskVideo(  'wvc1', 12, False); Components: ffdshow; Flags: exclusive unchecked
Name: video\vc1\libavcodec; Description: libavcodec; Check: CheckTaskVideo(  'wvc1',  1, False); Components: ffdshow; Flags: exclusive
Name: video\vc1\libavcodec; Description: libavcodec; Check: NOT  CheckTaskVideo(  'wvc1',  1, False); Components: ffdshow; Flags: exclusive unchecked
Name: video\wmv1; Description: WMV1; Check: CheckTaskVideo2( 'wmv1',     False); Components: ffdshow
Name: video\wmv1; Description: WMV1; Check: NOT  CheckTaskVideo2( 'wmv1',     False); Components: ffdshow; Flags: dontinheritcheck unchecked
Name: video\wmv2; Description: WMV2; Check: CheckTaskVideo2( 'wmv2',     False); Components: ffdshow
Name: video\wmv2; Description: WMV2; Check: NOT  CheckTaskVideo2( 'wmv2',     False); Components: ffdshow; Flags: dontinheritcheck unchecked
Name: video\wmv3; Description: WMV3; Check: CheckTaskVideo2( 'wmv3',     False); Components: ffdshow
Name: video\wmv3; Description: WMV3; Check: NOT  CheckTaskVideo2( 'wmv3',     False); Components: ffdshow; Flags: dontinheritcheck unchecked
Name: video\wvp2; Description: WMVP, WVP2; Check: CheckTaskVideo(  'wvp2', 12, False); Components: ffdshow
Name: video\wvp2; Description: WMVP, WVP2; Check: NOT  CheckTaskVideo(  'wvp2', 12, False); Components: ffdshow; Flags: dontinheritcheck unchecked
Name: video\mss2; Description: MSS1, MSS2; Check: CheckTaskVideo(  'mss2', 12, False); Components: ffdshow
Name: video\mss2; Description: MSS1, MSS2; Check: NOT  CheckTaskVideo(  'mss2', 12, False); Components: ffdshow; Flags: dontinheritcheck unchecked
Name: video\dvsd; Description: DV; Check: CheckTaskVideo(  'dvsd',  1, False); Components: ffdshow
Name: video\dvsd; Description: DV; Check: NOT  CheckTaskVideo(  'dvsd',  1, False); Components: ffdshow; Flags: dontinheritcheck unchecked
Name: video\other1; Description: H.261, MJPEG, Theora, VP3; Check: NOT  IsUpdate; Components: ffdshow
Name: video\other2; Description: CorePNG, MS Video 1, MSRLE, Techsmith, Truemotion; Check: NOT  IsUpdate; Components: ffdshow
Name: video\other3; Description: ASV1/2, CYUV, ZLIB, 8BPS, LOCO, MSZH, QPEG, WNV1, VCR1; Check: NOT  IsUpdate; Components: ffdshow; Flags: unchecked
Name: video\other4; Description: CamStudio, ZMBV, Ultimotion, VIXL, AASC, IV32, FPS1, RT21; Check: NOT  IsUpdate; Components: ffdshow; Flags: unchecked
Name: video\rawv; Description: {cm:tsk_rawVideo}; Check: CheckTaskVideo(  'rawv',  1, False); Components: ffdshow; Flags: dontinheritcheck
Name: video\rawv; Description: {cm:tsk_rawVideo}; Check: NOT  CheckTaskVideo(  'rawv',  1, False); Components: ffdshow; Flags: dontinheritcheck unchecked
Name: audio; Description: {cm:tsk_audioFormatsSelect}; GroupDescription: {cm:tsk_audioFormats}; Components: ffdshow; Flags: unchecked
Name: audio\mp3; Description: MP3; Components: ffdshow; Flags: unchecked
Name: audio\mp3\libmad; Description: libmad; Check: CheckTaskAudio(   'mp3',  7, True); Components: ffdshow; Flags: exclusive
Name: audio\mp3\libmad; Description: libmad; Check: NOT  CheckTaskAudio(   'mp3',  7, True); Components: ffdshow; Flags: exclusive unchecked
Name: audio\mp3\libavcodec; Description: libavcodec; Check: CheckTaskAudio(   'mp3',  1, False); Components: ffdshow; Flags: exclusive
Name: audio\mp3\libavcodec; Description: libavcodec; Check: NOT  CheckTaskAudio(   'mp3',  1, False); Components: ffdshow; Flags: exclusive unchecked
Name: audio\aac; Description: AAC; Components: ffdshow; Flags: unchecked
Name: audio\aac\libfaad2; Description: libfaad2; Check: CheckTaskAudio(   'aac',  8, True); Components: ffdshow; Flags: exclusive
Name: audio\aac\libfaad2; Description: libfaad2; Check: NOT  CheckTaskAudio(   'aac',  8, True); Components: ffdshow; Flags: exclusive unchecked
;Name: audio\aac\libavcodec;    Description: libavcodec;                                                Check:      CheckTaskAudio(   'aac',  1, False);                                    Components: ffdshow; Flags: exclusive
;Name: audio\aac\libavcodec;    Description: libavcodec;                                                Check: NOT  CheckTaskAudio(   'aac',  1, False);                                    Components: ffdshow; Flags: exclusive unchecked
Name: audio\ac3; Description: AC3; Components: ffdshow; Flags: unchecked
Name: audio\ac3\liba52; Description: liba52; Check: CheckTaskAudio(   'ac3', 15, True) OR CheckTaskAudio('ac3', 16, True); Components: ffdshow; Flags: exclusive
Name: audio\ac3\liba52; Description: liba52; Check: NOT (CheckTaskAudio(   'ac3', 15, True) OR CheckTaskAudio('ac3', 16, True)); Components: ffdshow; Flags: exclusive unchecked
Name: audio\ac3\libavcodec; Description: libavcodec; Check: CheckTaskAudio(   'ac3',  1, False); Components: ffdshow; Flags: exclusive
Name: audio\ac3\libavcodec; Description: libavcodec; Check: NOT  CheckTaskAudio(   'ac3',  1, False); Components: ffdshow; Flags: exclusive unchecked
Name: audio\eac3; Description: EAC3 (Dolby Digital Plus); Check: CheckTaskAudio(  'eac3',  1, True); Components: ffdshow
Name: audio\eac3; Description: EAC3 (Dolby Digital Plus); Check: NOT  CheckTaskAudio(  'eac3',  1, True); Components: ffdshow; Flags: unchecked
Name: audio\mlp; Description: MLP/Dolby TrueHD; Check: CheckTaskAudio(   'mlp',  1, True); Components: ffdshow
Name: audio\mlp; Description: MLP/Dolby TrueHD; Check: NOT  CheckTaskAudio(   'mlp',  1, True); Components: ffdshow; Flags: unchecked
Name: audio\dts; Description: DTS; Components: ffdshow; Flags: unchecked
Name: audio\dts\libdts; Description: libdts; Check: CheckTaskAudio(   'dts', 17, True) OR CheckTaskAudio('dts', 17, True); Components: ffdshow; Flags: exclusive
Name: audio\dts\libdts; Description: libdts; Check: NOT (CheckTaskAudio(   'dts', 17, True) OR CheckTaskAudio('dts', 17, True)); Components: ffdshow; Flags: exclusive unchecked
Name: audio\dts\libavcodec; Description: libavcodec; Check: CheckTaskAudio(   'dts',  1, False); Components: ffdshow; Flags: exclusive
Name: audio\dts\libavcodec; Description: libavcodec; Check: NOT  CheckTaskAudio(   'dts',  1, False); Components: ffdshow; Flags: exclusive unchecked
Name: audio\lpcm; Description: LPCM; Check: CheckTaskAudio(  'lpcm',  4, True); Components: ffdshow
Name: audio\lpcm; Description: LPCM; Check: NOT  CheckTaskAudio(  'lpcm',  4, True); Components: ffdshow; Flags: unchecked
Name: audio\mp2; Description: MP1, MP2; Components: ffdshow; Flags: unchecked
Name: audio\mp2\libmad; Description: libmad; Check: CheckTaskAudio(   'mp2',  7, True); Components: ffdshow; Flags: exclusive
Name: audio\mp2\libmad; Description: libmad; Check: NOT  CheckTaskAudio(   'mp2',  7, True); Components: ffdshow; Flags: exclusive unchecked
Name: audio\mp2\libavcodec; Description: libavcodec; Check: CheckTaskAudio(   'mp2',  1, False); Components: ffdshow; Flags: exclusive
Name: audio\mp2\libavcodec; Description: libavcodec; Check: NOT  CheckTaskAudio(   'mp2',  1, False); Components: ffdshow; Flags: exclusive unchecked
Name: audio\vorbis; Description: Vorbis; Components: ffdshow; Flags: unchecked
Name: audio\vorbis\tremor; Description: tremor; Check: CheckTaskAudio('vorbis', 18, True); Components: ffdshow; Flags: exclusive
Name: audio\vorbis\tremor; Description: tremor; Check: NOT  CheckTaskAudio('vorbis', 18, True); Components: ffdshow; Flags: exclusive unchecked
Name: audio\vorbis\libavcodec; Description: libavcodec; Check: CheckTaskAudio('vorbis',  1, False); Components: ffdshow; Flags: exclusive
Name: audio\vorbis\libavcodec; Description: libavcodec; Check: NOT  CheckTaskAudio('vorbis',  1, False); Components: ffdshow; Flags: exclusive unchecked
Name: audio\flac; Description: FLAC; Check: CheckTaskAudio(  'flac',  1, False); Components: ffdshow
Name: audio\flac; Description: FLAC; Check: NOT  CheckTaskAudio(  'flac',  1, False); Components: ffdshow; Flags: unchecked
Name: audio\tta; Description: True Audio; Check: CheckTaskAudio(   'tta',  1, True); Components: ffdshow
Name: audio\tta; Description: True Audio; Check: NOT  CheckTaskAudio(   'tta',  1, True); Components: ffdshow; Flags: unchecked
Name: audio\amr; Description: AMR; Check: CheckTaskAudio(   'amr',  1, True); Components: ffdshow
Name: audio\amr; Description: AMR; Check: NOT  CheckTaskAudio(   'amr',  1, True); Components: ffdshow; Flags: unchecked
Name: audio\qt; Description: QDM2, MACE; Check: CheckTaskAudio(  'qdm2',  1, True); Components: ffdshow
Name: audio\qt; Description: QDM2, MACE; Check: NOT  CheckTaskAudio(  'qdm2',  1, True); Components: ffdshow; Flags: unchecked
Name: audio\adpcm; Description: ADPCM, MS GSM, Truespeech; Check: NOT  IsUpdate; Components: ffdshow; Flags: unchecked
Name: audio\rawa; Description: {cm:tsk_rawAudio}; Check: CheckTaskAudio(  'rawa',  4, False); Components: ffdshow; Flags: dontinheritcheck
Name: audio\rawa; Description: {cm:tsk_rawAudio}; Check: NOT  CheckTaskAudio(  'rawa',  4, False); Components: ffdshow; Flags: dontinheritcheck unchecked
Name: filter; Description: {cm:tsk_filtersSelect}; GroupDescription: {cm:tsk_filters}; Components: ffdshow; Flags: unchecked
Name: filter\passthroughac3; Description: {cm:tsk_passthroughac3}; Check: GetTaskPassthroughAC3(); Components: ffdshow
Name: filter\passthroughac3; Description: {cm:tsk_passthroughac3}; Check: NOT  GetTaskPassthroughAC3(); Components: ffdshow; Flags: unchecked
Name: filter\passthroughdts; Description: {cm:tsk_passthroughdts}; Check: GetTaskPassthroughDTS(); Components: ffdshow
Name: filter\passthroughdts; Description: {cm:tsk_passthroughdts}; Check: NOT  GetTaskPassthroughDTS(); Components: ffdshow; Flags: unchecked
Name: filter\normalize; Description: {cm:tsk_volumeNorm}; Check: GetTaskVolNormalize(); Components: ffdshow
Name: filter\normalize; Description: {cm:tsk_volumeNorm}; Check: NOT  GetTaskVolNormalize(); Components: ffdshow; Flags: unchecked
Name: filter\subtitles; Description: {cm:tsk_subtitles}; Check: CheckTaskVideoInpreset('issubtitles', 1, False); Components: ffdshow
Name: filter\subtitles; Description: {cm:tsk_subtitles}; Check: NOT  CheckTaskVideoInpreset('issubtitles', 1, False); Components: ffdshow; Flags: unchecked
#if !PREF_YAMAGATA
Name: skiph264inloop; Description: {cm:tsk_skipInloop}; Check: NOT  IsUpdate; GroupDescription: {cm:tsk_tweaks}; Components: ffdshow; Flags: unchecked
#endif
Name: whitelist; Description: {cm:tsk_whitelist}; Check: NOT  IsUpdate; GroupDescription: {cm:tsk_compatibilityManager}; Components: ffdshow; Flags: unchecked
Name: whitelist\prompt; Description: {cm:tsk_whitelistPrompt}; Components: ffdshow; Flags: unchecked

[Icons]
#if is64bit
Name: {group}\{cm:shrt_audioConfig}; Filename: {sys}\rundll32.exe; Parameters: ffdshow.ax,configureAudio; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 4; Components: ffdshow
Name: {group}\{cm:shrt_videoConfig}; Filename: {sys}\rundll32.exe; Parameters: ffdshow.ax,configure; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 3; Components: ffdshow
Name: {group}\{cm:shrt_vfwConfig}; Filename: {sys}\rundll32.exe; Parameters: ff_vfw.dll,configureVFW; WorkingDir: {sys}; IconFilename: {app}\ffdshow.ax; IconIndex: 5; Components: ffdshow\vfw
#else
Name: {group}\{cm:shrt_audioConfig}; Filename: {syswow64}\rundll32.exe; Parameters: ffdshow.ax,configureAudio; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 4; Components: ffdshow; MinVersion: 0,4
Name: {group}\{cm:shrt_audioConfig}; Filename: {win}\rundll32.exe; Parameters: ffdshow.ax,configureAudio; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 4; Components: ffdshow; MinVersion: 4,0
Name: {group}\{cm:shrt_videoConfig}; Filename: {syswow64}\rundll32.exe; Parameters: ffdshow.ax,configure; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 3; Components: ffdshow; MinVersion: 0,4
Name: {group}\{cm:shrt_videoConfig}; Filename: {win}\rundll32.exe; Parameters: ffdshow.ax,configure; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 3; Components: ffdshow; MinVersion: 4,0
Name: {group}\{cm:shrt_vfwConfig}; Filename: {syswow64}\rundll32.exe; Parameters: ff_vfw.dll,configureVFW; WorkingDir: {syswow64}; IconFilename: {app}\ffdshow.ax; IconIndex: 5; Components: ffdshow\vfw; MinVersion: 0,4
Name: {group}\{cm:shrt_vfwConfig}; Filename: {win}\rundll32.exe; Parameters: ff_vfw.dll,configureVFW; WorkingDir: {sys}; IconFilename: {app}\ffdshow.ax; IconIndex: 5; Components: ffdshow\vfw; MinVersion: 4,0
#endif
#if include_makeavis
Name: {group}\makeAVIS; Filename: {app}\makeAVIS.exe; Components: ffdshow\makeavis
#endif
Name: {group}\{cm:shrt_uninstall}; Filename: {uninstallexe}

[Files]
; For speaker config
Source: ffSpkCfg.dll; Flags: dontcopy
Source: {#= bindir}\OpenSVCDecoder.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\libavcodec.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\libmplayer.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\ff_liba52.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\ff_libdts.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\ff_libfaad2.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\ff_libmad.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\ff_tremor.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\ff_unrar.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\ff_samplerate.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\ff_theora.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
#if include_x264
Source: {#= bindir}\ff_x264.dll; DestDir: {app}; Components: ffdshow\vfw; Flags: ignoreversion
  #if !is64bit
Source: Runtimes\pthreadGC2\x86\pthreadGC2.dll; DestDir: {sys}; Components: ffdshow\vfw; Flags: sharedfile uninsnosharedfileprompt restartreplace uninsrestartdelete
  #else
Source: Runtimes\pthreadGC2\x64\pthreadGC2-64.dll; DestDir: {sys}; Components: ffdshow\vfw; Flags: sharedfile uninsnosharedfileprompt restartreplace uninsrestartdelete
  #endif
#endif
#if include_xvidcore
Source: {#= bindir}\xvidcore.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
#endif
Source: {#= bindir}\distrib\InnoSetup\xxl\ff_kernelDeint\x86\ff_kernelDeint.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\distrib\InnoSetup\xxl\TomsMoComp_ff\x86\TomsMoComp_ff.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion
Source: {#= bindir}\libmpeg2_ff.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion restartreplace uninsrestartdelete

; Single build:
#if !PREF_CLSID_ICL
Source: {#= bindir}\ffdshow.ax; DestDir: {app}; Components: ffdshow; Flags: ignoreversion regserver restartreplace uninsrestartdelete noregerror
#endif
#if PREF_CLSID_ICL
Source: {#= bindir}\ffdshow_icl.ax; DestName: ffdshow.ax; DestDir: {app}; Components: ffdshow; Flags: ignoreversion regserver restartreplace uninsrestartdelete noregerror; MinVersion: 0,4
#endif
; Multi build example (requires cpu detection to be enabled):
;Source: {#= bindir}\ffdshow_generic.ax; DestName: ffdshow.ax; DestDir: {app}; Flags: ignoreversion regserver restartreplace uninsrestartdelete; Check: Is_MMX_Supported AND NOT Is_SSE_Supported;  Components: ffdshow
;Source: {#= bindir}\ffdshow_sse.ax;     DestName: ffdshow.ax; DestDir: {app}; Flags: ignoreversion regserver restartreplace uninsrestartdelete; Check: Is_SSE_Supported AND NOT Is_SSE2_Supported; Components: ffdshow
;Source: {#= bindir}\ffdshow_sse2.ax;    DestName: ffdshow.ax; DestDir: {app}; Flags: ignoreversion regserver restartreplace uninsrestartdelete; Check: Is_SSE2_Supported;                          Components: ffdshow

Source: {#= bindir}\ff_wmv9.dll; DestDir: {app}; Components: ffdshow; Flags: ignoreversion

Source: {#= bindir}\ff_vfw.dll; DestDir: {sys}; Components: ffdshow\vfw; Flags: ignoreversion restartreplace uninsrestartdelete

#if include_plugin_avisynth
Source: ..\..\ffavisynth.avsi; DestDir: {code:GetAviSynthPluginDir}; Components: ffdshow\plugins\avisynth; Flags: ignoreversion restartreplace uninsrestartdelete
Source: {#= bindir}\ffavisynth.dll; DestDir: {code:GetAviSynthPluginDir}; Components: ffdshow\plugins\avisynth; Flags: ignoreversion restartreplace uninsrestartdelete
#endif
#if include_plugin_virtualdub
Source: {#= bindir}\ffvdub.vdf; DestDir: {code:GetVdubPluginDir}; Components: ffdshow\plugins\virtualdub; Flags: ignoreversion restartreplace uninsrestartdelete
#endif
#if include_plugin_dscaler
Source: {#= bindir}\FLT_ffdshow.dll; DestDir: {code:GetDScalerDir|}; Components: ffdshow\plugins\dscaler; Flags: ignoreversion restartreplace uninsrestartdelete
#endif

#if include_makeavis
Source: {#= bindir}\makeAVIS.exe; DestDir: {app}; Components: ffdshow\makeavis; Flags: ignoreversion restartreplace uninsrestartdelete
Source: {#= bindir}\ff_acm.acm; DestDir: {sys}; Components: ffdshow\makeavis; Flags: ignoreversion restartreplace uninsrestartdelete
#endif

Source: ..\..\languages\*.*; DestDir: {app}\languages; Components: ffdshow; Flags: ignoreversion
Source: ..\..\custom matrices\*.*; DestDir: {app}\custom matrices; Components: ffdshow\vfw; Flags: ignoreversion
Source: ..\..\openIE.js; DestDir: {app}; Components: ffdshow; Flags: ignoreversion

#if is64bit
Source: ..\..\manifest64\ffdshow.ax.manifest; DestDir: {app}; Flags: ignoreversion restartreplace uninsrestartdelete; MinVersion: 0,5.01; OnlyBelowVersion: 0,5.03; Components: ffdshow
Source: ..\..\manifest64\ff_vfw.dll.manifest; DestDir: {sys}; Flags: ignoreversion restartreplace uninsrestartdelete; MinVersion: 0,5.01; OnlyBelowVersion: 0,5.03; Components: ffdshow\vfw
  #if include_makeavis
Source: ..\..\manifest64\makeAVIS.exe.manifest; DestDir: {app}; Flags: ignoreversion restartreplace uninsrestartdelete; MinVersion: 0,5.01; OnlyBelowVersion: 0,5.03; Components: ffdshow\makeavis
  #endif
#else
Source: ..\..\manifest32\ffdshow.ax.manifest; DestDir: {app}; Flags: ignoreversion restartreplace uninsrestartdelete; MinVersion: 0,5.01; OnlyBelowVersion: 0,5.03; Components: ffdshow
Source: ..\..\manifest32\ff_vfw.dll.manifest; DestDir: {sys}; Flags: ignoreversion restartreplace uninsrestartdelete; MinVersion: 0,5.01; OnlyBelowVersion: 0,5.03; Components: ffdshow\vfw
  #if include_makeavis
Source: ..\..\manifest32\makeAVIS.exe.manifest; DestDir: {app}; Flags: ignoreversion restartreplace uninsrestartdelete; MinVersion: 0,5.01; OnlyBelowVersion: 0,5.03; Components: ffdshow\makeavis
  #endif
#endif

[InstallDelete]
; Private assemblies
Type: files; Name: {app}\msvcr80.dll; Components: ffdshow
Type: files; Name: {app}\microsoft.vc80.crt.manifest; Components: ffdshow
Type: files; Name: {app}\languages\ffdshow.1041.jp; Components: ffdshow
#if localize
; Localized shortcuts
Type: files; Name: {group}\Video decoder configuration.lnk; Components: ffdshow
Type: files; Name: {group}\Audio decoder configuration.lnk; Components: ffdshow
Type: files; Name: {group}\Uninstall ffdshow.lnk; Components: ffdshow
Type: files; Name: {group}\VFW configuration.lnk; Components: ffdshow\vfw
#endif
#if !include_x264
Type: files; Name: {app}\ff_x264.dll; Components: ffdshow
#endif
#if !include_xvidcore
Type: files; Name: {app}\xvidcore.dll; Components: ffdshow
#endif


[Registry]
; Cleanup of settings
Root: HKCU; Subkey: Software\GNU; Flags: uninsdeletekeyifempty; Components: ffdshow
Root: HKCU; Subkey: Software\GNU\ffdshow; Flags: uninsdeletekey; Components: ffdshow
Root: HKCU; Subkey: Software\GNU\ffdshow_audio; Flags: uninsdeletekey; Components: ffdshow
Root: HKCU; Subkey: Software\GNU\ffdshow_audio_raw; Flags: uninsdeletekey; Components: ffdshow
Root: HKCU; Subkey: Software\GNU\ffdshow_enc; Flags: uninsdeletekey; Components: ffdshow\vfw
Root: HKCU; Subkey: Software\GNU\ffdshow_vfw; Flags: uninsdeletekey; Components: ffdshow\vfw
Root: HKLM; Subkey: Software\GNU; Flags: uninsdeletekeyifempty; Components: ffdshow
Root: HKLM; Subkey: Software\GNU\ffdshow; Flags: uninsdeletekey; Components: ffdshow
Root: HKLM; Subkey: Software\GNU\ffdshow_audio; Flags: uninsdeletekey; Components: ffdshow
Root: HKLM; Subkey: Software\GNU\ffdshow_enc; Flags: uninsdeletekey; Components: ffdshow\vfw
Root: HKLM; Subkey: Software\GNU\ffdshow_vfw; Flags: uninsdeletekey; Components: ffdshow\vfw

; Reset settings
Root: HKCU; Subkey: Software\GNU\ffdshow; Flags: deletekey; Components: ffdshow; Tasks: resetsettings
Root: HKCU; Subkey: Software\GNU\ffdshow_audio; Flags: deletekey; Components: ffdshow; Tasks: resetsettings
Root: HKCU; Subkey: Software\GNU\ffdshow_audio_raw; Flags: deletekey; Components: ffdshow; Tasks: resetsettings
Root: HKCU; Subkey: Software\GNU\ffdshow_enc; Flags: deletekey; Components: ffdshow\vfw; Tasks: resetsettings
Root: HKCU; Subkey: Software\GNU\ffdshow_vfw; Flags: deletekey; Components: ffdshow\vfw; Tasks: resetsettings
Root: HKLM; Subkey: Software\GNU\ffdshow; Flags: deletekey; Components: ffdshow; Tasks: resetsettings
Root: HKLM; Subkey: Software\GNU\ffdshow_audio; Flags: deletekey; Components: ffdshow; Tasks: resetsettings
Root: HKLM; Subkey: Software\GNU\ffdshow_enc; Flags: deletekey; Components: ffdshow\vfw; Tasks: resetsettings
Root: HKLM; Subkey: Software\GNU\ffdshow_vfw; Flags: deletekey; Components: ffdshow\vfw; Tasks: resetsettings

; Path
Root: HKLM; Subkey: Software\GNU\ffdshow; ValueType: string; ValueName: pth; ValueData: {app}; Components: ffdshow
Root: HKLM; Subkey: Software\GNU\ffdshow; ValueName: pthPriority; Components: ffdshow; Flags: deletevalue
#if include_plugin_avisynth
Root: HKLM; SubKey: Software\GNU\ffdshow; ValueType: string; ValueName: pthAvisynth; ValueData: {code:GetAviSynthPluginDir}; Flags: uninsclearvalue; Components: ffdshow\plugins\avisynth
#endif
#if include_plugin_virtualdub
Root: HKLM; SubKey: Software\GNU\ffdshow; ValueType: string; ValueName: pthVirtualDub; ValueData: {code:GetVdubPluginDir}; Flags: uninsclearvalue; Components: ffdshow\plugins\virtualdub
#endif
#if include_plugin_dscaler
Root: HKLM; SubKey: Software\GNU\ffdshow; ValueType: string; ValueName: dscalerPth; ValueData: {code:GetDScalerDir|}; Flags: uninsclearvalue; Components: ffdshow\plugins\dscaler
#endif

; Version info
Root: HKLM; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: revision; ValueData: {#= tryout_revision}; Components: ffdshow
Root: HKLM; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: builddate; ValueData: {#= buildyear}{#= buildmonth}{#= buildday}; Components: ffdshow

; Language
#if localize
Root: HKLM; Subkey: Software\GNU\ffdshow; ValueType: string; ValueName: lang; ValueData: {cm:langid}; Components: ffdshow
#endif

; Register VFW interface
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows NT\CurrentVersion\drivers.desc; ValueType: string; ValueName: ff_vfw.dll; ValueData: ffdshow video encoder; MinVersion: 0,4; Flags: uninsdeletevalue; Components: ffdshow\vfw
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32; ValueType: string; ValueName: VIDC.FFDS; ValueData: ff_vfw.dll; MinVersion: 0,4; Flags: uninsdeletevalue; Components: ffdshow\vfw
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS; MinVersion: 0,4; Flags: uninsdeletekey; Components: ffdshow\vfw
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS; ValueType: string; ValueName: Description; ValueData: ffdshow video encoder; MinVersion: 0,4; Components: ffdshow\vfw
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS; ValueType: string; ValueName: Driver; ValueData: ff_vfw.dll; MinVersion: 0,4; Components: ffdshow\vfw
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS; ValueType: string; ValueName: FriendlyName; ValueData: ffdshow video encoder; MinVersion: 0,4; Components: ffdshow\vfw

#if include_makeavis
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows NT\CurrentVersion\drivers.desc; ValueType: string; ValueName: ff_acm.acm; ValueData: ffdshow ACM codec; MinVersion: 0,4; Flags: uninsdeletevalue; Components: ffdshow\makeavis
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32; ValueType: string; ValueName: msacm.avis; ValueData: ff_acm.acm; MinVersion: 0,4; Flags: uninsdeletevalue; Components: ffdshow\makeavis
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis; MinVersion: 0,4; Flags: uninsdeletekey; Components: ffdshow\makeavis
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis; ValueType: string; ValueName: Description; ValueData: ffdshow ACM codec; MinVersion: 0,4; Components: ffdshow\makeavis
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis; ValueType: string; ValueName: Driver; ValueData: ff_acm.acm; MinVersion: 0,4; Components: ffdshow\makeavis
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis; ValueType: string; ValueName: FriendlyName; ValueData: ffdshow ACM codec; MinVersion: 0,4; Components: ffdshow\makeavis
#endif

; Recommended settings
Root: HKCU; Subkey: Software\GNU\ffdshow\default; ValueType: dword; ValueName: postprocH264mode; ValueData: 0; Components: ffdshow; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: Software\GNU\ffdshow\default; ValueType: dword; ValueName: resizeMethod; ValueData: 9; Components: ffdshow; Flags: createvalueifdoesntexist

#if !PREF_YAMAGATA
Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: fastH264; ValueData: 0; Components: ffdshow; Flags: createvalueifdoesntexist; Tasks: NOT skiph264inloop
Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: fastH264; ValueData: 2; Components: ffdshow; Tasks: skiph264inloop
#endif

Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: subTextpin; ValueData: 1; Components: ffdshow; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: subTextpinSSA; ValueData: 1; Components: ffdshow; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: Software\GNU\ffdshow\default; ValueType: dword; ValueName: subIsExpand; ValueData: 0; Components: ffdshow; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: Software\GNU\ffdshow\default; ValueType: dword; ValueName: isSubtitles; ValueData: 0; Components: ffdshow; Tasks: NOT filter\subtitles
Root: HKCU; Subkey: Software\GNU\ffdshow\default; ValueType: dword; ValueName: isSubtitles; ValueData: 1; Components: ffdshow; Tasks: filter\subtitles

Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: mixerNormalizeMatrix; ValueData: 0; Components: ffdshow; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: volNormalize; ValueData: 0; Components: ffdshow; Tasks: NOT filter\normalize
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: isvolume; ValueData: 1; Components: ffdshow; Tasks: filter\normalize
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: volNormalize; ValueData: 1; Components: ffdshow; Tasks: filter\normalize

Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: ismixer; ValueData: {code:GetIsMixer}; Components: ffdshow
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: mixerOut; ValueData: {code:GetMixerOut}; Components: ffdshow

; Blacklist
Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: isBlacklist; ValueData: 1; Flags: createvalueifdoesntexist; Components: ffdshow
Root: HKCU; Subkey: Software\GNU\ffdshow_audio; ValueType: dword; ValueName: isBlacklist; ValueData: 1; Flags: createvalueifdoesntexist; Components: ffdshow

; Compatibility list
Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: isWhitelist; ValueData: 0; Components: ffdshow; Tasks: NOT whitelist; Check: NOT IsUpdate
Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: isWhitelist; ValueData: 1; Components: ffdshow; Tasks: whitelist; Check: NOT IsUpdate
Root: HKCU; Subkey: Software\GNU\ffdshow_audio; ValueType: dword; ValueName: isWhitelist; ValueData: 0; Components: ffdshow; Tasks: NOT whitelist; Check: NOT IsUpdate
Root: HKCU; Subkey: Software\GNU\ffdshow_audio; ValueType: dword; ValueName: isWhitelist; ValueData: 1; Components: ffdshow; Tasks: whitelist; Check: NOT IsUpdate

Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: dontaskComp; ValueData: 0; Components: ffdshow; Tasks: whitelist AND NOT whitelist\prompt
Root: HKCU; Subkey: Software\GNU\ffdshow; ValueType: dword; ValueName: dontaskComp; ValueData: 1; Components: ffdshow; Tasks: whitelist AND whitelist\prompt
Root: HKCU; Subkey: Software\GNU\ffdshow_audio; ValueType: dword; ValueName: dontaskComp; ValueData: 0; Components: ffdshow; Tasks: whitelist AND NOT whitelist\prompt
Root: HKCU; Subkey: Software\GNU\ffdshow_audio; ValueType: dword; ValueName: dontaskComp; ValueData: 1; Components: ffdshow; Tasks: whitelist AND whitelist\prompt

; Registry keys for the audio/video formats:
#include "reg_formats.iss"

; Audio pass-through upgrade path:
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: passthroughAC3; ValueData: 0; Components: ffdshow; Tasks: NOT filter\passthroughac3
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: passthroughAC3; ValueData: 1; Components: ffdshow; Tasks: filter\passthroughac3
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: passthroughDTS; ValueData: 0; Components: ffdshow; Tasks: NOT filter\passthroughdts
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: passthroughDTS; ValueData: 1; Components: ffdshow; Tasks: filter\passthroughdts
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\; ValueName: ac3SPDIF; Components: ffdshow; Flags: deletevalue

; DRC upgrade path:
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: decoderDRC; ValueData: 1; Components: ffdshow; Check: GetDecoderDRC
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\default; ValueType: dword; ValueName: decoderDRC; ValueData: 0; Components: ffdshow; Check: NOT GetDecoderDRC
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\; ValueName: ac3DRC; Components: ffdshow; Flags: deletevalue
Root: HKCU; Subkey: Software\GNU\ffdshow_audio\; ValueName: dtsDRC; Components: ffdshow; Flags: deletevalue

[Run]
Description: {cm:run_audioConfig}; Filename: {syswow64}\rundll32.exe; Parameters: ffdshow.ax,configureAudio; WorkingDir: {app}; Flags: postinstall nowait unchecked; MinVersion: 0,4; Components: ffdshow
Description: {cm:run_audioConfig}; Filename: {win}\rundll32.exe; Parameters: ffdshow.ax,configureAudio; WorkingDir: {app}; Flags: postinstall nowait unchecked; MinVersion: 4,0; Components: ffdshow
Description: {cm:run_videoConfig}; Filename: {syswow64}\rundll32.exe; Parameters: ffdshow.ax,configure; WorkingDir: {app}; Flags: postinstall nowait unchecked; MinVersion: 0,4; Components: ffdshow
Description: {cm:run_videoConfig}; Filename: {win}\rundll32.exe; Parameters: ffdshow.ax,configure; WorkingDir: {app}; Flags: postinstall nowait unchecked; MinVersion: 4,0; Components: ffdshow
#if is64bit
Description: {cm:run_vfwConfig}; Filename: {sys}\rundll32.exe; Parameters: ff_vfw.dll,configureVFW; WorkingDir: {sys}; Flags: postinstall nowait unchecked; Components: ffdshow\vfw
#else
Description: {cm:run_vfwConfig}; Filename: {syswow64}\rundll32.exe; Parameters: ff_vfw.dll,configureVFW; WorkingDir: {syswow64}; Flags: postinstall nowait unchecked; MinVersion: 0,4; Components: ffdshow\vfw
Description: {cm:run_vfwConfig}; Filename: {win}\rundll32.exe; Parameters: ff_vfw.dll,configureVFW; WorkingDir: {sys}; Flags: postinstall nowait unchecked; MinVersion: 4,0; Components: ffdshow\vfw
#endif

; All custom strings in the installer:
#include "custom_messages.iss"

[Code]
// Global vars
var
  is_update: Boolean;
  reg_mixerOut: Cardinal;
  reg_ismixer: Cardinal;
  SpeakerPage: TInputOptionWizardPage;
  is8DisableMixer: Boolean;


function CheckTaskVideo(name: String; value: Integer; showbydefault: Boolean): Boolean;
var
  regval: Cardinal;
begin
  Result := False;
  if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow', name, regval) then begin
    Result := (regval = value);
  end
  else begin
    if RegQueryDwordValue(HKLM, 'Software\GNU\ffdshow', name, regval) then begin
      Result := (regval = value);
    end
    else begin
      Result := showbydefault;
    end
  end
end;

function CheckTaskVideo2(name: String; showbydefault: Boolean): Boolean;
var
  regval: Cardinal;
begin
  Result := False;
  if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow', name, regval) then begin
    Result := (regval > 0);
  end
  else begin
    if RegQueryDwordValue(HKLM, 'Software\GNU\ffdshow', name, regval) then begin
      Result := (regval > 0);
    end
    else begin
      Result := showbydefault;
    end
  end
end;

function CheckTaskAudio(name: String; value: Integer; showbydefault: Boolean): Boolean;
var
  regval: Cardinal;
begin
  Result := False;
  if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow_audio', name, regval) then begin
    Result := (regval = value);
  end
  else begin
    if RegQueryDwordValue(HKLM, 'Software\GNU\ffdshow_audio', name, regval) then begin
      Result := (regval = value);
    end
    else begin
      Result := showbydefault;
    end
  end
end;

function CheckTaskVideoInpreset(name: String; value: Integer; showbydefault: Boolean): Boolean;
var
  regval: Cardinal;
begin
  Result := False;
  if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow\default', name, regval) then
    Result := (regval = value)
  else
    Result := showbydefault;
end;

function CheckTaskAudioInpreset(name: String; value: Integer; showbydefault: Boolean): Boolean;
var
  regval: Cardinal;
begin
  Result := False;
  if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow_audio\default', name, regval) then
    Result := (regval = value)
  else
    Result := showbydefault;
end;

function GetTaskVolNormalize(): Boolean;
begin
  Result := False;
  if CheckTaskAudioInpreset('isvolume', 1, False) then
    if CheckTaskAudioInpreset('volNormalize', 1, False) then
     Result := True;
end;

function GetTaskPassthroughAC3(): Boolean;
var
  regval: Cardinal;
  presetList: TArrayOfString;
  index: Integer;
begin
  Result := False;
  if CheckTaskAudioInpreset('passthroughAC3', 1, False) then
    begin
  	  Result := True;
    end;
  if CheckTaskAudio('ac3', 16, False) then
    begin
      Result := True;
    end
  if RegGetSubkeyNames(HKCU, 'Software\GNU\ffdshow_audio\', presetList) then
    begin
      for index := 0 to GetArrayLength(presetList)-1 do
        begin
          if (presetList[index] <> 'default') then
            begin
              if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow_audio\' + presetList[index], 'outsfs', regval) then
                begin
                  // the second condition equals (regval & 16 > 0)
                  if (Result) OR (CheckTaskAudio('ac3SPDIF', 1, False) AND (((regval / 16) MOD 2) = 1)) then
                    begin
                      RegWriteDWordValue(HKCU, 'Software\GNU\ffdshow_audio\' + presetList[index], 'passthroughAC3', 1)
                    end
                end
            end
        end
    end
  if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow_audio\default', 'outsfs', regval) then
  begin
    // the second condition equals (regval & 16 > 0)
    if CheckTaskAudio('ac3SPDIF', 1, False) AND (((regval / 16) MOD 2) = 1) then
      begin
        Result := True;
      end
  end
end;

function GetTaskPassthroughDTS(): Boolean;
var
  regval: Cardinal;
  presetList: TArrayOfString;
  index: Integer;
begin
  Result := False;
  if CheckTaskAudioInpreset('passthroughDTS', 1, False) then
    begin
  	  Result := True;
    end;
  if CheckTaskAudio('dts', 16, False) then
    begin
      Result := True;
    end
  if RegGetSubkeyNames(HKCU, 'Software\GNU\ffdshow_audio\', presetList) then
    begin
      for index := 0 to GetArrayLength(presetList)-1 do
        begin
          if (presetList[index] <> 'default') then
            begin
              if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow_audio\' + presetList[index], 'outsfs', regval) then
                begin
                  // the second condition equals (regval & 16 > 0)
                  if (Result) OR (CheckTaskAudio('ac3SPDIF', 1, False) AND (((regval / 16) MOD 2) = 1)) then
                    begin
                      RegWriteDWordValue(HKCU, 'Software\GNU\ffdshow_audio\' + presetList[index], 'passthroughDTS', 1)
                    end
                end
            end
        end
    end
  if RegQueryDwordValue(HKCU, 'Software\GNU\ffdshow_audio\default', 'outsfs', regval) then
  begin
    // the second condition equals (regval & 16 > 0)
    if CheckTaskAudio('ac3SPDIF', 1, False) AND (((regval / 16) MOD 2) = 1) then
      begin
        Result := True;
      end
  end
end;

function GetDecoderDRC(): Boolean;
var
  presetList: TArrayOfString;
  index: Integer;
begin
  Result := False;
  if CheckTaskAudio('ac3DRC', 1, False) then
    begin
      Result := True;
    end
  if CheckTaskAudio('dtsDRC', 1, False) then
    begin
      Result := True;
    end
if (Result) then
  begin
    if RegGetSubkeyNames(HKCU, 'Software\GNU\ffdshow_audio\', presetList) then
      begin
        for index := 0 to GetArrayLength(presetList)-1 do
          begin
            if (presetList[index] <> 'default') then
              begin
                RegWriteDWordValue(HKCU, 'Software\GNU\ffdshow_audio\' + presetList[index], 'decoderDRC', 1)
              end
          end
      end
  end
  if CheckTaskAudioInpreset('decoderDRC', 1, False) then
    begin
      Result := True;
    end;
end;

#if include_plugin_avisynth
var
  avisynthplugindir: String;

function GetAviSynthPluginDir(dummy: String): String;
begin
  if Length(avisynthplugindir) = 0 then begin
    if NOT RegQueryStringValue(HKLM, 'Software\AviSynth', 'plugindir2_5', avisynthplugindir) OR NOT DirExists(avisynthplugindir) then begin
      if NOT RegQueryStringValue(HKLM, 'Software\AviSynth', 'plugindir', avisynthplugindir) OR NOT DirExists(avisynthplugindir) then begin
        avisynthplugindir := ExpandConstant('{app}');
      end
    end
  end

  Result := avisynthplugindir;
end;
#endif

#if include_plugin_dscaler
var
  dscalerdir: String;

function GetDScalerDir(dummy: String): String;
var
  proglist: Array of String;
  i: Integer;
  temp : String;
begin
  if Length(dscalerdir) = 0 then begin
    dscalerdir := ExpandConstant('{app}');
    if RegGetSubkeyNames(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall', proglist) then begin
      for i:=0 to (GetArrayLength(proglist) - 1) do begin
        if Pos('dscaler', Lowercase(proglist[i])) > 0 then begin
          if RegQueryStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\' + proglist[i], 'Inno Setup: App Path', temp) AND DirExists(temp) then begin
            dscalerdir := temp;
            Break;
          end
        end
      end
    end
  end
  Result := dscalerdir;
end;
#endif

#if include_plugin_virtualdub
var
  VdubDirPage: TInputDirWizardPage;

function GetVdubPluginDir(dummy: String): String;
begin
  Result := VdubDirPage.Values[0];
end;
#endif

function GetDefaultInstallDir(dummy: String): String;
begin
  if NOT RegQueryStringValue(HKLM, 'Software\GNU\ffdshow', 'pth', Result) OR (Length(Result) = 0) OR NOT DirExists(Result) then begin
    Result := ExpandConstant('{pf}\ffdshow');
  end
end;

function IsUpdate(): Boolean;
begin
  Result := is_update;
end;

#if !is64bit
procedure RemoveBuildUsingNSIS();
var
  regval: String;
  resultCode: Integer;
begin
  if RegQueryStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow', 'UninstallString', regval) then begin
    MsgBox(CustomMessage('msg_uninstallFirst'), mbInformation, mb_ok);
    if NOT Exec('>', regval, '', SW_SHOW, ewWaitUntilTerminated, resultCode) then begin
      MsgBox(SysErrorMessage(resultCode), mbError, MB_OK);
    end
  end
end;
#endif

function InitializeSetup(): Boolean;
begin
  Result := True;

  #if cpu_detection
  if Result AND NOT HasSupportedCPU() then begin
    Result := False;
    MsgBox(CustomMessage('unsupported_cpu'), mbError, MB_OK);
  end
    #if sse2_required
  if Result AND NOT Is_SSE2_Supported() then begin
    Result := False;
    MsgBox(CustomMessage('simd_msg_sse2'), mbError, MB_OK);
  end
    #elif sse_required
  if Result AND NOT Is_SSE_Supported() then begin
    Result := False;
    MsgBox(CustomMessage('simd_msg_sse'), mbError, MB_OK);
  end
    #endif
  #endif

  #if !is64bit
  if Result then begin
    RemoveBuildUsingNSIS;
  end
  #endif

  if Result then begin
    #if is64bit
    is_update := RegKeyExists(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow64_is1');
    #else
    is_update := RegKeyExists(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow_is1');
    #endif
  end
end;

#if cpu_detection
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    if IsTaskSelected('resetsettings') OR NOT RegValueExists(HKCU, 'Software\GNU\ffdshow\default', 'threadsnum') then begin
      RegWriteDwordValue(HKCU, 'Software\GNU\ffdshow\default', 'threadsnum', GetNumberOfCores);
    end
  end
end;
#endif

function InitializeUninstall(): Boolean;
begin
  Result := True;

  #if !is64bit
  // Also uninstall ancient versions when uninstalling.
  RemoveBuildUsingNSIS;
  #endif
end;

// #define DSSPEAKER_DIRECTOUT         0x00000000
// #define DSSPEAKER_HEADPHONE         0x00000001
// #define DSSPEAKER_MONO              0x00000002
// #define DSSPEAKER_QUAD              0x00000003
// #define DSSPEAKER_STEREO            0x00000004
// #define DSSPEAKER_SURROUND          0x00000005
// #define DSSPEAKER_5POINT1           0x00000006
// #define DSSPEAKER_7POINT1           0x00000007

function ffSpkCfg(): Integer;
external 'getSpeakerConfig@files:ffSpkCfg.dll stdcall delayload';

function getSpeakerConfig(): Integer;
begin
  try
    Result := ffSpkCfg();
  except
    Result := 4; // DSSPEAKER_STEREO
  end;
end;

function GetMixerOut(dummy: String): String;
begin
  if      SpeakerPage.Values[0] = True then
    Result := '0'
  else if SpeakerPage.Values[1] = True then
    Result := '17'
  else if SpeakerPage.Values[2] = True then
    Result := '1'
  else if SpeakerPage.Values[3] = True then
    Result := '2'
  else if SpeakerPage.Values[4] = True then
    Result := '12'
  else if SpeakerPage.Values[5] = True then
    Result := '6'
  else if SpeakerPage.Values[6] = True then
    Result := '13'
  else if SpeakerPage.Values[7] = True then
    Result := '24'
  else if SpeakerPage.Values[8] = True then
    Result := IntToStr(reg_mixerOut);
  RegWriteDWordValue(HKLM, 'Software\GNU\ffdshow_audio', 'isSpkCfg', 1);
end;

function GetIsMixer(dummy: String): String;
begin
  Result := '1';
  if (is8DisableMixer = True) and (SpeakerPage.Values[8] = True) then
    Result := '0';
end;

function ffRegReadDWordHKCU(regKeyName: String; regValName: String; defaultValue: Cardinal): Cardinal;
begin
  if NOT RegQueryDwordValue(HKCU, regKeyName, regValName, Result) then
    Result := defaultValue;
end;

procedure InitializeWizard;
var
  systemSpeakerConfig: Integer;
  reg_isSpkCfg: Cardinal;
  isMajorType: Boolean;
begin
  { Create custom pages }

  // Speaker setup

  is8DisableMixer := False;
  SpeakerPage := CreateInputOptionPage(wpSelectTasks,
    CustomMessage('spk_Label1'),
    CustomMessage('spk_Label2'),
    CustomMessage('spk_Label3'),
    True, False);
  SpeakerPage.Add('1.0 (' + CustomMessage('spk_mono')      + ')');                                          // 0
  SpeakerPage.Add('2.0 (' + CustomMessage('spk_headPhone') + ')');                                          // 17
  SpeakerPage.Add('2.0 (' + CustomMessage('spk_stereo')    + ')');                                          // 1
  SpeakerPage.Add('3.0 (' + CustomMessage('spk_front_3ch') + ')');                                          // 2
  SpeakerPage.Add('4.1 (' + CustomMessage('spk_quadro')    + ' + ' + CustomMessage('spk_subwoofer') + ')'); // 12
  SpeakerPage.Add('5.0 (' + CustomMessage('spk_5ch')       + ')');                                          // 6
  SpeakerPage.Add('5.1 (' + CustomMessage('spk_5ch')       + ' + ' + CustomMessage('spk_subwoofer') + ')'); // 13
  SpeakerPage.Add('7.1 (' + CustomMessage('spk_7ch')       + ' + ' + CustomMessage('spk_subwoofer') + ')'); // 24
  if  RegQueryDWordValue(HKCU, 'Software\GNU\ffdshow_audio\default', 'mixerOut', reg_mixerOut)
  and RegQueryDWordValue(HKCU, 'Software\GNU\ffdshow_audio\default', 'ismixer' , reg_ismixer)
  and RegQueryDWordValue(HKLM, 'Software\GNU\ffdshow_audio'        , 'isSpkCfg', reg_isSpkCfg) then
  begin
    if reg_ismixer = 1 then begin
      isMajorType := True;
      if      reg_mixerOut = 0 then
        SpeakerPage.Values[0] := True
      else if reg_mixerOut = 17 then
        SpeakerPage.Values[1] := True
      else if reg_mixerOut = 1 then
        SpeakerPage.Values[2] := True
      else if reg_mixerOut = 2 then
        SpeakerPage.Values[3] := True
      else if reg_mixerOut = 12 then
        SpeakerPage.Values[4] := True
      else if reg_mixerOut = 6 then
        SpeakerPage.Values[5] := True
      else if reg_mixerOut = 13 then
        SpeakerPage.Values[6] := True
      else if reg_mixerOut = 24 then
        SpeakerPage.Values[7] := True
      else begin
        if reg_mixerOut = 3 then
          SpeakerPage.Add('2+1 ('   + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_rear_1ch')  + ')')
        else if reg_mixerOut = 4 then
          SpeakerPage.Add('3+1 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_rear_1ch')  + ')')
        else if reg_mixerOut = 5 then
          SpeakerPage.Add('4.0 ('   + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_rear_2ch')  + ')')
        else if reg_mixerOut = 7 then
          SpeakerPage.Add('1.1 ('   + CustomMessage('spk_mono')      + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 8 then
          SpeakerPage.Add('2.1 ('   + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 9 then
          SpeakerPage.Add('3.1 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 10 then
          SpeakerPage.Add('2+1.1 (' + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_rear_1ch')  + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 11 then
          SpeakerPage.Add('3+1.1 (' + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_rear_1ch')  + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 14 then
          SpeakerPage.Add(            CustomMessage('spk_dolby1'))
        else if reg_mixerOut = 19 then
          SpeakerPage.Add(            CustomMessage('spk_dolby1')    + ' + ' + CustomMessage('spk_subwoofer'))
        else if reg_mixerOut = 15 then
          SpeakerPage.Add(            CustomMessage('spk_dolby2'))
        else if reg_mixerOut = 20 then
          SpeakerPage.Add(            CustomMessage('spk_dolby2')    + ' + ' + CustomMessage('spk_subwoofer'))
        else if reg_mixerOut = 16 then
          SpeakerPage.Add(            CustomMessage('spk_sameAsInput'))
        else if reg_mixerOut = 18 then
          SpeakerPage.Add(            CustomMessage('spk_hrtf'))
        else if reg_mixerOut = 21 then
          SpeakerPage.Add('6.0 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_side_2ch')  + ' + ' + CustomMessage('spk_rear_1ch')  + ')')
        else if reg_mixerOut = 22 then
          SpeakerPage.Add('6.1 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_side_2ch')  + ' + ' + CustomMessage('spk_rear_1ch')  + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 23 then
          SpeakerPage.Add('7.0 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_side_2ch')  + ' + ' + CustomMessage('spk_rear_2ch')  + ')')
        else if reg_mixerOut > 24 then
          SpeakerPage.Add(           CustomMessage('spk_unknownSpk'));
        SpeakerPage.Values[8] := True;
        isMajorType := False;
      end
      if isMajorType then begin
       SpeakerPage.Add(CustomMessage('spk_disableMixer'));
       is8DisableMixer := True;
      end
    end else begin
      SpeakerPage.Add(CustomMessage('spk_disableMixer'));
      is8DisableMixer := True;
      SpeakerPage.Values[8] := True;
    end
  end else begin
    reg_ismixer := 1;
    reg_mixerOut := 1;
    is8DisableMixer := True;
    systemSpeakerConfig := getSpeakerConfig(); // read the setting of control panel(requires directX 8)
    SpeakerPage.Values[2] := True;             // default 2.0 (Stereo)

    if      systemSpeakerConfig = 2 then // DSSPEAKER_MONO
      SpeakerPage.Values[0] := True
    else if systemSpeakerConfig = 1 then // DSSPEAKER_HEADPHONE
      SpeakerPage.Values[1] := True
    else if systemSpeakerConfig = 3 then // DSSPEAKER_QUAD
      begin
       SpeakerPage.Add('4.0 (' + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_rear_2ch') + ')');
       SpeakerPage.Values[8] := True;
       isMajorType := False;
       is8DisableMixer := False;
       reg_mixerOut := 5;
      end
    else if systemSpeakerConfig = 5 then // DSSPEAKER_SURROUND
      SpeakerPage.Values[5] := True
    else if systemSpeakerConfig = 6 then // DSSPEAKER_5POINT1
      SpeakerPage.Values[6] := True
    else if systemSpeakerConfig = 7 then // DSSPEAKER_7POINT1
      SpeakerPage.Values[6] := True
    else if systemSpeakerConfig = 8 then // 7.1ch hometheater
      SpeakerPage.Values[7] := True;
    if is8DisableMixer then
     SpeakerPage.Add(CustomMessage('spk_disableMixer'));
  end

#if include_plugin_virtualdub
  // VirtualDub plugin install directory setting
  VdubDirPage := CreateInputDirPage(SpeakerPage.ID,
    FmtMessage(CustomMessage('plg_Label1'), ['VirtualDub']),
    FmtMessage(CustomMessage('plg_Label2'), ['VirtualDub']),
    FmtMessage(CustomMessage('plg_Label3'), ['VirtualDub']),
    False, '');
  VdubDirPage.Add('');
#endif
end;

#if include_plugin_virtualdub
function ShouldSkipPage(PageID: Integer): Boolean;
var
  regval: String;
begin
  Result := False;
  if PageID = VdubDirPage.ID then begin
    if IsComponentSelected('ffdshow\plugins\virtualdub') then begin
      if VdubDirPage.Values[0] = '' then begin
        if RegQueryStringValue(HKLM, 'Software\GNU\ffdshow', 'pthVirtualDub', regval)
        and not (regval = ExpandConstant('{app}')) and not (regval = '') then
          VdubDirPage.Values[0] := regval
        else if FileOrDirExists(ExpandConstant('{pf}\VirtualDub\plugins')) then
            VdubDirPage.Values[0] := ExpandConstant('{pf}\VirtualDub\plugins')
        else if FileOrDirExists(ExpandConstant('{sd}\VirtualDub\plugins')) then
            VdubDirPage.Values[0] := ExpandConstant('{sd}\VirtualDub\plugins')
        else
          VdubDirPage.Values[0] := ExpandConstant('{app}');
      end
    end
    else begin
      Result := True;
    end
  end
end;
#endif
