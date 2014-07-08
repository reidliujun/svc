unit IffDecoder_com;

interface

uses windows;

const
  IID_IffDecoder:TGUID='{00F99063-70D5-4bcc-9D88-3801F3E3881B}';
  CLSID_FFDSHOW :TGUID='{04FE9017-F873-410e-871E-AB91661A4EF7}';

  IDFF_autoPreset           =1; //automatic preset loading enabled
  IDFF_trayIcon             =3; //is tray icon visible
  IDFF_trayHwnd             =34;
  IDFF_cfgDlgHwnd           =4; //handle of configuration dialog
  IDFF_autoPresetFileFirst  =5; //try to load preset from file
  IDFF_autoLoadedFromFile   =6;
  IDFF_mode                 =7;
   IDFF_MODE_CONFIG=0;
   IDFF_MODE_PLAYER=1;
   IDFF_MODE_FFVFW =2;
  IDFF_currentFrame        =14;
  IDFF_decodingFps         =15;
  IDFF_buildHistogram      =16;
  IDFF_AVIname             =17;
  IDFF_AVIfourcc           =18;
  IDFF_AVIaspectX          =27;
  IDFF_AVIaspectY          =28;
  IDFF_subFlnmChanged      =20;
  IDFF_workaroundBugs      =25;
  IDFF_again               =26;
  IDFF_subCurrentFlnm      =29;
  IDFF_movieFillsQuant     =30;
  IDFF_frameType           =31;
  IDFF_movieSource         =33;
   IDFF_MOVIE_NONE     =0;
   IDFF_MOVIE_LAVC     =1;
   IDFF_MOVIE_XVID     =2;
   IDFF_MOVIE_THEO     =3;
   IDFF_MOVIE_RAW      =4;
   IDFF_MOVIE_LIBMPEG2 =5;
  IDFF_installPath          =35;
  IDFF_cpuFlags             =36;

  IDFF_dlgRestorePos       = 9;
  IDFF_dlgPosX             =10;
  IDFF_dlgPosY             =11;
  IDFF_lvWidth0            =12;
  IDFF_showHints           =13;
  IDFF_lastPage            = 2; //last active page in configuration dialog
  IDFF_defaultPreset       =19;
  IDFF_lvKeysWidth0        =21;
  IDFF_lvKeysWidth1        =22;
  IDFF_lang                =32;
  IDFF_alwaysOnTop         =35;
  IDFF_applying            =37;

  IDFF_presetAutoloadFlnm    =1901;//should be preset autoloaded depending on filename
  IDFF_presetAutoloadSize    =1902;//should be preset autoloaded depending on movie dimensions
  IDFF_presetAutoloadSizeXmin=1903;
  IDFF_presetAutoloadSizeXmax=1904;
  IDFF_presetAutoloadSizeCond=1905;//0 - and, 1 - or
  IDFF_presetAutoloadSizeYmin=1906;
  IDFF_presetAutoloadSizeYmax=1907;

  IDFF_filterPostproc         =100;
  IDFF_isPostproc             =106;
  IDFF_orderPostproc          =109;
  IDFF_fullPostproc           =111;
  IDFF_postprocMethod         =114; //0 - mplayer, 1 - nic
  IDFF_postprocMethodNicFirst =117;
  IDFF_ppqual                 =101; //postprocessing quality set by user (active when not autoq)
  IDFF_autoq                  =102; //is automatic postprocessing control enabled?
  IDFF_ppIsCustom             =103; //custom postprocessing settings are enabled
  IDFF_ppcustom               =104; //custom postprocessing settings
  IDFF_currentq               =105;
  IDFF_deblockStrength        =110;
  IDFF_levelFixLum            =107;
  IDFF_levelFixChrom          =108;
  IDFF_fullYrange             =112;
  IDFF_tempNoiseFilter        =113;
  IDFF_postprocNicXthresh     =115;
  IDFF_postprocNicYthresh     =116;

  IDFF_isDeinterlace        =1401;
  IDFF_fullDeinterlace      =1402;
  IDFF_swapFields           =1409;
  IDFF_deinterlaceMethod    =1403;
  IDFF_deinterlaceMethodDef =1404;
  IDFF_fieldnum             =1405;
  IDFF_fieldcount           =1406;
  IDFF_tomocompSE           =1407;
  IDFF_tomocompSEdef        =1408;
  IDFF_tomocompVF           =1414;
  IDFF_dscalerDIflnm        =1412;
  IDFF_dscalerDIcfg         =1413;

  IDFF_isDecimate           =1410;
  IDFF_decimateRatio        =1411;

  IDFF_filterPictProp     =200;
  IDFF_isPictProp         =205;
  IDFF_orderPictProp      =207;
  IDFF_fullPictProp       =213;
  IDFF_lumGain            =201; //luminance gain
  IDFF_lumOffset          =202; //luminance offset
  IDFF_gammaCorrection    =206; //gamma correction (*100)
  IDFF_hue                =203; //hue
  IDFF_saturation         =204; //saturation
  IDFF_lumGainDef         =208;
  IDFF_lumOffsetDef       =209;
  IDFF_gammaCorrectionDef =210;
  IDFF_hueDef             =211;
  IDFF_saturationDef      =212;

  IDFF_filterLevels      =1600;
  IDFF_isLevels          =1601;
  IDFF_orderLevels       =1602;
  IDFF_fullLevels        =1603;
  IDFF_levelsInMin       =1604;
  IDFF_levelsGamma       =1605;
  IDFF_levelsInMax       =1606;
  IDFF_levelsOutMin      =1607;
  IDFF_levelsOutMax      =1608;
  IDFF_levelsOnlyLuma    =1609;
  IDFF_levelsFullY       =1610;

  IDFF_flip              = 301;//should output video be flipped?
  IDFF_idct              = 601;//IDCT function user (0 - libavcodec simple 16383, 1 - libavcodec normal, 2 - reference, 3 - libavcodec simple 16384)
  IDFF_videoDelay        =1801;//video delay in ms
  IDFF_hwOverlayAspect   =1351;
  IDFF_grayscale         = 602; //only grayscale decoding - faster

  IDFF_filterSharpen        = 400;
  IDFF_isSharpen            = 401;//is xshapen filter active?
  IDFF_orderSharpen         = 407;
  IDFF_fullSharpen          = 408;
  IDFF_sharpenMethod        = 406;//0 - xsharpen, 1 - unsharp, 2 - msharpen
  IDFF_xsharp_strength      = 402;//xsharpen filter strength
  IDFF_xsharp_threshold     = 403;//xsharpen filter threshold
  IDFF_xsharp_strengthDef   = 409;
  IDFF_xsharp_thresholdDef  = 410;
  IDFF_unsharp_strength     = 404;//unsharp filter strength
  IDFF_unsharp_threshold    = 405;//unsharp filter threshold
  IDFF_unsharp_strengthDef  = 411;
  IDFF_unsharp_thresholdDef = 412;
  IDFF_msharp_strength      = 413;
  IDFF_msharp_threshold     = 414;
  IDFF_msharpHQ             = 415;
  IDFF_msharpMask           = 416;
  IDFF_msharp_strengthDef   = 417;
  IDFF_msharp_thresholdDef  = 418;
  IDFF_warpsharpDepth       = 419;
  IDFF_warpsharpThreshold   = 420;
  IDFF_warpsharpDepthDef    = 421;
  IDFF_warpsharpThresholdDef= 422;

  IDFF_filterDCT            =450;
  IDFF_isDCT                =451;
  IDFF_orderDCT             =452;
  IDFF_fullDCT              =453;
  IDFF_dct0                 =454;
  IDFF_dct1                 =455;
  IDFF_dct2                 =456;
  IDFF_dct3                 =457;
  IDFF_dct4                 =458;
  IDFF_dct5                 =459;
  IDFF_dct6                 =460;
  IDFF_dct7                 =461;

  IDFF_filterNoise            =500;
  IDFF_isNoise                =501;//is noising filter active?
  IDFF_orderNoise             =506;
  IDFF_fullNoise              =507;
  IDFF_noiseMethod            =505;//0 - my noise, 1 - avih noise, 2 - mplayer
  IDFF_uniformNoise           =502;//is uniform noise active (applies only to luma noise now)?
  IDFF_noisePattern           =510;
  IDFF_noiseAveraged          =511;
  IDFF_noiseStrength          =503;//luma noise strength
  IDFF_noiseStrengthChroma    =504;//chroma noise strength
  IDFF_noiseStrengthDef       =508;
  IDFF_noiseStrengthChromaDef =509;

  IDFF_filterResize        =700;
  IDFF_isResize            =701;//is resizing active (or will be resizing active)
  IDFF_orderResize         =722;
  IDFF_fullResize          =723;
  IDFF_resizeMode          =728;//0 - exact size, 1 - aspect ratio , 2 - 16
  IDFF_resizeDx            =702;//new width
  IDFF_resizeDy            =703;//new height
  IDFF_resizeA1            =729;
  IDFF_resizeA2            =730;
  IDFF_resizeIf            =733;//0 - always, 1 - size, 2 - number of pixels
  IDFF_resizeIfXcond       =734;//-1 - less, 1 - more
  IDFF_resizeIfXval        =735;//width to be compared to
  IDFF_resizeIfYcond       =736;//-1 - less, 1 - more
  IDFF_resizeIfYval        =737;//height to be compared to
  IDFF_resizeIfXYcond      =738;//0 - and, 1 - or
  IDFF_resizeIfPixCond     =739;//-1 - less, 1 - more
  IDFF_resizeIfPixVal      =740;
  IDFF_bordersX            =741;
  IDFF_bordersY            =742;
  IDFF_bordersLocked       =743;

  IDFF_isAspect            =704;//0 - no aspect ratio correctio, 1 - keep original aspect, 2 - aspect ratio is set in IDFF_aspectRatio
  IDFF_aspectRatio         =707;//aspect ratio (<<16)

  IDFF_resizeMethod        =706;
  IDFF_resizeMethodDef     =731;
  IDFF_resizeBicubicParam  =724;
  IDFF_resizeXparam        =725;
  IDFF_resizeGaussParam    =726;
  IDFF_resizeLanczosParam  =727;
  IDFF_resizeGblurLum      =708;// *100
  IDFF_resizeGblurChrom    =709;// *100
  IDFF_resizeSharpenLum    =710;// *100
  IDFF_resizeSharpenChrom  =711;// *100

  IDFF_isCropNzoom         =712;
  IDFF_cropNzoomMode       =713;//0 - zoom, 1 - crop, 2 - pan&scan
  IDFF_magnificationX      =714;
  IDFF_cropLeft            =715;
  IDFF_cropRight           =716;
  IDFF_cropTop             =717;
  IDFF_cropBottom          =718;
  IDFF_autocrop            =719;
  IDFF_magnificationY      =720;
  IDFF_magnificationLocked =721;
  IDFF_panscanZoom         =744;//0 - orig size, 100 max zoom
  IDFF_panscanX            =745;//-100..100 - center x position
  IDFF_panscanY            =746;//-100..100 - center y position

  IDFF_filterSubtitles     =800;
  IDFF_isSubtitles         =801;
  IDFF_orderSubtitles      =815;
  IDFF_fullSubtitles       =817;
  IDFF_subFilename         =821;
  IDFF_subPosX             =810;
  IDFF_subPosY             =811;
  IDFF_subDelay            =812;
  IDFF_subDelayDef         =818;
  IDFF_subSpeed            =813;
  IDFF_subSpeedDef         =819;
  IDFF_subAutoFlnm         =814;
  IDFF_subSearchDir        =822;

  IDFF_fontName            =820;
  IDFF_fontCharset         =802;
  IDFF_fontAutosize        =823;
  IDFF_fontSizeP           =803;
  //IDFF_fontSize           =803;
  IDFF_fontSizeA           =824;
  IDFF_fontWeight          =804;
  IDFF_fontShadowStrength  =805; //shadow strength (0..100) 100 - subtitles aren't transparent
  IDFF_fontShadowRadius    =806; //shadow radius
  IDFF_fontSpacing         =808;
  IDFF_fontColor           =809;

  IDFF_filterBlur              =900;
  IDFF_isBlur                  =901;
  IDFF_orderBlur               =903;
  IDFF_fullBlur                =905;
  IDFF_blurIsSoften            =921;
  IDFF_blurStrength            =902;
  IDFF_blurStrengthDef         =906;
  IDFF_blurIsTempSmooth        =922;
  IDFF_tempSmooth              =904;
  IDFF_tempSmoothColor         =912;
  IDFF_tempSmoothDef           =907;
  IDFF_blurIsSmoothLuma        =923;
  IDFF_smoothStrengthLuma      =908;
  IDFF_smoothStrengthLumaDef   =910;
  IDFF_blurIsSmoothChroma      =926;
  IDFF_smoothStrengthChroma    =909;
  IDFF_smoothStrengthChromaDef =911;
  IDFF_blurIsGradual           =924;
  IDFF_gradualStrength         =913;
  IDFF_gradualStrengthDef      =914;
  IDFF_blurIsMplayerTNR        =925;
  IDFF_mplayerTNR1             =915;
  IDFF_mplayerTNR2             =916;
  IDFF_mplayerTNR3             =917;
  IDFF_mplayerTNR1def          =918;
  IDFF_mplayerTNR2def          =919;
  IDFF_mplayerTNR3def          =920;

  IDFF_filterOffset      =1100;
  IDFF_isOffset          =1101;
  IDFF_orderOffset       =1102;
  IDFF_fullOffset        =1109;
  IDFF_offsetY_X         =1103;
  IDFF_offsetY_Y         =1104;
  IDFF_offsetU_X         =1105;
  IDFF_offsetU_Y         =1106;
  IDFF_offsetV_X         =1107;
  IDFF_offsetV_Y         =1108;

  IDFF_filterShowMV      =1200;
  IDFF_isShowMV          =1201;
  IDFF_orderShowMV       =1202;

  IDFF_filterAvisynth       =1250;
  IDFF_isAvisynth           =1251;
  IDFF_orderAvisynth        =1252;
  IDFF_fullAvisynth         =1253;
  IDFF_avisynthScript       =1254;
  IDFF_avisynthInCsp        =1255;

  IDFF_isOSD                =1501;
  IDFF_OSDfontName          =1509;
  IDFF_OSDfontCharset       =1502;
  IDFF_OSDfontSize          =1503;
  IDFF_OSDfontWeight        =1504;
  IDFF_OSDfontShadowStrength=1505;
  IDFF_OSDfontShadowRadius  =1506;
  IDFF_OSDfontSpacing       =1507;
  IDFF_OSDfontColor         =1508;
  IDFF_OSDformat            =1510;
   IDFF_OSDtype_inputSize =1520;
   IDFF_OSDtype_outputSize=1521;
   IDFF_OSDtype_cpuUsage  =1522;
   IDFF_OSDtype_systemTime=1523;
   IDFF_OSDtype_meanQuant =1524;
  IDFF_OSDuser              =1511; //don't use these, use drawOSD() instead
  IDFF_OSDuserPx            =1512;
  IDFF_OSDuserPy            =1513;

  IDFF_filterGrab   =2000;
  IDFF_isGrab       =2001;
  IDFF_orderGrab    =2002;
  IDFF_fullGrab     =2003;
  IDFF_grabPath     =2004;
  IDFF_grabPrefix   =2005;
  IDFF_grabDigits   =2006;
  IDFF_grabFormat   =2007;//0 - jpeg (needs ffvfw), 1 - bmp, 2 - png
  IDFF_grabMode     =2008;//0 - all frames, 1 - one frame (grabFrameNum), 2 - range (grabFrameNum1-grabFrameNum2)
  IDFF_grabFrameNum =2009;
  IDFF_grabFrameNum1=2010;
  IDFF_grabFrameNum2=2011;
  IDFF_grabQual     =2012;//0..100

  IDFF_isOverlayControl   =2101;
  IDFF_overlayBrightness  =2102;
  IDFF_overlayContrast    =2103;
  IDFF_overlayHue         =2104;
  IDFF_overlaySaturation  =2105;
  IDFF_overlaySharpness   =2106;
  IDFF_overlayGamma       =2107;
  IDFF_overlayColorEnable =2108;

  IDFF_isKeys            =1701;
  IDFF_showKeys          =1706;
  IDFF_keysAlways        =1702;

  IDFF_xvid              =1001;//are AVIs with this FOURCC played by ffdshow?
  IDFF_div3              =1002;
  IDFF_divx              =1003;
  IDFF_dx50              =1004;
  IDFF_mp43              =1005;
  IDFF_mp42              =1006;
  IDFF_mp41              =1007;
  IDFF_h263              =1008;
  IDFF_wmv1              =1011;
  IDFF_wmv2              =1017;
  IDFF_mjpg              =1014;
  IDFF_dvsd              =1015;
  IDFF_hfyu              =1016;
  IDFF_mpg1              =1012;
  IDFF_mpg2              =1013;
  IDFF_rawv              =1009;

  IDFF_hwOverlay         =1350;
  IDFF_outI420           =1309;
  IDFF_outYV12           =1301;
  IDFF_outYUY2           =1302;
  IDFF_outYVYU           =1303;
  IDFF_outUYVY           =1304;
  IDFF_outRGB32          =1305;
  IDFF_outRGB24          =1306;
  IDFF_outRGB555         =1307;
  IDFF_outRGB565         =1308;

  IDFF_filterDScaler        =2200;
  IDFF_isDScaler            =2201;
  IDFF_orderDScaler         =2202;
  IDFF_fullDScaler          =2203;
  IDFF_dscalerFltflnm       =2204;
  IDFF_dscalerCfg           =2205;

  IDFF_filterPerspective        =2300;
  IDFF_isPerspective            =2301;
  IDFF_orderPerspective         =2302;
  IDFF_fullPerspective          =2303;
  IDFF_perspectiveIsSrc         =2313;
  IDFF_perspectiveX1            =2304;
  IDFF_perspectiveY1            =2305;
  IDFF_perspectiveX2            =2306;
  IDFF_perspectiveY2            =2307;
  IDFF_perspectiveX3            =2308;
  IDFF_perspectiveY3            =2309;
  IDFF_perspectiveX4            =2310;
  IDFF_perspectiveY4            =2311;
  IDFF_perspectiveInterpolation =2312;

  IDFF_max                =2313;

