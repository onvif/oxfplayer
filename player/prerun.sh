#!/bin/bash

declare CONFIGURATION

#Output path
declare TARGET_BIN_PATH

#FFmpeg configuration
declare FFMPEG_BUILD_TYPE
declare FFMPEG_BIN_PATH
declare FFMPEG_BIN_POSTFIX
readonly -a FFMPEG_DLLS_ZERANOE=("avcodec-54.dll" "avdevice-54.dll" "avfilter-3.dll" "avformat-54.dll" "avutil-52.dll" "swresample-0.dll" "swscale-2.dll")

#OpenSSL configuration
declare OPENSSL_BIN_PATH
readonly -a OPENSSL_DLLS=("libeay32.dll" "ssleay32.dll")

#PortAudio configuration
declare PORTAUDIO_BIN_PATH
declare PORTAUDIO_BIN_POSTFIX
declare PORTAUDIO_DLL

#CONFIGURATION types
readonly QT="QtCreator/MinGW"
readonly VS10="Visual Studio 2010"
readonly VS12="Visual Studio 2012"

#FFMPEG_BUILD_TYPE types
readonly ZERANOE="Zeranoe"
readonly MANUAL="cross_compile_ffmpeg.sh"

readonly EXE_NAME="ONVIFPlayer.exe"

function check_file_list()
{
	local -r PATH_PREFIX="${1}"
	shift
	local -ar FILESET=("${@}")
	local -i RESULT=0
	for FILENAME in "${FILESET[@]}"; do
		if [ ! -f "${PATH_PREFIX}/${FILENAME}" ]; then
			echo "	Error: Missing file ${FILENAME}"
			RESULT=2
		fi
	done
	return ${RESULT}
}

function copy_file_list()
{
	local -r PATH_PREFIX="${1}"
	local -r TARGET_PATH="${2}"
	shift
	shift
	local -ar FILESET=("${@}")
	for FILENAME in "${FILESET[@]}"; do
		cp -fT "${PATH_PREFIX}/${FILENAME}" "${TARGET_PATH}/${FILENAME}"
	done
}

function check_ffmpeg_dlls()
{
	local BIN_PATH="${1}"
	case "${FFMPEG_BUILD_TYPE}" in
		"${ZERANOE}")
			if check_file_list "${BIN_PATH}" "${FFMPEG_DLLS_ZERANOE[@]}"; then
				return 0
			fi
			;;					
		"${MANUAL}")
			if [ -f "${BIN_PATH}/libavcodec/avcodec-54.dll" ]; then
				if [ -f "${BIN_PATH}/libavdevice/avdevice-54.dll" ]; then
					if [ -f "${BIN_PATH}/libavfilter/avfilter-3.dll" ]; then
						if [ -f "${BIN_PATH}/libavformat/avformat-54.dll" ]; then
							if [ -f "${BIN_PATH}/libavutil/avutil-52.dll" ]; then
								if [ -f "${BIN_PATH}/libswresample/swresample-0.dll" ]; then
									if [ -f "${BIN_PATH}/libswscale/swscale-2.dll" ]; then
										return 0
									fi
								fi
							fi
						fi
					fi
				fi
			fi
			;;
	esac
	return 2
}

function configure_output()
{
	until [ -n "${TARGET_BIN_PATH}" ]; do
		local READ_PATH
		read -e -p "Target binary path: " READ_PATH
		echo "Performing check..."
		if [ -d "${READ_PATH}" ]; then
			if [ -f "${READ_PATH}/${EXE_NAME}" ]; then
				TARGET_BIN_PATH="${READ_PATH}"
			else
				echo "	Warning: ONVIFPlayer executable not found at the target path. Continue?"
				select CONTINUE in Yes No;
				do
					case "${CONTINUE}" in
						"Yes")
							TARGET_BIN_PATH="${READ_PATH}"
						;;
						"No")
							echo "Please, input target path once more"
						;;
					esac
					break
				done
			fi
		fi
	done
	echo "Target path set to ${TARGET_BIN_PATH} "
}

function configure_ffmpeg()
{
	until [ -n "${FFMPEG_BIN_PATH}" ]; do
		local READ_PATH
		case "${FFMPEG_BUILD_TYPE}" in
		"${ZERANOE}")
			read -e -p "Path to FFmpeg win32-shared folder: " READ_PATH
			;;					
		"${MANUAL}")
			read -e -p "FFmpeg build path: " READ_PATH
			;;
		esac
		echo "Perfoming check..."
		local BIN_PATH="${READ_PATH}/${FFMPEG_BIN_POSTFIX}"
		if check_ffmpeg_dlls "${BIN_PATH}"; then
			FFMPEG_BIN_PATH="${BIN_PATH}"
		fi
	done
	echo "OK"
}

