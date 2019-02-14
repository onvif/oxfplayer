# Building the installer
This file contains how_to_build instructions for ONVIF Export Player Installer

## 1. Get Inno Setup

Get and install Inno Setup from http://www.jrsoftware.org/download.php/is.exe


## 2. Add executable and dlls to the 'input' folder

Make sure that you added all required binaries to the "input" folder:

* player executable;
* Qt libraries with 'platforms' subfolder (can be found in plugins folder);
* FFMpeg dlls;
* PortAudio dll;
* OpenSLL dlls;

Check: You can verify that all files are added if you can start player executable from 'input' folder successfully.

## 3. Compile installer

Open setup_MinGW.iss or setup_VS.iss file using Inno Setup and compile it (Ctrl+F9).


## 4. Result

Result file named ONVIFPlayerSetup.exe will appear in "output" folder.

