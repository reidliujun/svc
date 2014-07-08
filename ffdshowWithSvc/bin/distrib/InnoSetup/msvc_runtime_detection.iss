[Code]
type
  RuntimeDetails = record
    cpu_arch: String;
    name: String;
    public_key_token: String;
    version: Array of String;
    majorversion: String;
    msvcr_filename: String;
  end;

function IsVista(): Boolean;
begin
  Result := InstallOnThisVersion('0,6','0,0') = irInstall;
end;

function DetectRuntime(runtime: RuntimeDetails): Boolean;
var
  FindRec: TFindRec;
  temp: String;
  foundname: String;
  subkeys: TArrayOfString;
  i,j: Integer;
  binval: String;
  rootkey: Integer;
begin
  Result := False;

  // Start detection
  for i:=0 to (GetArrayLength(runtime.version)-1) do begin
    if NOT Result then begin
      log('Start search for runtime version: ' + runtime.version[i]);
      temp := ExpandConstant('{win}\WinSxS\' + runtime.cpu_arch + '_' + runtime.name + '_' + runtime.public_key_token + '_' + runtime.version[i] + '_*');
      if FindFirst(temp, FindRec) then begin
        foundname := FindRec.Name;
        log('Found runtime folder name: '+ foundname);
        temp := ExpandConstant('{win}\WinSxS\' + foundname + '\' + runtime.msvcr_filename);
        if FileExists(temp) then begin
          log('Found runtime file: ' + temp);
          temp := ExpandConstant('{win}\WinSxS\Manifests\' + foundname + '.manifest');
          if FileExists(temp) then begin
            log('Found manifest file: ' + temp);
            // Check for policy file

            // Vista
            if IsVista then begin
              temp := ExpandConstant('{win}\WinSxS\Manifests\' + runtime.cpu_arch + '_policy.' + runtime.majorversion + '.' + runtime.name + '_' + runtime.public_key_token + '_' + runtime.version[i] + '_*');
              if FindFirst(temp, FindRec) then begin
                log('Found policy file: ' + FindRec.Name);
                // Check for registry key
                if IsWin64 then begin
                  rootkey := HKLM64;
                end
                else begin
                  rootkey := HKLM;
                end
                if RegGetSubkeyNames(rootkey, 'SOFTWARE\Microsoft\Windows\CurrentVersion\SideBySide\Winners', subkeys) then begin
                  temp := LowerCase(runtime.cpu_arch + '_' + runtime.name + '_' +  runtime.public_key_token);
                  for j:=0 to (GetArrayLength(subkeys)-1) do begin
                    if Pos(temp, LowerCase(subkeys[j])) = 1 then begin
                      if RegQueryBinaryValue(rootkey, 'SOFTWARE\Microsoft\Windows\CurrentVersion\SideBySide\Winners\' + subkeys[j] + '\' + runtime.majorversion, runtime.version[i], binval) then begin
                        log('Found policy registry key: HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\SideBySide\Winners\' + subkeys[j] + '\' + runtime.majorversion + ', ' + runtime.version[i]);
                        // Everything has been found
                        log('Runtime has successfully been detected');
                        Result := True;
                        break;
                      end
                    end
                  end
                end
                if NOT Result then begin
                  log('Registry key not found');
                end
              end
              else begin
                log('Could not find policy manifest file');
              end
            end
            // 2000/XP/2003
            else begin
              temp := ExpandConstant('{win}\WinSxS\Policies\' + runtime.cpu_arch + '_policy.' + runtime.majorversion + '.' + runtime.name + '_' + runtime.public_key_token + '_*');
              if FindFirst(temp, FindRec) then begin
                foundname := FindRec.Name;
                log('Found policy folder name: ' + foundname);
                temp := ExpandConstant('{win}\WinSxS\Policies\' + foundname + '\' + runtime.version[i] + '.policy');
                if FileExists(temp) then begin
                  log('Found policy file: ' + temp);
                  // Everything has been found
                  log('Runtime has successfully been detected');
                  Result := True;
                end
                else begin
                  log('File not found: ' + temp);
                end
              end
              else begin
                log('Could not find Policies folder');
              end
            end
          end
          else begin
            log('File not found: ' + temp);
          end
        end
        else begin
          log('File not found: ' + runtime.msvcr_filename);
        end
      end
      else begin
        log('Runtime folder not found');
      end
      log('End search for runtime version: ' + runtime.version[i]);
    end
  end
end;

function CheckForRequiredRuntimes(): Boolean;
var
  runtime: RuntimeDetails;
  runtime_name: String;
  runtime_url: String;
  ErrorCode: Integer;
begin
  Result := True;
  
#if 0
  #if VS2005SP1
    #if is64bit
    runtime_name := 'Microsoft Visual C++ 2005 SP1 Redistributable Package (x64)';
    runtime_url := 'http://www.microsoft.com/downloads/details.aspx?familyid=EB4EBE2D-33C0-4A47-9DD4-B9A6D7BD44DA&displaylang=en';
    #else
    runtime_name := 'Microsoft Visual C++ 2005 SP1 Redistributable Package (x86)';
    runtime_url := 'http://www.microsoft.com/downloads/details.aspx?familyid=200B2FD9-AE1A-4A14-984D-389C36F85647&displaylang=en';
    #endif
  #endif
  #if VS2008
    #if is64bit
    runtime_name := 'Microsoft Visual C++ 2008 Redistributable Package (x64)';
    runtime_url := 'http://www.microsoft.com/downloads/details.aspx?familyid=BD2A6171-E2D6-4230-B809-9A8D7548C1B6&displaylang=en';
    #else
    runtime_name := 'Microsoft Visual C++ 2008 Redistributable Package (x86)';
    runtime_url := 'http://www.microsoft.com/downloads/details.aspx?familyid=9B2DA534-3E03-4391-8A4D-074B9F2BC1BF&displaylang=en';
    #endif
  #endif
#endif
  //#if VS2008SP1
    #if is64bit
    runtime_name := 'Microsoft Visual C++ 2008 SP1 Redistributable Package (x64)';
    runtime_url := 'http://www.microsoft.com/downloads/details.aspx?FamilyID=ba9257ca-337f-4b40-8c14-157cfdffee4e&DisplayLang=en';
    #else
    runtime_name := 'Microsoft Visual C++ 2008 SP1 Redistributable Package (x86)';
    runtime_url := 'http://www.microsoft.com/downloads/details.aspx?FamilyID=a5c84275-3b97-4ab7-a40d-3802b2af5fc2&DisplayLang=en';
    #endif
  //#endif

  #if is64bit
  runtime.cpu_arch := 'amd64';
  #else
  runtime.cpu_arch := 'x86';
  #endif

#if 0
  #if VS2005SP1
  runtime.name := 'Microsoft.VC80.CRT';
  SetArrayLength(runtime.version, 3);
  runtime.version[0] := '8.0.50727.762';
  runtime.version[1] := '8.0.50727.1433';
  runtime.version[2] := '8.0.50727.1434';
  runtime.majorversion := '8.0';
  runtime.msvcr_filename := 'msvcr80.dll';
  #endif
  #if VS2008
  runtime.name := 'Microsoft.VC90.CRT';
  SetArrayLength(runtime.version, 2);
  runtime.version[0] := '9.0.21022.8';
  runtime.version[1] := '9.0.30411.0';
  runtime.majorversion := '9.0';
  runtime.msvcr_filename := 'msvcr90.dll';
  #endif
#endif
  //#if VS2008SP1
  runtime.name := 'Microsoft.VC90.CRT';
  SetArrayLength(runtime.version, 2);
  runtime.version[0] := '9.0.30729.1';
  runtime.version[1] := '9.0.30729.17';
  runtime.majorversion := '9.0';
  runtime.msvcr_filename := 'msvcr90.dll';
  //#endif
  
  runtime.public_key_token := '1fc8b3b9a1e18e3b';

  if NOT DetectRuntime(runtime) then begin
    Result := False;
    msgbox('ffdshow requires the following runtime package to be installed on your computer:'#13#10#13#10 + runtime_name + #13#10#13#10#13#10 + 'Please install that runtime package. After that you should be able to install ffdshow.', mbError, MB_OK);
    if msgbox('Would you like to visit the Microsoft website to download the required runtime package?', mbInformation, MB_YESNO) = idYES then begin
      ShellExec('open', runtime_url, '', '', SW_SHOW, ewNoWait, ErrorCode);
    end
  end
end;
