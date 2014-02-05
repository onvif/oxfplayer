#!/bin/bash

declare CONFIGURATION

#FFmpeg configuration
declare FFMPEG_BUILD_TYPE
declare FFMPEG_SOURCE_INCLUDE_PATH
declare FFMPEG_SOURCE_LIB_PATH
declare FFMPEG_INCLUDE_POSTFIX
declare FFMPEG_LIB_POSTFIX
readonly FFMPEG_TARGET_INCLUDE_PATH="./ext/FFMPEG-1.2/include"
readonly FFMPEG_TARGET_LIB_PATH="./ext/FFMPEG-1.2/lib/Windows"
readonly -a FFMPEG_AVCODEC_HEADERS=("avcodec.h" "avfft.h" "dxva2.h" "old_codec_ids.h" "vaapi.h" "vda.h" "vdpau.h" "version.h" "xvmc.h")
readonly -a FFMPEG_AVDEVICE_HEADERS=("avdevice.h" "version.h")
readonly -a FFMPEG_AVFILTER_HEADERS=("asrc_abuffer.h" "avcodec.h" "avfilter.h" "avfiltergraph.h" "buffersink.h" "buffersrc.h" "version.h")
readonly -a FFMPEG_AVFORMAT_HEADERS=("avformat.h" "avio.h" "version.h")
readonly -a FFMPEG_AVUTIL_HEADERS=("adler32.h" "aes.h" "attributes.h" "audioconvert.h" "audio_fifo.h" "avassert.h" "avconfig.h" "avstring.h" "avutil.h" "base64.h" "blowfish.h" "bprint.h" "bswap.h" "channel_layout.h" "common.h" "cpu.h" "crc.h" "dict.h" "error.h" "eval.h" "fifo.h" "file.h" "hmac.h" "imgutils.h" "intfloat.h" "intfloat_readwrite.h" "intreadwrite.h" "lfg.h" "log.h" "lzo.h" "mathematics.h" "md5.h" "mem.h" "old_pix_fmts.h" "opt.h" "parseutils.h" "pixdesc.h" "pixfmt.h" "random_seed.h" "rational.h" "samplefmt.h" "sha.h" "time.h" "timecode.h" "timestamp.h" "version.h" "xtea.h")
readonly -a FFMPEG_POSTPROC_HEADERS=("postprocess.h" "version.h")
readonly -a FFMPEG_SWRESAMPLE_HEADERS=("swresample.h" "version.h")
readonly -a FFMPEG_SWSCALE_HEADERS=("swscale.h" "version.h")
readonly -a FFMPEG_LIBS_ZERANOE=("avcodec.lib" "avdevice.lib" "avfilter.lib" "avformat.lib" "avutil.lib" "swresample.lib" "swscale.lib")

#OpenSSL configuration
declare OPENSSL_SOURCE_INCLUDE_PATH
declare OPENSSL_SOURCE_LIB_PATH
declare OPENSSL_TARGET_LIB_PATH
declare -a OPENSSL_LIBS
readonly OPENSSL_TARGET_INCLUDE_PATH="./ext/OpenSSL-1.0.1/include/openssl"
readonly -a OPENSSL_HEADERS=("aes.h" "asn1.h" "asn1t.h" "asn1_mac.h" "bio.h" "blowfish.h" "bn.h" "buffer.h" "camellia.h" "cast.h" "cmac.h" "cms.h" "comp.h" "conf.h" "conf_api.h" "crypto.h" "des.h" "des_old.h" "dh.h" "dsa.h" "dso.h" "dtls1.h" "ebcdic.h" "ec.h" "ecdh.h" "ecdsa.h" "engine.h" "err.h" "evp.h" "e_os2.h" "hmac.h" "krb5_asn.h" "kssl.h" "lhash.h" "md4.h" "md5.h" "modes.h" "objects.h" "obj_mac.h" "ocsp.h" "opensslconf.h" "opensslv.h" "ossl_typ.h" "pem.h" "pem2.h" "pkcs12.h" "pkcs7.h" "pqueue.h" "rand.h" "rc2.h" "rc4.h" "ripemd.h" "rsa.h" "safestack.h" "seed.h" "sha.h" "srp.h" "srtp.h" "ssl.h" "ssl2.h" "ssl23.h" "ssl3.h" "stack.h" "symhacks.h" "tls1.h" "ts.h" "txt_db.h" "ui.h" "ui_compat.h" "whrlpool.h" "x509.h" "x509v3.h" "x509_vfy.h")
readonly -a OPENSSL_LIBS_QT=("libcrypto.a" "libcrypto.dll.a" "libssl.a" "libssl.dll.a")
readonly -a OPENSSL_LIBS_VS=("libeay32.lib" "ssleay32.lib")

