[Setup]
AppId={{D6664B36-0792-491C-9BB8-CE12976FF666}
AppName=Warlock Satanic Black Metal Processing Suite VST3
AppVersion=1.5.0
AppPublisher=Nihil Audio Laboratories
AppPublisherURL=https://example.com
DefaultDirName={commoncf64}\VST3\Nihil Audio\Warlock
DefaultGroupName=Nihil Audio
Compression=lzma2/max
SolidCompression=yes
OutputDir=.\BuildInstaller
OutputBaseFilename=Warlock_SatanicSuite_Setup_x64
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
DisableWelcomePage=no
DisableDirPage=no
DisableProgramGroupPage=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: ".\build\Warlock_artefacts\Release\VST3\Warlock.vst3\*"; DestDir: "{commoncf64}\VST3\Warlock.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\Resources\PedalFaceplate.png"; DestDir: "{commoncf64}\VST3\Warlock.vst3\Contents\Resources"; Flags: ignoreversion
Source: ".\Resources\Cab_Center.wav"; DestDir: "{commoncf64}\VST3\Warlock.vst3\Contents\Resources"; Flags: ignoreversion
Source: ".\Resources\Cab_Edge.wav"; DestDir: "{commoncf64}\VST3\Warlock.vst3\Contents\Resources"; Flags: ignoreversion

[Registry]
Root: HKLM; Subkey: "Software\Nihil Audio\Warlock"; ValueType: string; ValueName: "Version"; ValueData: "1.5.0"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Nihil Audio\Warlock"; ValueType: string; ValueName: "InstallPath"; ValueData: "{commoncf64}\VST3\Warlock.vst3"; Flags: uninsdeletekey

[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
  if not Is64BitInstallMode then
  begin
    SuppressibleMsgBox('Fatal execution fault: Warlock requires a 64-bit operating system environment to link components.', mbCriticalError, MB_OK, MB_OK);
    Result := False;
  end;
end;
