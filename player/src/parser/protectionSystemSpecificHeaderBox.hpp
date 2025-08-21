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

#ifndef PSSH_BOX_H
#define PSSH_BOX_H

#include "basic/fullBox.hpp"
#include <openssl/hpke.h>

typedef uint8_t KidType[16];

//! Class describing Surveillance Export box.
/*!
 * \brief Declared in ISO 23001-7 and Onvif Recording Encryption. \n
 * Type: 'pssh' \n
 * Container: moov, file level \n
 * Mandatory: No \n
 * Quantity: Zero or one
 */
class ProtectionSystemSpecificHeaderBox
        : public FullBox<QUuid, uint32_t>
{
public:
    typedef FullBox<QUuid, uint32_t> FullBoxType;

public:
    explicit ProtectionSystemSpecificHeaderBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream)
    {
        FullBoxType::initialize(stream);
        readContents(stream);
    }

public:
    QUuid getSystemID() { return std::get<2>(FullBoxType::m_data); }
    uint32_t getKID_count() { return std::get<3>(FullBoxType::m_data); }
    HexArray getKID() { return HexArray(m_kid, sizeof(m_kid)); }
    uint32_t getDataSize() { return m_dataSize; }
    uint32_t getEntries() { return m_entries; }
    uint16_t getCertThumbprintAlg() { return m_thumbAlg; }
    HexArray getCertThumbprint() { return HexArray(m_thumb, m_thumbSize); }
    uint16_t getEncryptionVersion() { return m_encVersion; }
    HexArray getEncryptedKey() { return HexArray(m_encKey, m_encDataSize); }
    uint16_t getHPKE_KEM() { return m_hpke[0]; }
    uint16_t getHPKE_KDF() { return m_hpke[1]; }
    uint16_t getHPKE_AEAD() { return m_hpke[2]; }
    uint16_t getSharedSecretSize() { 
        switch (m_hpke[0]) {
        case OSSL_HPKE_KEM_ID_P256: return 65;
        case OSSL_HPKE_KEM_ID_P384: return 97;
        case OSSL_HPKE_KEM_ID_P521: return 209;
        case OSSL_HPKE_KEM_ID_X25519: return 32;
        default: return 0;
        }
    }
    HexArray getSharedSecret() { return HexArray(m_sharedSecret, getSharedSecretSize()); }

public:
    BOX_INFO("pssh", "Protection System Specific Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() override
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(SystemID);
        BOX_PROPERTY(KID_count);
        BOX_PROPERTY(KID);
        BOX_PROPERTY(DataSize);
        BOX_PROPERTY(Entries);
        BOX_PROPERTY(CertThumbprintAlg);
        BOX_PROPERTY(CertThumbprint);
        BOX_PROPERTY(EncryptionVersion);
        BOX_PROPERTY(EncryptedKey);
        BOX_PROPERTY(HPKE_KEM);
        BOX_PROPERTY(HPKE_KDF);
        BOX_PROPERTY(HPKE_AEAD);
        BOX_PROPERTY(SharedSecret);
    }

private:
    //! Reads the lists of video and audio entries from the stream.
    void readContents(LimitedStreamReader &stream)
    {
//		int version = getVersion();
        stream.read(m_kid, 16);
        stream.read(m_dataSize);
        stream.read(m_entries);
        stream.read(m_thumbAlg);
        stream.read(m_thumbSize);
        stream.read(m_thumb, m_thumbSize);
        stream.read(m_encVersion);
        if (m_encVersion == 1) {
            stream.read(m_encDataSize);
            if (m_encDataSize <= sizeof(m_encKey)) {
                stream.read(m_encKey, m_encDataSize);
            }
        }
        else if (m_encVersion == 2) {
            stream.read(m_hpke[0]);
            stream.read(m_hpke[1]);
            stream.read(m_hpke[2]);
            int sharedSecretSize = getSharedSecretSize();
            if (sharedSecretSize <= sizeof(m_sharedSecret)) {
                stream.read(m_sharedSecret, sharedSecretSize);
            }
            uint16_t u16;
            stream.read(u16);
            m_encDataSize = u16;
            if (m_encDataSize <= sizeof(m_encKey)) {
                stream.read(m_encKey, m_encDataSize);
            }
        }
    }

private:
    uint8_t m_kid[16];
    uint32_t m_dataSize;
    uint32_t m_entries;
    uint16_t m_thumbAlg;
    uint32_t m_thumbSize;
    uint8_t m_thumb[128];
    uint8_t m_encVersion{};
    uint32_t m_encDataSize{};
    uint8_t m_encKey[1024];
    uint16_t m_hpke[3]{};
    uint8_t m_sharedSecret[128]{};
}; 

#endif // SURVIELANCE_EXPORT_BOX_H
