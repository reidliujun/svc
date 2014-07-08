unit ffdshow_sample_main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms,
  Dialogs,IffDecoder_com, StdCtrls,ActiveX;

type
  TmainForm = class(TForm)
    Button1: TButton;
    Button2: TButton;
    lbxProps: TListBox;
    lblAutoq: TLabel;
    Label1: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure lbxPropsDblClick(Sender: TObject);
  private
    { Private declarations }
    deci:IffDecoder;
    procedure getPresets;
  public
    { Public declarations }
  end;

var
  mainForm: TmainForm;

implementation

{$R *.dfm}

procedure TmainForm.FormCreate(Sender: TObject);
begin
 CoInitialize(nil);
end;

procedure TmainForm.Button1Click(Sender: TObject);
begin
 deci:=nil;
 if CoCreateInstance(CLSID_FFDSHOW,nil,CLSCTX_INPROC_SERVER,IID_IffDecoder,deci)<>S_OK then begin
   Application.MessageBox('Unable to create IffDecoder instance','ERROR',MB_OK or MB_ICONERROR);
   exit;
  end;
 button1.Enabled:=false;button2.enabled:=true;
 getPresets;
end;

procedure TmainForm.Button2Click(Sender: TObject);
begin
 if assigned(deci) then begin
   deci:=nil;
   button1.Enabled:=true;button2.enabled:=false;
   lbxProps.Clear;
  end;
end;

procedure TmainForm.getPresets;
var numpresets:Integer;
    i:Integer;
    presetname:string;
begin
 deci.initPresets;
 deci.getNumPresets(@numpresets);
 SetLength(presetname,256);
 for i:=0 to numpresets-1 do begin
   deci.getPresetName(i,PChar(presetname),255);
   lbxProps.Items.Add(presetname);
  end;
end;

procedure TmainForm.lbxPropsDblClick(Sender: TObject);
var paramName:array[0..255] of char;
begin
 if lbxProps.ItemIndex=-1 then Exit;
 deci.setActivePreset(PChar(lbxProps.items[lbxProps.ItemIndex]),0);
 lblAutoq.caption:='autoq='+IntToStr(deci.getParam2(IDFF_autoq));
end;

end.
