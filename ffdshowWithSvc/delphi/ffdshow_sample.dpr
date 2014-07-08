program ffdshow_sample;

uses
  Forms,
  ffdshow_sample_main in 'ffdshow_sample_main.pas' {mainForm},
  IffDecoder_com in 'IffDecoder_com.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TmainForm, mainForm);
  Application.Run;
end.
