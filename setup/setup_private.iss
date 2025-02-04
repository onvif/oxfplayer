;/************************************************************************************
;* Copyright (c) 2013 ONVIF.
;* All rights reserved.
;*
;* Redistribution and use in source and binary forms, with or without
;* modification, are permitted provided that the following conditions are met:
;*    * Redistributions of source code must retain the above copyright
;*      notice, this list of conditions and the following disclaimer.
;*    * Redistributions in binary form must reproduce the above copyright
;*      notice, this list of conditions and the following disclaimer in the
;*      documentation and/or other materials provided with the distribution.
;*    * Neither the name of ONVIF nor the names of its contributors may be
;*      used to endorse or promote products derived from this software
;*      without specific prior written permission.
;*
;* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
;* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;* DISCLAIMED. IN NO EVENT SHALL ONVIF BE LIABLE FOR ANY DIRECT, INDIRECT,
;* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
;* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;************************************************************************************/
                    
#ifndef NormalSetup
	#error You should not execute this file directly. Call one of the custom specific setup files to build a setup (MinGW or VS)
#endif

[Setup]
AppName = ONVIFPlayer
AppVersion = 2.0 beta
DefaultDirName = {pf}\ONVIF\ONVIFPlayer
DefaultGroupName = ONVIFPlayer
LicenseFile = license.txt
OutputDir = output
OutputBaseFilename = ONVIFPlayerSetup
SetupIconFile = setup.ico

[Files]
; exe
Source: "input/ONVIFPlayer.exe"; DestDir: "{app}"
; dlls
#ifdef MinGW
Source: "input/D3DCompiler_*.dll"; DestDir: "{app}"
Source: "input/libgcc_s_sjlj-1.dll"; DestDir: "{app}"
Source: "input/libstdc++-6.dll"; DestDir: "{app}"
Source: "input/libwinpthread-1.dll"; DestDir: "{app}"
#endif
#ifdef VS
Source: "VC_redist.x64.exe"; DestDir: "{tmp}"
#endif
; Qt
Source: "input/Qt*.dll"; DestDir: "{app}"
Source: "input/plugins/platforms/*.*"; DestDir: "{app}/platforms"
; FFMpeg
Source: "input/av*.dll"; DestDir: "{app}"
Source: "input/sw*.dll"; DestDir: "{app}"
; OpenSSL
Source: "input/libcrypto*.dll"; DestDir: "{app}"
Source: "input/libssl*.dll"; DestDir: "{app}"
;Licenses
Source: "license.txt"; DestDir: "{app}"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Icons]
Name: "{group}\ONVIFPlayer"; Filename: "{app}\ONVIFPlayer.exe"; WorkingDir: "{app}"
Name: "{commondesktop}\ONVIFPlayer"; Filename: "{app}\ONVIFPlayer.exe"; WorkingDir: "{app}"; Tasks: desktopicon
Name: "{group}\Uninstall ONVIFPlayer"; Filename: "{uninstallexe}"; WorkingDir: "{app}"

[Run]
#ifdef VS
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/quiet"; Flags: waituntilterminated
#endif
Filename: "{app}\ONVIFPlayer.exe"; Flags: postinstall nowait



