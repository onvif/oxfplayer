#!/bin/bash

# Exit on error
set -e

# Function to check Ubuntu version
check_ubuntu_version() {
  if [[ $(lsb_release -rs) != "24.04" || $(lsb_release -is) != "Ubuntu" ]]; then
    echo "Error: This script is only for Ubuntu 24.04."
    exit 1
  fi
}

# Check the system is Ubuntu 24.04
check_ubuntu_version

# Install required packages
sudo apt update -y && sudo apt install -y libssl-dev cmake build-essential qtbase5-dev qt5-qmake libqt5multimedia5-plugins qtmultimedia5-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio

# Parse arguments
if [[ $# -ne 4 ]]; then
  echo "Usage: $0 <Release|Debug> <threaded-signing|unthreaded-signing> <FFMPEG_PATH> <VERSION>"
  exit 1
fi

BUILD_TYPE=$1
SIGNING_TYPE=$2
FFMPEG_PATH=$3
VERSION=$4

TEMPLATE_PACKAGE_NAME=onvif-player_ubuntu24.04-amd64
PACKAGE_NAME=onvif-player-${VERSION}_ubuntu24.04-amd64

# Validate BUILD_TYPE
if [[ $BUILD_TYPE != "Release" && $BUILD_TYPE != "Debug" ]]; then
  echo "Error: BUILD_TYPE must be 'Release' or 'Debug'."
  exit 1
fi

# Validate SIGNING_TYPE
if [[ $SIGNING_TYPE != "threaded-signing" && $SIGNING_TYPE != "unthreaded-signing" ]]; then
  echo "Error: SIGNING_TYPE must be 'threaded-signing' or 'unthreaded-signing'."
  exit 1
fi

# Validate FFMPEG_PATH
if [[ ! -d $FFMPEG_PATH ]]; then
  echo "Error: FFMPEG_PATH does not exist or is not a directory."
  exit 1
fi

# Step 6: Remove and recreate 'build' directory
rm -rf build
mkdir build

# Prepare build folder
cp -r $TEMPLATE_PACKAGE_NAME build/$PACKAGE_NAME
ln -s -r ../player build/player

cd build

# Prepare postinst file
chmod 775 $PACKAGE_NAME/DEBIAN/postinst

# Replace $VERSION in control file
sed -i "s/\$VERSION/$VERSION/g" $PACKAGE_NAME/DEBIAN/control

# Replace $VERSION in desktop file
mkdir -p $PACKAGE_NAME/usr/share/applications
sed -i "s/\$VERSION/$VERSION/g" $PACKAGE_NAME/usr/share/applications/OnvifPlayer.desktop

cd player

# Create build folder
mkdir -p build
cd build

# Run cmake
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DPLUGIN_NAME=$SIGNING_TYPE -DFFMPEG_PATH=$FFMPEG_PATH ..

# Build the project
cmake --build . -j $(nproc)

cd ../..

# Copy ONVIFPlayer binary
mkdir -p $PACKAGE_NAME/usr/local/bin
cp player/build/bin/${BUILD_TYPE}/ONVIFPlayer $PACKAGE_NAME/usr/local/bin/onvif-player
chmod 775 $PACKAGE_NAME/usr/local/bin/onvif-player

#  Copy FFmpeg shared libraries
mkdir -p $PACKAGE_NAME/usr/local/lib
cp -r $FFMPEG_PATH/lib $PACKAGE_NAME/usr/local/lib/onvif-player
rm -rf $PACKAGE_NAME/usr/local/lib/onvif-player/libavdevice.*
rm -rf $PACKAGE_NAME/usr/local/lib/onvif-player/libavfilter.*
rm -rf $PACKAGE_NAME/usr/local/lib/onvif-player/libpostproc.*
rm -rf $PACKAGE_NAME/usr/local/lib/onvif-player/pkgconfig

# Step 20: Build Debian package
dpkg-deb --build $PACKAGE_NAME

echo "Debian package built successfully: $PACKAGE_NAME"

