#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Validate arguments
if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <Release|Debug> <threaded-signing|unthreaded-signing> <ffmpeg_dir> <version>"
    exit 1
fi

BUILD_TYPE=$1
SIGNING_TYPE=$2
FFMPEG_PATH=$3
VERSION=$4

# Check if the first argument is Release or Debug
if [[ "$BUILD_TYPE" != "Release" && "$BUILD_TYPE" != "Debug" ]]; then
    echo "Error: First argument must be 'Release' or 'Debug'."
    exit 1
fi

# Check if the second argument is threaded-signing or unthreaded-signing
if [[ "$SIGNING_TYPE" != "threaded-signing" && "$SIGNING_TYPE" != "unthreaded-signing" ]]; then
    echo "Error: Second argument must be 'threaded-signing' or 'unthreaded-signing'."
    exit 1
fi

# Check if the third argument is a valid directory
if [ ! -d "$FFMPEG_PATH" ]; then
    echo "Error: Third argument must be a valid directory."
    exit 1
fi

# Check if flatpak is installed and executable
if ! command -v flatpak &> /dev/null; then
    echo "Error: 'flatpak' is not installed or not in PATH. Please install it and try again."
    exit 1
fi

# Check if flatpak-builder is installed and executable
if ! command -v flatpak-builder &> /dev/null; then
    echo "Error: 'flatpak-builder' is not installed or not in PATH. Please install it and try again."
    exit 1
fi

# Remove 'build' directory if it exists and create a new one
rm -rf build
mkdir build

# Copy necessary files to the build directory
cp org.onvif.Player.yaml build/

# Create a symlink to the player folder in the build directory
ln -s -r "../player" build/player

# Create a symlink to the ffmpeg folder in the build directory
ln -s "$FFMPEG_PATH" build/ffmpeg

# Prepare share folder
cp -r ./share build/share

# Change to the build directory
cd build

# Replace placeholders
sed -i "s|\$VERSION|$VERSION|g" share/org.onvif.Player.desktop
sed -i "s|\$BUILD_TYPE|$BUILD_TYPE|g" org.onvif.Player.yaml
sed -i "s|\$SIGNING_TYPE|$SIGNING_TYPE|g" org.onvif.Player.yaml

# Add the Flathub repository if it doesn't exist
flatpak remote-add --if-not-exists --user flathub https://dl.flathub.org/repo/flathub.flatpakrepo

# Build the Flatpak package
flatpak-builder --force-clean --user --install-deps-from=flathub --repo=repo builddir org.onvif.Player.yaml

# Create the Flatpak bundle
flatpak build-bundle repo onvif-player-$VERSION-x86_64.flatpak org.onvif.Player

echo "Build completed successfully. Output file: onvif-player-$VERSION-x86_64.flatpak"

