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

#ifndef MEDIAPARSER_H
#define MEDIAPARSER_H

#include "crosscompilation_cxx11.h"

#include <QObject>
#include <memory>
#include "basic/mixin/children.hpp"
#include "basic/fileBox.hpp"
#include "consistencyChecker.h"
#include "validatorISO.h"
#include "validatorSurveillance.h"
#include "validatorOXF.h"
#include "segmentExtractor.h"
#include "signatureExtractor.h"

typedef QMap< QString, std::shared_ptr<FileBox> > FilesetInformation;

//! Main interface class for the parser.
/*!
 * \brief Class performing all the necessary actions around file parsing, validation, fragment list extraction.
 */
class MediaParser : public QObject
{
    Q_OBJECT
public:
    explicit MediaParser(QObject *parent = 0);

public:
    //! Adds a file to a fileset, parsing its contents.
    void addFile(QString path);
    //! Clears a fileset information.
    void clearContents();
    //! Returns if the file (or a fileset) is a valid Onvif export file.
    bool isValidOXF() const;
    //! Returns if the file (or a fileset) is a valid Surveillance file.
    SurveillanceConformanceType isValidSurveillance();
    //! Returns if the file (!not a fileset) is a valid ISO base media file.
    bool isValidISO();
    //! Returns if the fileset consists of a valid ISO base media files.
    bool isValidISOFileset();
    //! Returns the Surveillance fragments list.
    SegmentList getSegments();
    //! Returns the signature information for the fragment set.
    SigningInformationMap getSignaturesMap() const;
    //! Returns the fileset parsing result, e.g. for showing them in the UI.
    FilesetInformation getFilesetInformation();

signals:
    //! This signal is sent before the file is parsed.
    void fileOpened(QString path);
    //! This signal is sent after the file parsing was finished.
    void fileClosed();
    //! This signal is sent when the fileset is cleared.
    void contentsCleared();
    
public slots:

private:
    //! Fileset information.
    FilesetInformation m_fileset_information;
    //! Consistency checker.
    ConsistencyChecker m_consistency_checker;
    //! Validator for ISO base media files.
    ValidatorISO m_validator_iso;
    //! Validator for Surveillance files.
    ValidatorSurveillance m_validator_Surveillance;
    //! Validator for Onvif export files.
    ValidatorOXF m_validator_oxf;
    //! Extractor of the Surveillance fragments list.
    SegmentExtractor m_segment_extractor;
    //! Extractor of the OXF signatures list.
    SignatureExtractor m_signature_extractor;
};

#endif // MEDIAPARSER_H
