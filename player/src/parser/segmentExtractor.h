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

#ifndef FRAGMENTEXTRACTOR_H
#define FRAGMENTEXTRACTOR_H

#include "crosscompilation_cxx11.h"

#include <QObject>
#include "basic/box.h"
#include "../common/segmentInfo.h"

/**
 * Class performing the extraction of file segments.
 * Segments are either ISO 23000-10 fragments or CMAF segments.
 * CMAF support tbd.
 */
class SegmentExtractor : public QObject
{
    Q_OBJECT
public:
    explicit SegmentExtractor(QObject *parent = nullptr);

public:
    //! Returns the Surveillance fragments list.
    SegmentList getSegments();

signals:

public slots:
    //! This slot is called when the fileset information is cleared.
    void onContentsCleared();
    //! This slot is called when the file is being added to a fileset.
    void onFileOpened(QString path);
    //! This slot is called when the file parsing was finished.
    void onFileClosed();
    //! This slot is called when the box is created.
    void onBoxCreated(Box *box);

private:
    //! Flag indicating, that all fragments in the fileset are Surveillance files.
    bool m_fragments_have_surveillance_boxes;
    //! Surveillance fragments list for a fileset.
    SegmentList m_segments;
};

#endif // FRAGMENTEXTRACTOR_H
