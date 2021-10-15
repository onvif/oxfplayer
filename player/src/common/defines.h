/************************************************************************************
* Copyright (c) 2013 ONVIF.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*    * Neither the name of ONVIF nor the names of its contributors may be
*      used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ONVIF BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************/

#ifndef DEFINES_H
#define DEFINES_H

#include <QObject>

//! After that count DecodeThread will sent queryFilled() signal.
#define MINIMUM_FRAMES_IN_QUEUE_TO_START 20

//! DecodeThread will try to keep query size in this value.
#define MINIMUM_FRAMES_IN_QUEUE 30

//! DecodeThread will read such count of frames per one round.
#define FRAMES_TO_READ 10

//! Sleep timeout for DecodeThread.
#define DECODE_SLEEP_TIMEOUT 50

//! Extentions for Open File dialog.
#define AVAILIBLE_EXTENTIONS "Video (*.mp4 *.mov);;All (*.*)"

//! Notification interval for audio playback.
#define AUDIO_NOTIFY_TIMEOUT 200

//! Sleep timeout for decoders threads.
#define WAIT_TREAD 50

//! Backstep for seeking in ms.
#define SEEK_BACKSTEP 6400

//! Moving area speed.
#define MOVING_AREA_SPEED 10

//! Moving area step.
#define MOVING_AREA_STEP 15

//! Moving area show time.
#define MOVING_AREA_SHOW_TIME 5000

//! Format of conversion of QDateTime to string
#define DATETIME_CONVERSION_FORMAT Qt::ISODate

//! Short ormat of conversion of QDateTime to string
#define DATETIME_SHORT_CONVERSION_FORMAT "hh:mm:ss"

//! Windows path to config and certificates.
#define WINP_APP_DATA_ROAMING "/AppData/Roaming/"

//! Company name for config file.
#define COMPANY_NAME "ONVIF"

//! Project name for config file.
#define PRODUCT_NAME "ONVIFPlayer"

//! Path to config file
#define CONFIG_FILE_NAME "config.ini"

//! Folder for certificates
#define CERTIFICATES_FOLDER "KnownCerts"

//! Binary format filter
#define BINARY_FORMAT QObject::tr("Binary format (*.der)")

//! Binary filter.
#define BINARY_FILTER "*.der"

//! Binary extention
#define BINARY_EXT ".der"

//! Base 64 encoding
#define BASE64_FORMAT QObject::tr("Base 64 encoded (*.pem)")

//! Base 64 extention
#define BASE64_EXT ".pem"

#endif // DEFINES_H
