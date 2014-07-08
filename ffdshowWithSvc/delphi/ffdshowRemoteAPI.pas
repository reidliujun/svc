unit ffdshowRemoteAPI;

interface

const
  FFDSHOW_REMOTE_MESSAGE='ffdshow_remote_message';
  FFDSHOW_REMOTE_CLASS='ffdshow_remote_class';

  //lParam - parameter id to be used by WPRM_PUTPARAM and WPRM_GETPARAM
  WPRM_SETPARAM_ID=0;

  //lParam - new value of parameter
  //returns TRUE or FALSE
  WPRM_PUTPARAM   =1;

  //lParam - unused
  //return the value of parameter
  WPRM_GETPARAM   =2;

  //lParam - parameter id
  WPRM_GETPARAM2  =3;

  WPRM_STOP       =4;
  WPRM_RUN        =5;

  //returns duration is seconds,
  // -1 - if not available
  //  0 - stopped
  //  1 - paused
  //  2 - running
  WPRM_GETSTATE   =6;

  //returns movie duration in seconds
  WPRM_GETDURATION=7;
  //returns current position in seconds
  WPRM_GETCURTIME=8;

  //WM_COPYDATA
  //COPYDATASTRUCT.dwData=
  COPY_PUTPARAMSTR    = 9; // lpData points to new param value
  COPY_SETACTIVEPRESET=10; // lpData points to new preset name
implementation

end.




