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

#include "afIdentificationBoxTest.h"
#include "cameraMicrophoneIdentificationBoxTest.h"
#include "certificateBoxTest.h"
#include "compactSampleSizeBoxTest.h"
#include "engineTest.h"
#include "surveillanceExportBoxTest.h"
#include "movieHeaderBoxTest.h"
#include "movieExtendsHeaderBoxTest.h"
#include "mediaHeaderBoxTest.h"
#include "additionalUserInformationBoxTest.h"
#include "trackFragmentHeaderBoxTest.h"
#include "editListBoxTest.h"
#include "sampleDependencyTypeBoxTest.h"
#include "sampleSizeBoxTest.h"
#include "signatureBoxTest.h"
#include "signatureConfigurationBoxTest.h"
#include "surveillanceMetadataSampleConfigBoxTest.h"
#include "surveillanceMetadataSampleEntryBoxTest.h"
#include "trackFragmentRandomAccessBoxTest.h"
#include "trackHeaderBoxTest.h"
#include "trackRunBoxTest.h"
#include "certificateSSLTest.h"

int main(int argc, char *argv[])
{
    int result = 0;

    {
        AfIdentificationBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        CameraMicrophoneIdentificationBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        CertificateBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        CompactSampleSizeBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        EngineTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        SurveillanceExportBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        MediaHeaderBoxTest32  tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        MediaHeaderBoxTest64   tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        AdditionalUserInformationBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        EditListBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        MovieExtendsHeaderBoxTest32  tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        MovieExtendsHeaderBoxTest64  tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        MovieHeaderBoxTest32  tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        MovieHeaderBoxTest64   tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        SampleDependencyTypeBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        SampleSizeBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        SignatureBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        SignatureConfigurationBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        SurveillanceMetadataSampleConfigBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }    
    {
        TrackFragmentHeaderBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        TrackFragmentRandomAccessBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        TrackHeaderBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        TrackRunBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        CertificateSSLTest  tc;
        result += QTest::qExec(&tc, argc, argv);
    }
    {
        SurveillanceMetadataSampleEntryBoxTest tc;
        result += QTest::qExec(&tc, argc, argv);
    }

    return result;
}
