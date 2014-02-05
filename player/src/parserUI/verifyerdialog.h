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

#ifndef VERIFYERDIALOG_H
#define VERIFYERDIALOG_H

#include "crosscompilation_cxx11.h"

#include <QDialog>
#include <QTreeWidgetItem>
#include "mediaParser.h"
#include "oxfverifier.h"

//  reference to Qt UI
namespace Ui {
    class verifyerDialog;
}

//! Structure that contains all that needed for verification.
struct VerifyingStructure
{
    const SigningInformation*   m_ref_inf;
    QString                     m_file_name;
    QTreeWidgetItem*            m_tree_item;

    VerifyingStructure(const SigningInformation* ref_inf,
                       const QString& file_name,
                       QTreeWidgetItem* tree_item) :
        m_ref_inf(ref_inf),
        m_file_name(file_name),
        m_tree_item(tree_item)
    {}
};

//! this class implements user interface for file signature verification.
class VerifyerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit VerifyerDialog(QWidget *parent = 0);
    ~VerifyerDialog();

    //!  clear current content of the dialog and reset internal data
    void  clearContent(void);

    //! prepare for verification
    void  initialize(MediaParser& parser);

public slots:
    //! handle the user request to cancel operation
    void onCancelClicked(void);

    //!  handle the certificate operation request
    void onCertificateActionClicked(void);

    //! perform verification for one certificate only
    void onCertificateVerify(void);

    //!  handle the overall start button
    void onVerifyAllClicked(void);

    //! handle selection in signatures list
    void onSignatureItemClicked(QTreeWidgetItem * item, int column);

    //! Operation progress slots.
    void onOperationStarted(int);
    void onOperationCompleted(VerificationStatus );
    //void onOperationRunning(bool* isCanceled);
    void onOperationRunning(void);
    
private:
    //! UI
    Ui::verifyerDialog*         m_ui;

    //!  verifier object
    OXFVerifier*                m_verifier;

    //! Currently verifying items
    QList<VerifyingStructure>   m_verifying_items;

    //! Currently selected tree item
    QTreeWidgetItem*            m_selected_tree_item;

    //!  Should we cancel current operation ?
    bool                        m_cancel_operation;

    //! Is initialized.
    bool                        m_already_initialized;
};

#endif // VERIFYERDIALOG_H
