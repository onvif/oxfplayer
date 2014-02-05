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

#ifndef ONVIFSIGNINFO_H
#define ONVIFSIGNINFO_H

#include <QList>
#include <QMap>

#include "signingInformation.h"

//! Class describing ONVIF signing information.
/*!
 * \brief Declared in Onvif Export File Format. \n
 */
class ONVIFSigningInformation
{
public:
    ONVIFSigningInformation() : m_signsList() {}

    //!
    void addSignature(const SigningInformation & signature) { m_signsList.push_back(signature); }

    //!  This method returns how many times this particular file has been signed
    int  getSignCount(void) const       { return m_signsList.count(); }

    //!  get signing information with specified index
    const SigningInformation&   getSigningInformation(int indx) const  { return m_signsList.at(indx); }

private:
    QList<SigningInformation>   m_signsList;

};

typedef QMap<QString, ONVIFSigningInformation> SigningInformationMap;

#endif // ONVIFSIGNINFO_H