#PortAudio configuration
declare PORTAUDIO_SOURCE_INCLUDE_PATH
declare PORTAUDIO_SOURCE_LIB_PATH
declare PORTAUDIO_LIB_POSTFIX
declare PORTAUDIO_LIB
declare PORTAUDIO_TARGET_LIB
readonly PORTAUDIO_TARGET_INCLUDE_PATH="./ext/PortAudio/include"
readonly PORTAUDIO_TARGET_LIB_PATH="./ext/PortAudio/lib/Windows"
readonly PORTAUDIO_HEADER="portaudio.h"

#CONFIGURATION types
readonly QT="QtCreator/MinGW"
readonly VS10="Visual Studio 2010"
readonly VS12="Visual Studio 2012"

#FFMPEG_BUILD_TYPE types
readonly ZERANOE="Zeranoe"
readonly MANUAL="cross_compile_ffmpeg.sh"

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

function check_ffmpeg_includes()
{
	local INCLUDE_PATH="${1}"
	if check_file_list "${INCLUDE_PATH}/libavcodec" "${FFMPEG_AVCODEC_HEADERS[@]}"; then
		if check_file_list "${INCLUDE_PATH}/libavdevice" "${FFMPEG_AVDEVICE_HEADERS[@]}"; then
			if check_file_list "${INCLUDE_PATH}/libavfilter" "${FFMPEG_AVFILTER_HEADERS[@]}"; then
				if check_file_list "${INCLUDE_PATH}/libavformat" "${FFMPEG_AVFORMAT_HEADERS[@]}"; then
					if check_file_list "${INCLUDE_PATH}/libavutil" "${FFMPEG_AVUTIL_HEADERS[@]}"; then
						if check_file_list "${INCLUDE_PATH}/libpostproc" "${FFMPEG_POSTPROC_HEADERS[@]}"; then
							if check_file_list "${INCLUDE_PATH}/libswresample" "${FFMPEG_SWRESAMPLE_HEADERS[@]}"; then
								if check_file_list "${INCLUDE_PATH}/libswscale" "${FFMPEG_SWSCALE_HEADERS[@]}";	then
									return 0
								fi
							fi
						fi
					fi
				fi
			fi
		fi
	fi
	return 2
}

function check_ffmpeg_libs()
{
	local LIB_PATH="${1}"
	case "${FFMPEG_BUILD_TYPE}" in
		"${ZERANOE}")
			if check_file_list "${LIB_PATH}" "${FFMPEG_LIBS_ZERANOE[@]}"; then
				return 0
			fi
			;;					
		"${MANUAL}")
			if [ -f "${LIB_PATH}/libavcodec/avcodec.lib" ]; then
				if [ -f "${LIB_PATH}/libavdevice/avdevice.lib" ]; then
					if [ -f "${LIB_PATH}/libavfilter/avfilter.lib" ]; then
						if [ -f "${LIB_PATH}/libavformat/avformat.lib" ]; then
							if [ -f "${LIB_PATH}/libavutil/avutil.lib" ]; then
								if [ -f "${LIB_PATH}/libswresample/swresample.lib" ]; then
									if [ -f "${LIB_PATH}/libswscale/swscale.lib" ]; then
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

