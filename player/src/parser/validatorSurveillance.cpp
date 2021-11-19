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

#include "validatorSurveillance.h"

#include "afIdentificationBox.hpp"

ValidatorSurveillance::ValidatorSurveillance(QObject *parent) :
    QObject(parent)
{
}

SurveillanceConformanceType ValidatorSurveillance::isValidFile()
{
    if((m_fileset_information.size() == 1) && (m_fileset_information[m_current_file].isValid()))
    {
        return SurveillanceConformant;
    }
    return IsNotSurveillanceFormat;
}

SurveillanceConformanceType ValidatorSurveillance::isValidFileset()
{
    if(m_fileset_information.size() == 1)
    {
        SurviellanceFileInformation & info = m_fileset_information.begin().value();
        if(info.isValid())
        {
            if(info.m_segment_type == SurviellanceFileInformation::StandaloneFragment)
                return SurveillanceConformant;
            else
                return IsNotSurveillanceFileset;
        }
        else
        {
            return IsNotSurveillanceFormat;
        }
    }

    bool hasStartFragment = false, hasFinalFragment = false;
    for(auto it = m_fileset_information.begin(), end = m_fileset_information.end(); it != end; ++it)
    {
		const SurviellanceFileInformation & info = *it;
        if(info.isValid() == false)
        {
            return IsNotSurveillanceFormat;
        }
        switch(info.m_segment_type)
        {
        case SurviellanceFileInformation::StartFragment:
            {
                if(hasStartFragment)
                    return IsNotSurveillanceFileset;
                hasStartFragment = true;
            }
            break;
        case SurviellanceFileInformation::FinalFragment:
            {
                if(hasFinalFragment)
                    return IsNotSurveillanceFileset;
                hasFinalFragment = true;
            }
            break;
        case SurviellanceFileInformation::MiddleFragment:
            break;
        default:
            return IsNotSurveillanceFileset;
        }
    }
    if(hasStartFragment && hasFinalFragment)
        return SurveillanceConformant;
    else
        return IsNotSurveillanceFileset;
}

void ValidatorSurveillance::onContentsCleared()
{
    m_fileset_information.clear();
}

void ValidatorSurveillance::onFileOpened(QString path)
{
    m_fileset_information.insert(path, SurviellanceFileInformation());
    m_current_file = path;
}

void ValidatorSurveillance::onFileClosed()
{
    m_current_file.clear();
}

void ValidatorSurveillance::onBoxCreated(Box *box)
{
    static const FourCC sc_sumi_four_cc(AFIdentificationBox::getFourCC());

    FourCC four_cc = box->getBoxFourCC();

    if(!m_current_file.isEmpty())
    {
        if(four_cc == sc_sumi_four_cc)
        {
            SurviellanceFileInformation & current_file_info = m_fileset_information[m_current_file];

            current_file_info.m_af_identification_box_count++;
            current_file_info.m_segment_type = SurviellanceFileInformation::FragmentType(current_file_info.m_segment_type | SurviellanceFileInformation::IsSurveillance);

            AFIdentificationBox * af_identification_box = dynamic_cast<AFIdentificationBox*>(box);
            current_file_info.m_predecessor_UUID = af_identification_box->getPredecessorUUID();
            current_file_info.m_segment_UUID = af_identification_box->getFragmentUUID();
            current_file_info.m_successor_UUID = af_identification_box->getSuccessorUUID();

            if(current_file_info.m_predecessor_UUID == current_file_info.m_segment_UUID)
            {
                current_file_info.m_segment_type = SurviellanceFileInformation::FragmentType(current_file_info.m_segment_type | SurviellanceFileInformation::IsStartFragment);
            }
            if(current_file_info.m_successor_UUID == current_file_info.m_segment_UUID)
            {
                current_file_info.m_segment_type = SurviellanceFileInformation::FragmentType(current_file_info.m_segment_type | SurviellanceFileInformation::IsFinalFragment);
            }

            for(auto it = m_fileset_information.begin(), end = m_fileset_information.end(); it != end; ++it)
			{
				SurviellanceFileInformation & file_info = *it;
                if(file_info.m_segment_UUID != current_file_info.m_segment_UUID)
                {
                    if((file_info.m_successor_UUID == current_file_info.m_segment_UUID)
                            && (current_file_info.m_predecessor_UUID == file_info.m_segment_UUID))
                    {
                        file_info.m_segment_type = SurviellanceFileInformation::FragmentType(file_info.m_segment_type | SurviellanceFileInformation::HasSuccessor);
                        current_file_info.m_segment_type = SurviellanceFileInformation::FragmentType(current_file_info.m_segment_type | SurviellanceFileInformation::HasPredecessor);
                    }
                    if((file_info.m_predecessor_UUID == current_file_info.m_segment_UUID)
                            && (current_file_info.m_successor_UUID == file_info.m_segment_UUID))
                    {
                        file_info.m_segment_type = SurviellanceFileInformation::FragmentType(file_info.m_segment_type | SurviellanceFileInformation::HasPredecessor);
                        current_file_info.m_segment_type = SurviellanceFileInformation::FragmentType(current_file_info.m_segment_type | SurviellanceFileInformation::HasSuccessor);
                    }
                }
            }
        }
    }
}
