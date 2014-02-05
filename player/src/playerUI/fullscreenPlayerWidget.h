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

#ifndef FULLSCREENPLAYERWIDGET_H
#define FULLSCREENPLAYERWIDGET_H

#include "crosscompilation_cxx11.h"

#include <QWidget>
#include "playerWidgetInterface.h"

#include "fragmentListWidget.h"
#include "controlsWidget.h"
#include "videoFrameWidget.h"
#include "movingOutArea.h"

class FullscreenPlayerWidget : public QWidget, public PlayerWidgetInterface
{
private:
    Q_OBJECT

public:
    FullscreenPlayerWidget(QWidget* parent = 0);

    ~FullscreenPlayerWidget();
    
    virtual VideoFrameWidget* getVideoWidget() { return &m_video_frame; }
    
    virtual void setControls(ControlsWidget* controls);

    virtual void removeControls();

    //! Recalc controls placement.
    void relocateControls();

protected:
    //! On close event.
    virtual void closeEvent(QCloseEvent* event);
    
    //! On resize event.
    virtual void resizeEvent(QResizeEvent* event);

    //! On show event.
    virtual void showEvent(QShowEvent* event);

    //! On hide event.
    virtual void hideEvent(QHideEvent* event);

private:
    //! Video view UI.
    VideoFrameWidget    m_video_frame;
    //! Moving areas for conrols.
    MovingOutArea       m_controls_moving_area;
};

#endif // FULLSCREENPLAYERWIDGET_H