function configure_ffmpeg()
{
	until [ -n "${FFMPEG_SOURCE_INCLUDE_PATH}" ]; do
		local READ_PATH
		case "${FFMPEG_BUILD_TYPE}" in
		"${ZERANOE}")
			read -e -p "Path to FFmpeg win32-dev folder: " READ_PATH
			;;					
		"${MANUAL}")
			read -e -p "FFmpeg build path: " READ_PATH
			;;
		esac
		echo "Perfoming check..."
		local INCLUDE_PATH="${READ_PATH}/${FFMPEG_INCLUDE_POSTFIX}"
		local LIB_PATH="${READ_PATH}/${FFMPEG_LIB_POSTFIX}"
		if check_ffmpeg_includes "${INCLUDE_PATH}"; then
			if check_ffmpeg_libs "${LIB_PATH}"; then
				FFMPEG_SOURCE_INCLUDE_PATH="${INCLUDE_PATH}"
				FFMPEG_SOURCE_LIB_PATH="${LIB_PATH}"
			fi
		fi
	done
	echo "OK"
}

function configure_openssl()
{
	until [ -n "${OPENSSL_SOURCE_INCLUDE_PATH}" ]; do
		local READ_PATH
		read -e -p "OpenSSL build path: " READ_PATH
		echo "Perfoming check..."
		local INCLUDE_PATH="${READ_PATH}/include/openssl"
		local LIB_PATH="${READ_PATH}/lib"
		if check_file_list "${INCLUDE_PATH}" "${OPENSSL_HEADERS[@]}"; then
			if check_file_list "${LIB_PATH}" "${OPENSSL_LIBS[@]}"; then
				OPENSSL_SOURCE_INCLUDE_PATH="${INCLUDE_PATH}"
				OPENSSL_SOURCE_LIB_PATH="${LIB_PATH}"
			fi
		fi
	done
	echo "OK"
}

function configure_portaudio()
{
	until [ -n "${PORTAUDIO_SOURCE_INCLUDE_PATH}" ]; do
		local READ_PATH
		read -e -p "PortAudio build path: " READ_PATH
		echo "Perfoming check..."
		local INCLUDE_PATH="${READ_PATH}/include"
		local LIB_PATH="${READ_PATH}/${PORTAUDIO_LIB_POSTFIX}"
		if [ -f "${INCLUDE_PATH}/${PORTAUDIO_HEADER}" ]; then
			if [ -f "${LIB_PATH}/${PORTAUDIO_LIB}" ] ; then
				PORTAUDIO_SOURCE_INCLUDE_PATH="${INCLUDE_PATH}"
				PORTAUDIO_SOURCE_LIB_PATH="${LIB_PATH}"
			else
				echo "	Error: Missing file ${PORTAUDIO_LIB}"
			fi
		else
			echo "	Error: Missing file ${PORTAUDIO_HEADER}"
		fi
	done
	echo "OK"
}

