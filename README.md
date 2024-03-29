# oxfplayer - ONVIF Export File Player

Reference player for signed ONVIF export files with metadata.

![Screenshot](http://developer.onvif.org/pub/export/oxfplayer-1.jpg)

## Overview
The reference player is provided by ONVIF as open source to ease third party verification of the cryptographic mechanism. 
It supports playback of mp4 and qt files with and without absolute timestamps. 
Authenticity of the media can be verified via signature verification. 

The player supports rendering of timed metadata bounding boxes and object shapes. 
Recorded metadata events are displayed in the left hand panel with on click timeline navigation.

## Environment
The UI is based on Qt. Build envrionments are provided for Windows and Linux.
## License

BSD 3 clause

## Binaries
 * [Version 2.1 (Windows)](https://github.com/onvif/oxfplayer/releases/download/2.1/ONVIFPlayerSetup-2.1.exe)
 * [Version 2.0 (Windows)](https://github.com/onvif/oxfplayer/releases/download/2.0/ONVIFPlayerSetup.exe)
 * [Version 1.1 (Windows)](https://github.com/onvif/oxfplayer/blob/1.1/ONVIFPlayerSetup.exe)

## References

 * [ISO/IEC 14496-12](https://www.iso.org/standard/68960.html)
 * [ONVIF Export File Format](https://www.onvif.org/specs/stream/ONVIF-ExportFileFormat-Spec.pdf)
 * [MP4RA Box registry](https://mp4ra.org/#/atoms)
