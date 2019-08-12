; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Amazon Polly TTS for Windows"
#define MyAppVersion ".1"
#define MyAppPublisher "Amazon Web Services"
#define MyAppURL "https://aws.amazon.com/polly"
#define DebugOrRelease "Release"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A6481E88-FD76-4BE2-BDF9-BB38F8A37B95}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commonpf}\{#MyAppName}
DefaultGroupName={#MyAppName}                  
OutputBaseFilename=setup
OutputDir=installer
Compression=lzma
SolidCompression=yes            
PrivilegesRequired=admin
LicenseFile=license.txt
WizardStyle=modern   
DisableWelcomePage=no
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: ".\InstallVoices\{#DebugOrRelease}\InstallVoices.exe"; DestDir: "{app}"; Flags: ignoreversion 64bit; Check: IsWin64
Source: ".\InstallVoices\{#DebugOrRelease}\aws-c-common.dll"; DestDir: "{app}"; Flags: ignoreversion 64bit; Check: IsWin64
Source: ".\InstallVoices\{#DebugOrRelease}\aws-c-event-stream.dll"; DestDir: "{app}"; Flags: ignoreversion 64bit; Check: IsWin64
Source: ".\InstallVoices\{#DebugOrRelease}\aws-checksums.dll"; DestDir: "{app}"; Flags: ignoreversion 64bit; Check: IsWin64
Source: ".\InstallVoices\{#DebugOrRelease}\aws-cpp-sdk-core.dll"; DestDir: "{app}"; Flags: ignoreversion 64bit; Check: IsWin64
Source: ".\InstallVoices\{#DebugOrRelease}\aws-cpp-sdk-polly.dll"; DestDir: "{app}"; Flags: ignoreversion 64bit; Check: IsWin64
Source: ".\PollyTTSEngine\{#DebugOrRelease}\PollyTTSWindows.dll"; DestDir: "{app}"; Flags: ignoreversion regserver 64bit; Check: IsWin64
Source: ".\PollyTTSEngine\{#DebugOrRelease}\tinyxml2.dll"; DestDir: "{app}"; Flags: ignoreversion 64bit; Check: IsWin64
Source: ".\PollyTTSEngine\{#DebugOrRelease}\fmt.dll"; DestDir: "{app}"; Flags: ignoreversion 64bit; Check: IsWin64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Run]
Filename: "{app}\InstallVoices.exe"; Flags: runascurrentuser; Parameters: "install"; StatusMsg: "Installing Voices..."

[UninstallRun]
Filename: "{app}\InstallVoices.exe"; Parameters: "uninstall"

[Code]  
var
  PricingPage: TInputOptionWizardPage;
procedure OpenBrowser(Url: string);
var
  ErrorCode: Integer;
begin
  ShellExec('open', Url, '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

procedure ValidatePage;
begin
  WizardForm.NextButton.Enabled := (PricingPage.SelectedValueIndex=1);
end;  

procedure EditChange(Sender: TObject);
begin
  ValidatePage;
end;

procedure LinkLabelClick(Sender: TObject);
begin
  OpenBrowser('https://aws.amazon.com/polly/pricing');
end;

procedure ActivatePricingPage(Page: TWizardPage);
var
  LinkLabel: TLabel;
begin
  WizardForm.NextButton.Enabled := False;
  LinkLabel := TLabel.Create(WizardForm);
  LinkLabel.Parent := WizardForm;
  LinkLabel.Left := ScaleX(16);
  LinkLabel.Top :=
  WizardForm.NextButton.Top + (WizardForm.NextButton.Height div 2) -
    (LinkLabel.Height div 2);
  LinkLabel.Caption := 'Amazon Polly pricing page';
  LinkLabel.ParentFont := True;
  LinkLabel.Font.Style := LinkLabel.Font.Style + [fsUnderline];
  LinkLabel.Font.Color := clBlue;
  LinkLabel.Cursor := crHand;
  LinkLabel.OnClick := @LinkLabelClick;
end;

procedure PageActivate(Sender: TWizardPage);
begin
  ValidatePage;
end;

procedure InitializeWizard;
begin
  PricingPage := CreateInputOptionPage(wpWelcome,
    'Pricing Information', 'Review Amazon Polly pricing',
    'Before continuing, please review the Amazon Polly pricing at the link below.',
    True, False);
  PricingPage.Add('I have NOT read and understand the Amazon Polly pricing terms');
  PricingPage.Add('I have read and understand the Amazon Polly pricing terms');
  PricingPage.CheckListBox.OnClickCheck := @EditChange;
  PricingPage.OnActivate := @ActivatePricingPage
  
end;
