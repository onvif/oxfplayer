# ONVIF_player

## How to build

### Needed applications/libraries
You need next applications/libraries to be installed:
- Visual Studio 2022 (for Windows build)
- Qt 5.12
- Qt add-on for Visual Studio (for Windows build) (https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools2022)
- CMake (https://cmake.org/download/)
- OpenSSL 3.3.1 (https://slproweb.com/products/Win32OpenSSL.html)
- FFmgpeg 4.4, not higher (https://github.com/andrewGuzGRSE/FFmpegBuilds, https://github.com/BtbN/FFmpeg-Builds)
- GStreamer 1.0 or higher (https://gstreamer.freedesktop.org/download/#windows)

### Update submodules after clone
```
git submodule update --init --recursive
```

### Build on Windows

#### Add environment variables
To compile you should set 3 environment variables:
- OPENSSL_PATH (something like C:\Program Files\OpenSSL-Win64)
- FFMPEG_PATH (something like C:\ffmpeg-4.4)
- GSTREAMER (something like C:\gstreamer\1.0\msvc_x86_64)

All of them should point to root folders, where include and lib subfolders can be found.

#### Prepare 3rd party libraries
```
cd ./player/3rdparty
./prepare3rdparty.bat
```

#### Build player

Qt must be configured in Visual Studio with name `Qt 5.12 x64` (like C:\Qt\Qt5.12.12\5.12.12\msvc2017_64)
![image](https://github.com/user-attachments/assets/8c7cd7ef-08ca-47c2-bd1b-5278e0a0a218)

Open `player/proj/OXFPlayer-VS2022/OXFPlayer.sln` and build the player.

### Build on Linux


```
cd player
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=BUILD_TYPE -DPLUGIN_NAME=PLUGIN_NAME -DFFMPEG_PATH=PATH_TO_LOCAL_FFMPEG .. 
cmake --build .
```
Where
- BUILD_TYPE can be 'Debug' or 'Release'
- PLUGIN_NAME can be `threaded-signing` or `unthreaded-signing`
- PATH_TO_LOCAL_FFMPEG is a path to FFmpeg 4.4 if it's not a part of you Linux

In case of using not system FFmpeg, you will have to copy it's so files near by Player executable.