function configure_libraries()
{
	echo "Preparing file structure..."
	echo "Please, input requested paths (TAB can be used for autocompletion)"
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
	mkdir -p "${FFMPEG_TARGET_INCLUDE_PATH}/libavcodec"
	mkdir -p "${FFMPEG_TARGET_INCLUDE_PATH}/libavdevice"
	mkdir -p "${FFMPEG_TARGET_INCLUDE_PATH}/libavfilter"
	mkdir -p "${FFMPEG_TARGET_INCLUDE_PATH}/libavformat"
	mkdir -p "${FFMPEG_TARGET_INCLUDE_PATH}/libavutil"
	mkdir -p "${FFMPEG_TARGET_INCLUDE_PATH}/libpostproc"
	mkdir -p "${FFMPEG_TARGET_INCLUDE_PATH}/libswresample"
	mkdir -p "${FFMPEG_TARGET_INCLUDE_PATH}/libswscale"
	mkdir -p "${FFMPEG_TARGET_LIB_PATH}"
	copy_file_list "${FFMPEG_SOURCE_INCLUDE_PATH}/libavcodec" "${FFMPEG_TARGET_INCLUDE_PATH}/libavcodec" "${FFMPEG_AVCODEC_HEADERS[@]}"
	copy_file_list "${FFMPEG_SOURCE_INCLUDE_PATH}/libavdevice" "${FFMPEG_TARGET_INCLUDE_PATH}/libavdevice" "${FFMPEG_AVDEVICE_HEADERS[@]}"
	copy_file_list "${FFMPEG_SOURCE_INCLUDE_PATH}/libavfilter" "${FFMPEG_TARGET_INCLUDE_PATH}/libavfilter" "${FFMPEG_AVFILTER_HEADERS[@]}"
	copy_file_list "${FFMPEG_SOURCE_INCLUDE_PATH}/libavformat" "${FFMPEG_TARGET_INCLUDE_PATH}/libavformat" "${FFMPEG_AVFORMAT_HEADERS[@]}"
	copy_file_list "${FFMPEG_SOURCE_INCLUDE_PATH}/libavutil" "${FFMPEG_TARGET_INCLUDE_PATH}/libavutil" "${FFMPEG_AVUTIL_HEADERS[@]}"
	copy_file_list "${FFMPEG_SOURCE_INCLUDE_PATH}/libpostproc" "${FFMPEG_TARGET_INCLUDE_PATH}/libpostproc" "${FFMPEG_POSTPROC_HEADERS[@]}"
	copy_file_list "${FFMPEG_SOURCE_INCLUDE_PATH}/libswresample" "${FFMPEG_TARGET_INCLUDE_PATH}/libswresample" "${FFMPEG_SWRESAMPLE_HEADERS[@]}"
	copy_file_list "${FFMPEG_SOURCE_INCLUDE_PATH}/libswscale" "${FFMPEG_TARGET_INCLUDE_PATH}/libswscale" "${FFMPEG_SWSCALE_HEADERS[@]}"
	case "${FFMPEG_BUILD_TYPE}" in
		"${ZERANOE}")
			copy_file_list "${FFMPEG_SOURCE_LIB_PATH}" "${FFMPEG_TARGET_LIB_PATH}" "${FFMPEG_LIBS_ZERANOE[@]}"
			;;					
		"${MANUAL}")
			cp -fT "${FFMPEG_SOURCE_LIB_PATH}/libavcodec/avcodec.lib" "${FFMPEG_TARGET_LIB_PATH}/avcodec.lib"
			cp -fT "${FFMPEG_SOURCE_LIB_PATH}/libavdevice/avdevice.lib" "${FFMPEG_TARGET_LIB_PATH}/avdevice.lib"
			cp -fT "${FFMPEG_SOURCE_LIB_PATH}/libavfilter/avfilter.lib" "${FFMPEG_TARGET_LIB_PATH}/avfilter.lib"
			cp -fT "${FFMPEG_SOURCE_LIB_PATH}/libavformat/avformat.lib" "${FFMPEG_TARGET_LIB_PATH}/avformat.lib"
			cp -fT "${FFMPEG_SOURCE_LIB_PATH}/libavutil/avutil.lib" "${FFMPEG_TARGET_LIB_PATH}/avutil.lib"
			cp -fT "${FFMPEG_SOURCE_LIB_PATH}/libswresample/swresample.lib" "${FFMPEG_TARGET_LIB_PATH}/swresample.lib"
			cp -fT "${FFMPEG_SOURCE_LIB_PATH}/libswscale/swscale.lib" "${FFMPEG_TARGET_LIB_PATH}/swscale.lib"
			;;
	esac
}