function configure_openssl()
{
	until [ -n "${OPENSSL_BIN_PATH}" ]; do
		local READ_PATH
		read -e -p "OpenSSL build path: " READ_PATH
		echo "Perfoming check..."
		local BIN_PATH="${READ_PATH}/bin"
		if check_file_list "${BIN_PATH}" "${OPENSSL_DLLS[@]}"; then
			OPENSSL_BIN_PATH="${BIN_PATH}"
		fi
	done
	echo "OK"
}

function configure_portaudio()
{
	until [ -n "${PORTAUDIO_BIN_PATH}" ]; do
		local READ_PATH
		read -e -p "PortAudio build path: " READ_PATH
		echo "Perfoming check..."
		local BIN_PATH="${READ_PATH}/${PORTAUDIO_BIN_POSTFIX}"
		if [ -f "${BIN_PATH}/${PORTAUDIO_DLL}" ]; then
			PORTAUDIO_BIN_PATH="${BIN_PATH}"
		else
			echo "	Error: Missing file ${PORTAUDIO_DLL}"
		fi
	done
	echo "OK"
}

function configure_libraries()
{
	echo "Preparing file structure..."
	echo "Please, input requested paths (TAB can be used for autocompletion)"
	echo
	configure_output
	echo
	configure_ffmpeg
	echo
	configure_openssl
	echo
	configure_portaudio
	echo
}

function copy_ffmpeg()
{
	case "${FFMPEG_BUILD_TYPE}" in
		"${ZERANOE}")
			copy_file_list "${FFMPEG_BIN_PATH}" "${TARGET_BIN_PATH}" "${FFMPEG_DLLS_ZERANOE[@]}"
			;;					
		"${MANUAL}")
			cp -fT "${FFMPEG_BIN_PATH}/libavcodec/avcodec-54.dll" "${TARGET_BIN_PATH}/avcodec-54.dll"
			cp -fT "${FFMPEG_BIN_PATH}/libavdevice/avdevice-54.dll" "${TARGET_BIN_PATH}/avdevice-54.dll"
			cp -fT "${FFMPEG_BIN_PATH}/libavfilter/avfilter-3.dll" "${TARGET_BIN_PATH}/avfilter-3.dll"
			cp -fT "${FFMPEG_BIN_PATH}/libavformat/avformat-54.dll" "${TARGET_BIN_PATH}/avformat-54.dll"
			cp -fT "${FFMPEG_BIN_PATH}/libavutil/avutil-52.dll" "${TARGET_BIN_PATH}/avutil-52.dll"
			cp -fT "${FFMPEG_BIN_PATH}/libswresample/swresample-0.dll" "${TARGET_BIN_PATH}/swresample-0.dll"
			cp -fT "${FFMPEG_BIN_PATH}/libswscale/swscale-2.dll" "${TARGET_BIN_PATH}/swscale-2.dll"
			;;
	esac
}

function copy_openssl()
{
	copy_file_list "${OPENSSL_BIN_PATH}" "${TARGET_BIN_PATH}" "${OPENSSL_DLLS[@]}"
}

function copy_portaudio()
{
	cp -fT "${PORTAUDIO_BIN_PATH}/${PORTAUDIO_DLL}" "${TARGET_BIN_PATH}/${PORTAUDIO_DLL}"
}

function copy_libraries()
{
	echo "Collecting and copying required libraries..."
	copy_ffmpeg
	copy_openssl
	copy_portaudio
}

function select_configuration()
{
	echo "Welcome to ONVIF Player configuration builder"
	echo
	echo "Please, select target configuration"
	select CONFIGURATION in "${QT}" "${VS10}" "${VS12}";
	do
		case "${CONFIGURATION}" in
			"${QT}")
				PORTAUDIO_BIN_POSTFIX="lib/.libs"
				PORTAUDIO_DLL="libportaudio-2.dll"
				;;
			"${VS10}")
				PORTAUDIO_BIN_POSTFIX="build/msvc/Win32/Release"
				PORTAUDIO_DLL="portaudio_x86.dll"
				;;
			"${VS12}")
				PORTAUDIO_BIN_POSTFIX="build/msvc/Win32/Release"
				PORTAUDIO_DLL="portaudio_x86.dll"
				;;
		esac
		echo "Target configuration set to ${CONFIGURATION}"
		echo
		break
	done
	echo "Please, select FFmpeg build type"
	select FFMPEG_BUILD_TYPE in "${ZERANOE}" "${MANUAL}";
	do
		case "${FFMPEG_BUILD_TYPE}" in
			"${ZERANOE}")
				FFMPEG_BIN_POSTFIX="bin"
				;;
			"${MANUAL}")
				FFMPEG_BIN_POSTFIX=""
				;;
		esac
		echo "FFmpeg build type set to ${FFMPEG_BUILD_TYPE}"
		echo
		break
	done
	echo
}

select_configuration
configure_libraries
copy_libraries
echo
echo "Finished"
echo "Now you should be able to run ONVIF Player from within ${CONFIGURATION} IDE"