type
  PPointer=^Pointer;
  PPchar=^PChar;
  IffDecoder = interface(IUnknown)
    ['{00F99063-70D5-4bcc-9D88-3801F3E3881B}']
    function getParam(paramID:integer; value:pinteger):HRESULT;stdcall;
    function getParam2(paramID:integer):HRESULT;stdcall;
    function putParam(paramID:integer;value:integer):HRESULT;stdcall;
    function getNumPresets(value:pinteger):HRESULT;stdcall;
    function getPresetName(i:integer;buf:pchar;len:integer):HRESULT;stdcall;
    function getActivePresetName(buf:pchar;len:integer):HRESULT;stdcall;
    function setActivePreset(const name:pchar;create:integer):HRESULT;stdcall;
    function getAVIdimensions(x:pinteger;y:pinteger):HRESULT;stdcall;
    function getAVIfps(fps1000:pinteger):HRESULT;stdcall;
    function saveActivePreset(const name:pchar):HRESULT;stdcall;
    function saveActivePresetToFile(const flnm:pchar):HRESULT;stdcall;
    function loadActivePresetFromFile(const flnm:pchar):HRESULT;stdcall;
    function removePreset(const name:pchar):HRESULT;stdcall;
    function notifyParamsChanged:HRESULT;stdcall;
    function getAVcodecVersion(buf:pchar;len:integer):HRESULT;stdcall;
    function getPPmode(ppmode:pinteger):HRESULT;stdcall;
    function getRealCrop(left,top,right,bottom:pinteger):HRESULT;stdcall;
    function getMinOrder2:HRESULT;stdcall;
    function getMaxOrder2:HRESULT;stdcall;
    function saveGlobalSettings:HRESULT;stdcall;
    function loadGlobalSettings:HRESULT;stdcall;
    function saveDialogSettings:HRESULT;stdcall;
    function loadDialogSettings:HRESULT;stdcall;
    function getPresets(presets2:ppointer):HRESULT;stdcall;
    function setPresets(const presets2:pointer):HRESULT;stdcall;
    function savePresets:HRESULT;stdcall;
    function getPresetPtr(preset:ppointer):HRESULT;stdcall;
    function setPresetPtr(preset:pointer):HRESULT;stdcall;
    function renameActivePreset(const newName:pchar):HRESULT;stdcall;
    function setOnChangeMsg(wnd:HWND;msg:integer):HRESULT;stdcall;
    function setOnFrameMsg(wnd:HWND;msg:integer):HRESULT;stdcall;
    function isDefaultPreset(const presetName:pchar):HRESULT;stdcall;
    function showCfgDlg(owner:HWND):HRESULT;stdcall;
    function getXvidVersion(buf:pchar;len:integer):HRESULT;stdcall;
    function getMovieSource(moviePtr:ppointer):HRESULT;stdcall;
    function getOutputDimensions(x:pinteger;y:pinteger):HRESULT;stdcall;
    function getCpuUsage2:HRESULT;stdcall;
    function getOutputFourcc(buf:pchar;len:integer):HRESULT;stdcall;
    function getInputBitrate2:HRESULT;stdcall;
    function getHistogram(dst:array of byte):HRESULT;stdcall;
    function setFilterOrder(filterID:integer;newOrder:integer):HRESULT;stdcall;
    function buildHistogram(const pict:pointer;full:integer):HRESULT;stdcall;
    function cpuSupportsMMX:HRESULT;stdcall;
    function cpuSupportsMMXEXT:HRESULT;stdcall;
    function cpuSupportsSSE:HRESULT;stdcall;
    function cpuSupportsSSE2:HRESULT;stdcall;
    function cpuSupports3DNOW:HRESULT;stdcall;
    function cpuSupports3DNOWEXT:HRESULT;stdcall;
    function getAVIfps1000_2:HRESULT;stdcall;
    function getParamStr(paramID:integer;buf:pchar;buflen:integer):HRESULT;stdcall;
    function putParamStr(paramID:integer;const buf:pchar):HRESULT;stdcall;
    function invParam(paramID:integer):HRESULT;stdcall;
    function getInstance(hi:HMODULE):HRESULT;stdcall;
    function saveKeysSettings:HRESULT;stdcall;
    function loadKeysSettings:HRESULT;stdcall;
    function seek(seconds:integer):HRESULT;stdcall;
    function tell(seconds:pinteger):HRESULT;stdcall;
    function getDuration(seconds:pinteger):HRESULT;stdcall;
    function getKeyParamCount2:HRESULT;stdcall;
    function getKeyParamDescr(i:integer;descr:ppchar):HRESULT;stdcall;
    function getKeyParamKey2(i:integer):HRESULT;stdcall;
    function setKeyParamKey(i:integer;key:integer):HRESULT;stdcall;
    function getImgFilters(imgFiltersPtr:ppointer):HRESULT;stdcall;
    function getQuant(quantPtr:ppointer):HRESULT;stdcall;
    function calcNewSize(inDx,inDy:integer;outDx,outDy:pinteger):HRESULT;stdcall;
    function grabNow:HRESULT;stdcall;
    function getOverlayControlCapability(idff:integer):HRESULT;stdcall; //S_OK - can be set, S_FALSE - not supported
    function getParamName(i:Integer;buf:PChar;len:integer):HRESULT;stdcall;
    function getTranslator(trans:ppointer):HRESULT;stdcall;
    function getIffDecoderVersion2:HRESULT;stdcall;
    function drawOSD(px:Integer;py:Integer;const Text:pchar):HRESULT;stdcall; //accepts max. lines separated by \n
    function lock(lockId:integer):HRESULT;stdcall;
    function unlock(lockId:integer):HRESULT;stdcall;
    function getInstance2:HRESULT;stdcall;
    function getGraph(IgraphPtr:ppointer):HRESULT;stdcall;
    function getConfig(configPtr:ppointer):HRESULT;stdcall;
    function initDialog:HRESULT;stdcall;
    function initPresets:HRESULT;stdcall;
    function calcMeanQuant(quant:psingle):HRESULT;stdcall;
    function initKeys:HRESULT;stdcall;
  end;

implementation

end.
