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

#ifndef VIDEOCONTEXT_H
#define VIDEOCONTEXT_H

class MainContext;
struct AVStream;
class VideoDecodeThread;

//! Class that contains and initialiaze main video parameters.
class VideoContext
{
public:
    VideoContext();

    ~VideoContext();

    //! Init class using video stream from Maincontext.
    /*!
     * \param main_context main context
     * \param video_stream_index index of video stream in main context
	 * \param fpsHint Hint for frame per seconds to be used in cases duration is not set
     * \return
     */
    bool open(AVStream *stream, double fpsHint);

    //! Clear parameters.
    void clear();

    //! Increase current fps by 1.0.
    void increaseFps();

    //! Decrease current fps by 1.0. fps can't be less then 0.
    void decreaseFps();

    //! Set current fps equal to standart fps.
    void flushCurrentFps();

    //! Calculate next frame delay using current fps.
    int getTimerDelay();

private:
    //! Video stream.
    AVStream*   m_video_stream;
    //! Fps readed from stream info. Average fps.
    double      m_fps;
    //! Fps used for playing. Can be greater or smaller then fps.
    double      m_current_fps;
};

#endif // VIDEOCONTEXT_H
