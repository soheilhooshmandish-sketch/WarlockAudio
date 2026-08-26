; Warlock Chainsaw Distortion VST3 — Inno Setup 6.3+ / 7
; Installs the Steinberg bundle to the official 64-bit VST3 folder.
; Skins/IRs are compiled into the DLL (BinaryData) AND copied into Contents/Resources.

#define MyAppName      "Warlock Chainsaw Distortion"
#define MyAppVersion   "1.1.0"
#define MyAppPublisher "Nihil Audio"
#define MyAppId        "{{C1844B36-0792-491C-9BB8-CE12976FF443}"

[Setup]
AppId={#MyAppId}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppCopyright=Copyright (C) 2026 {#MyAppPublisher}
VersionInfoVersion={#MyAppVersion}
VersionInfoCompany={#MyAppPublisher}
VersionInfoProductName={#MyAppName}

DefaultDirName={commoncf64}\VST3\Warlock.vst3
UsePreviousAppDir=yes
DisableDirPage=no
DirExistsWarning=no
DisableProgramGroupPage=yes
DisableWelcomePage=no

OutputDir=.\BuildInstaller
OutputBaseFilename=Warlock_Distortion_Setup_x64
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
SetupIconFile=
UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={sys}\imageres.dll,1

ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog

CloseApplications=yes
CloseApplicationsFilter=*.exe
RestartApplications=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Messages]
WelcomeLabel1=Warlock Chainsaw Distortion
WelcomeLabel2=This installs the 64-bit VST3 bundle into the official Steinberg folder:%n%nC:\Program Files\Common Files\VST3\Warlock.vst3%n%nSkins, knobs, logo, stomp, and cabinet IRs are inside the plugin.%n%nClose your DAW first. Admin rights are required.
WizardSelectDir=Select VST3 location
SelectDirDesc=Where should the Warlock.vst3 bundle go?
SelectDirLabel3=Leave this as Common Files\VST3\Warlock.vst3 unless your host uses a custom VST3 folder. The path must end in .vst3.
FinishedLabel=Warlock is installed. Rescan plug-ins in your DAW (Cubase: VST Plug-in Manager, Reaper: Clear cache / re-scan, Live: Plug-Ins Rescan).

[Files]
; JUCE writes a folder bundle. Unpack it INTO {app} (which IS Warlock.vst3).
Source: ".\build\Warlock_artefacts\Release\VST3\Warlock.vst3\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs restartreplace uninsrestartdelete
; Host-visible extras. Chassis/knobs/stomp/logo/IRs are compiled into the DLL.
Source: ".\Resources\legal.txt"; DestDir: "{app}\Contents\Resources"; Flags: ignoreversion
Source: ".\Resources\moduleinfo.example.json"; DestDir: "{app}\Contents\Resources"; DestName: "moduleinfo.json"; Flags: ignoreversion
Source: ".\Resources\Snapshots\*"; DestDir: "{app}\Contents\Resources\Snapshots"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\Resources\windows\desktop.ini"; DestDir: "{app}"; Flags: ignoreversion; Attribs: hidden system
Source: ".\Resources\windows\Plugin.ico"; DestDir: "{app}"; Flags: ignoreversion; Attribs: hidden

[Registry]
Root: HKLM64; Subkey: "Software\{#MyAppPublisher}\Warlock"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"; Flags: uninsdeletekey
Root: HKLM64; Subkey: "Software\{#MyAppPublisher}\Warlock"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Run]
Filename: "{win}\explorer.exe"; Parameters: """{app}"""; Description: "Open the VST3 bundle folder"; Flags: postinstall nowait skipifsilent unchecked

[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
  if not Is64BitInstallMode then
  begin
    SuppressibleMsgBox(
      'Warlock is a 64-bit VST3 and cannot install in 32-bit mode.',
      mbCriticalError, MB_OK, IDOK);
    Result := False;
  end;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
  Target: String;
begin
  Result := True;
  if CurPageID = wpSelectDir then
  begin
    Target := AddBackslash(WizardDirValue);
    if CompareText(ExtractFileExt(RemoveBackslash(WizardDirValue)), '.vst3') <> 0 then
    begin
      if MsgBox(
           'VST3 plug-ins must be a folder ending in .vst3 (Steinberg bundle).' + #13#10 +
           'Install to:' + #13#10 + Target + 'Warlock.vst3 ?',
           mbConfirmation, MB_YESNO) = IDYES then
        WizardForm.DirEdit.Text := Target + 'Warlock.vst3'
      else
        Result := False;
    end;
  end;
end;
