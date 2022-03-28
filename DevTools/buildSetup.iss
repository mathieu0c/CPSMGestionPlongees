; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "CPSMGestionPlongees"
#define MyAppVersion "0.X.X"
#define MyAppPublisher "Mathieu CHARLES"
#define MyAppURL "https://github.com/mathieu0c/CPSMGestionPlongees"
#define MyAppExeName "CPSMGestionPlongees.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{E8922A2D-7D0F-45E5-BC22-148F6B3468A9}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=..\LICENSE
; Remove the following line to run in administrative install mode (install for all users.)
PrivilegesRequired=lowest
OutputDir=.
OutputBaseFilename="{#MyAppName}_setup_x64"
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: ".\{#MyAppName}_release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\{#MyAppName}_release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
;Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}";WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}"


[Code]
function CreateFile(): boolean;
var
  fileName : string;
  lines : TArrayOfString;
begin
  Result := true;
  fileName := ExpandConstant('{app}\UPDATED.TAG');
  SetArrayLength(lines, 1);
  lines[0] := '42';
  Result := SaveStringsToFile(filename,lines,true);
  exit;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if  CurStep=ssPostInstall then
    begin
         CreateFile();
    end
end;
