unit ffdshow_remote_main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ComCtrls, StdCtrls, ExtCtrls;

type
  TmainForm = class(TForm)
    btFind: TButton;
    lvFFDSHOW: TListView;
    pnl: TPanel;
    chbLevels: TCheckBox;
    btRun: TButton;
    btStop: TButton;
    lblCurtime: TLabel;
    lblDuration: TLabel;
    tmrTime: TTimer;
    edPreset: TEdit;
    btPreset: TButton;
    procedure btFindClick(Sender: TObject);
    procedure lvFFDSHOWSelectItem(Sender: TObject; Item: TListItem;
      Selected: Boolean);
    procedure FormCreate(Sender: TObject);
    procedure chbLevelsClick(Sender: TObject);
    procedure btRunClick(Sender: TObject);
    procedure btStopClick(Sender: TObject);
    procedure tmrTimeTimer(Sender: TObject);
    procedure btPresetClick(Sender: TObject);
  private
    { Private declarations }
    remotemsg:cardinal;
    function  getParam(hwnd:HWND;id: Integer): Integer;
    procedure putParam(hwnd:HWND;id,val: Integer);
    procedure putParamStr(hwnd:HWND;id:Integer;val:string);
    procedure setActivePreset(hwnd: HWND; preset: string);
  public
    { Public declarations }
  end;

var
  mainForm: TmainForm;

implementation

uses ffdshowRemoteAPI,IffDecoder_com;

{$R *.DFM}

function hwnd2app(h:HWND):string;
var thread:Integer;
    process:DWORD;
begin
// thread:=GetWindowThreadProcessId(h,@process);
 Result:='unknown';
end;

function enumFFDSHOWwindows(hwnd:HWND;lParam:lParam):BOOL;stdcall;
var classname:array [0..256] of Char;
    li:TListItem;
begin
 GetClassName(hwnd,classname,256);
 if classname=FFDSHOW_REMOTE_CLASS then begin
   li:=TListView(lParam).Items.Add;
   li.Caption:=IntToHex(hwnd,8);
   li.SubItems.Add(hwnd2app(hwnd));
   li.Data:=Pointer(hwnd);
  end;
 Result:=True;
end;

procedure TmainForm.btFindClick(Sender: TObject);
begin
 lvFFDSHOW.Items.clear;
 EnumWindows(@enumFFDSHOWwindows,Integer(lvFFDSHOW));
end;

function TmainForm.getParam(hwnd:HWND;id:Integer):Integer;
begin
 Result:=SendMessage(hwnd,remotemsg,WPRM_GETPARAM2,id);
end;

procedure TmainForm.putParam(hwnd:HWND;id, val: Integer);
begin
 SendMessage(hwnd,remotemsg,WPRM_SETPARAM_ID,id);
 SendMessage(hwnd,remotemsg,WPRM_PUTPARAM,val);
end;

procedure TmainForm.putParamStr(hwnd: HWND; id: Integer; val: string);
var cds:TCopyDataStruct;
begin
 SendMessage(hwnd,remotemsg,WPRM_SETPARAM_ID,id);
 cds.dwData:=COPY_PUTPARAMSTR;
 cds.lpData:=PChar(val);
 cds.cbData:=Length(val)+1;
 SendMessage(hwnd,WM_COPYDATA,Handle,LPARAM(@cds));
end;

procedure TmainForm.setActivePreset(hwnd:HWND;preset:string);
var cds:TCopyDataStruct;
begin
 cds.dwData:=COPY_SETACTIVEPRESET;
 cds.lpData:=PChar(preset);
 cds.cbData:=Length(preset)+1;
 SendMessage(hwnd,WM_COPYDATA,Handle,LPARAM(@cds));
end;

procedure TmainForm.lvFFDSHOWSelectItem(Sender: TObject; Item: TListItem; Selected: Boolean);
var secs:Integer;
begin
 if Assigned(item) and selected then begin
   pnl.visible:=true;
   chbLevels.Checked:=getParam(HWND(Item.Data),IDFF_isLevels)<>0;
   secs:=SendMessage(HWND(Item.Data),remotemsg,WPRM_GETDURATION,0);
   lblDuration.Caption:=format('/%2u:%2u:%2u',[secs div 3600,(secs div 60) mod 60,secs mod 60]);
   tmrTime.Enabled:=True;
  end
 else begin
   pnl.visible:=False;
   tmrTime.Enabled:=False;
  end;
end;

procedure TmainForm.FormCreate(Sender: TObject);
begin
 remotemsg:=RegisterWindowMessage(FFDSHOW_REMOTE_MESSAGE);
end;

procedure TmainForm.chbLevelsClick(Sender: TObject);
begin
 if chbLevels.Checked then
  putParam(HWND(lvFFDSHOW.Selected.Data),IDFF_isLevels,1)
 else
  putParam(HWND(lvFFDSHOW.Selected.Data),IDFF_isLevels,0);
end;

procedure TmainForm.btRunClick(Sender: TObject);
begin
 SendMessage(HWND(lvFFDSHOW.Selected.Data),remotemsg,WPRM_RUN,0);
end;

procedure TmainForm.btStopClick(Sender: TObject);
begin
 SendMessage(HWND(lvFFDSHOW.Selected.Data),remotemsg,WPRM_STOP,0);
end;

procedure TmainForm.tmrTimeTimer(Sender: TObject);
var sec:Integer;
begin
 if not Assigned(lvFFDSHOW.selected) then Exit;
 sec:=SendMessage(HWND(lvFFDSHOW.Selected.Data),remotemsg,WPRM_GETCURTIME,0);
 lblCurtime.Caption:=format('%2u:%2u:%2u',[sec div 3600,(sec div 60) mod 60,sec mod 60]);
end;

procedure TmainForm.btPresetClick(Sender: TObject);
begin
 setActivePreset(HWND(lvFFDSHOW.Selected.Data),edPreset.text);
end;

end.