function copy_openssl()
{
	mkdir -p "${OPENSSL_TARGET_INCLUDE_PATH}"
	mkdir -p "${OPENSSL_TARGET_LIB_PATH}"
	copy_file_list "${OPENSSL_SOURCE_INCLUDE_PATH}" "${OPENSSL_TARGET_INCLUDE_PATH}" "${OPENSSL_HEADERS[@]}"
	copy_file_list "${OPENSSL_SOURCE_LIB_PATH}" "${OPENSSL_TARGET_LIB_PATH}" "${OPENSSL_LIBS[@]}"
}

function copy_portaudio()
{
	mkdir -p "${PORTAUDIO_TARGET_INCLUDE_PATH}"
	mkdir -p "${PORTAUDIO_TARGET_LIB_PATH}"
	cp -fT "${PORTAUDIO_SOURCE_INCLUDE_PATH}/${PORTAUDIO_HEADER}" "${PORTAUDIO_TARGET_INCLUDE_PATH}/${PORTAUDIO_HEADER}"
	cp -fT "${PORTAUDIO_SOURCE_LIB_PATH}/${PORTAUDIO_LIB}" "${PORTAUDIO_TARGET_LIB_PATH}/${PORTAUDIO_TARGET_LIB}"
}

function copy_libraries()
{
	echo "Copying required headers and libraries..."
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
				OPENSSL_TARGET_LIB_PATH="./ext/OpenSSL-1.0.1/lib"
				OPENSSL_LIBS=("${OPENSSL_LIBS_QT[@]}")
				PORTAUDIO_LIB_POSTFIX="lib/.libs"
				PORTAUDIO_LIB="libportaudio.dll.a"
				PORTAUDIO_TARGET_LIB="libportaudio.dll.a"
				;;
			"${VS10}")
				OPENSSL_TARGET_LIB_PATH="./ext/OpenSSL-1.0.1/lib-vs2010"
				OPENSSL_LIBS=("${OPENSSL_LIBS_VS[@]}")
				PORTAUDIO_LIB_POSTFIX="build/msvc/Win32/Release"
				PORTAUDIO_LIB="portaudio_x86.lib"
				PORTAUDIO_TARGET_LIB="portaudio_x86_vs2010.lib"
				;;
			"${VS12}")
				OPENSSL_TARGET_LIB_PATH="./ext/OpenSSL-1.0.1/lib-vs2012"
				OPENSSL_LIBS=("${OPENSSL_LIBS_VS[@]}")
				PORTAUDIO_LIB_POSTFIX="build/msvc/Win32/Release"
				PORTAUDIO_LIB="portaudio_x86.lib"
				PORTAUDIO_TARGET_LIB="portaudio_x86_vs2012.lib"
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
				FFMPEG_INCLUDE_POSTFIX="include"
				FFMPEG_LIB_POSTFIX="lib"
				;;
			"${MANUAL}")
				FFMPEG_INCLUDE_POSTFIX=""
				FFMPEG_LIB_POSTFIX=""
				;;
		esac
		echo "FFmpeg build type set to ${FFMPEG_BUILD_TYPE}"
		echo
		break
	done
	echo
}

function patch_ffmpeg()
{
	echo "Patching FFmpeg..."
	mv "./ext/FFMPEG-1.2/include/libavutil/common.h" "./ext/FFMPEG-1.2/include/libavutil/common.h.bak"
	sed -e 's/#include <inttypes.h>/#if (_MSC_VER <= 1700)\n#include <stdint.h>\n#else\n#include <inttypes.h>\n#endif/g' "./ext/FFMPEG-1.2/include/libavutil/common.h.bak" > "./ext/FFMPEG-1.2/include/libavutil/common.h"
	rm -f "./ext/FFMPEG-1.2/include/libavutil/common.h.bak"
	echo "OK"
}

select_configuration
configure_libraries
copy_libraries
patch_ffmpeg
echo
echo "Finished"
echo "Now you should be able to build ONVIF Player in ${CONFIGURATION} IDE"