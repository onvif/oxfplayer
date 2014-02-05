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

#include "crosscompilation_cxx11.h"

#include <fstream>

#include "mediaParser.h"

#include "boxFactory.h"

MediaParser::MediaParser(QObject *parent) :
    QObject(parent)
{
    BoxFactory & factory = BoxFactory::instance();

    QObject::connect(&factory, &BoxFactory::boxCreated, &m_consistency_checker, &ConsistencyChecker::onBoxCreated);
    QObject::connect(&m_consistency_checker, &ConsistencyChecker::mandatoryBoxIsMissing, &factory, &BoxFactory::onMandatoryBoxIsMissing);
    QObject::connect(&m_consistency_checker, &ConsistencyChecker::mandatoryBoxesAreMissing, &factory, &BoxFactory::onMandatoryBoxesAreMissing);
    QObject::connect(&m_consistency_checker, &ConsistencyChecker::boxCountIsExceeding, &factory, &BoxFactory::onBoxCountIsExceeding);
    QObject::connect(&m_consistency_checker, &ConsistencyChecker::unexpectedBoxesMet, &factory, &BoxFactory::onUnexpectedBoxesMet);

    QObject::connect(this, &MediaParser::fileOpened, &m_validator_iso, &ValidatorISO::onFileOpened);
    QObject::connect(this, &MediaParser::fileClosed, &m_validator_iso, &ValidatorISO::onFileClosed);
    QObject::connect(this, &MediaParser::contentsCleared, &m_validator_iso, &ValidatorISO::onContentsCleared);
    QObject::connect(&factory, &BoxFactory::boxCreated, &m_validator_iso, &ValidatorISO::onBoxCreated);

    QObject::connect(this, &MediaParser::fileOpened, &m_validator_Surveillance, &ValidatorSurveillance::onFileOpened);
    QObject::connect(this, &MediaParser::fileClosed, &m_validator_Surveillance, &ValidatorSurveillance::onFileClosed);
    QObject::connect(this, &MediaParser::contentsCleared, &m_validator_Surveillance, &ValidatorSurveillance::onContentsCleared);
    QObject::connect(&factory, &BoxFactory::boxCreated, &m_validator_Surveillance, &ValidatorSurveillance::onBoxCreated);

    QObject::connect(this, &MediaParser::fileOpened, &m_validator_oxf, &ValidatorOXF::onFileOpened);
    QObject::connect(this, &MediaParser::fileClosed, &m_validator_oxf, &ValidatorOXF::onFileClosed);
    QObject::connect(this, &MediaParser::contentsCleared, &m_validator_oxf, &ValidatorOXF::onContentsCleared);
    QObject::connect(&factory, &BoxFactory::boxCreated, &m_validator_oxf, &ValidatorOXF::onBoxCreated);

    QObject::connect(this, &MediaParser::fileOpened, &m_fragment_extractor, &FragmentExtractor::onFileOpened);
    QObject::connect(this, &MediaParser::fileClosed, &m_fragment_extractor, &FragmentExtractor::onFileClosed);
    QObject::connect(this, &MediaParser::contentsCleared, &m_fragment_extractor, &FragmentExtractor::onContentsCleared);
    QObject::connect(&factory, &BoxFactory::boxCreated, &m_fragment_extractor, &FragmentExtractor::onBoxCreated);

    QObject::connect(this, &MediaParser::fileOpened, &m_signature_extractor, &SignatureExtractor::onFileAdded);
    QObject::connect(this, &MediaParser::fileClosed, &m_signature_extractor, &SignatureExtractor::onFileClosed);
    QObject::connect(this, &MediaParser::contentsCleared, &m_signature_extractor, &SignatureExtractor::onContentsCleared);
    QObject::connect(&factory, &BoxFactory::boxCreated, &m_signature_extractor, &SignatureExtractor::onBoxCreated);
}

void MediaParser::addFile(QString path)
{
    if(!m_fileset_information.contains(path))
    {
        std::shared_ptr<FileBox> fileBox(new FileBox());
        const QByteArray asc = path.toLocal8Bit();
        std::string str_path(asc.constData(), asc.length());

        LimitedStreamReader limited_stream( std::shared_ptr<std::istream>(new std::ifstream(str_path, std::ios::binary ) ) );

        emit fileOpened(path);

        fileBox->initialize(limited_stream);

        m_consistency_checker.checkFileBox(fileBox.get());

        emit fileClosed();

        m_fileset_information[path] = fileBox;
    }
}

void MediaParser::clearContents()
{
    m_fileset_information.clear();
    emit contentsCleared();
}

bool MediaParser::isValidOXF() const
{
    return m_validator_oxf.isValidFileset();
}

SurveillanceConformanceType MediaParser::isValidSurveillance()
{
    return m_validator_Surveillance.isValidFileset();
}

bool MediaParser::isValidISO()
{
    return m_validator_iso.isValidFile();
}

bool MediaParser::isValidISOFileset()
{
    return m_validator_iso.isValidFileset();
}

FragmentsList MediaParser::getFragmentsList()
{
    return m_fragment_extractor.getFragmentsList();
}

SigningInformationMap MediaParser::getSignaturesMap() const
{
    return m_signature_extractor.getSignaturesMap();
}

FilesetInformation MediaParser::getFilesetInformation()
{
    return m_fileset_information;
}
