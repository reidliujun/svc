program ffdshow_remote;

uses
  Forms,
  ffdshow_remote_main in 'ffdshow_remote_main.pas' {mainForm},
  ffdshowRemoteAPI in 'ffdshowRemoteAPI.pas',
  IffDecoder_com in 'IffDecoder_com.pas';

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TmainForm, mainForm);
  Application.Run;
end.
