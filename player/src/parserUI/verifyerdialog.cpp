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

#include <QDebug>
#include <QSslCertificate>
#include <QSslKey>
#include <QFileDialog>

#include "verifyerdialog.h"
#include "ui_verifyerdialog.h"

#include "oxfverifier.h"
#include "ONVIFSignInfo.h"
#include "certificateStorage.h"

static const QString  strEmptyString("");

QRgb green  = qRgb(0,176,80);
QRgb red    = qRgb(255,80,80);
QRgb yellow = qRgb(255,255,103);

// ctor
VerifyerDialog::VerifyerDialog(QWidget *parent)
               : QDialog(parent),
                 m_ui(new Ui::verifyerDialog),
                 m_verifier(nullptr),
                 m_selected_tree_item(nullptr),
                 m_cancel_operation(false),
                 m_already_initialized(false)
{
    qRegisterMetaType<VerificationStatus>("VerificationStatus");

    m_ui->setupUi(this);
    m_verifier = new OXFVerifier(this);

    connect(m_verifier, SIGNAL(operationCompleted(VerificationStatus)), this, SLOT(onOperationCompleted(VerificationStatus)));
    connect(m_verifier, SIGNAL(operationStarted(int)),                  this, SLOT(onOperationStarted(int)));
    connect(m_verifier, SIGNAL(operationRunning()),                     this, SLOT(onOperationRunning()));

    // connect buttons
    connect(m_ui->btnCancel,    SIGNAL(clicked()), this, SLOT(onCancelClicked()));
    connect(m_ui->btnVerifyAll, SIGNAL(clicked()), this, SLOT(onVerifyAllClicked()));
    connect(m_ui->btnAction,    SIGNAL(clicked()), this, SLOT(onCertificateActionClicked()));
    connect(m_ui->btnVerify,    SIGNAL(clicked()), this, SLOT(onCertificateVerify()));

    connect(m_ui->listSignatures, SIGNAL(itemClicked(QTreeWidgetItem* , int )), this, SLOT(onSignatureItemClicked(QTreeWidgetItem* , int )));

    // set controls into the default state
    clearContent();

    Qt::WindowFlags flags = this->windowFlags();
    flags &= ~(Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);
}

//dtor
VerifyerDialog::~VerifyerDialog()
{
    delete m_verifier;
    delete m_ui;
}

// clear all controls and set them into the default state
void  VerifyerDialog::clearContent(void)
{
    // hide progress
    m_ui->verificationProgress->reset();
    m_ui->verificationProgress->hide();

    // status (local and overall
    //m_ui->txtLocalStatus->setText(strEmptyString);
    m_ui->txtStatus->setText(strEmptyString);

    // hide cancel button
    m_ui->btnCancel->hide();

    // before initalization lety hide overall and signature specefic start aas well
    m_ui->listSignatures->clear();
    m_ui->btnVerify->setEnabled(false);
    m_ui->btnVerifyAll->setEnabled(false);
    m_ui->btnAction->setEnabled(false);
    m_already_initialized = false;

    m_verifying_items.clear();
    m_selected_tree_item = nullptr;
}

// get information required for starting verification, that is file set and signature map.
void  VerifyerDialog::initialize(MediaParser& parser)
{
    if (m_already_initialized)
        return;

    m_already_initialized = true;

    // prepare tree heades
    QStringList header_labels;
    header_labels << "File" << "Issuer" << "Signature" << "Certificate";
    m_ui->listSignatures->setHeaderLabels(header_labels);

    // flag that shows that there ara any signatures to verify
    bool signatures_found = false;

    // ok, let`s get fragments and correspoding signatures
    SigningInformationMap   siMap = parser.getSignaturesMap();
    if ( !(siMap.size() > 0) )
    {
        QString tmp = m_ui->txtStatus->text();
        tmp.append(tr(" This file set is not signed."));
        m_ui->txtStatus->setText(tmp);
        return;
    }

    FragmentsList           fgList = parser.getFragmentsList();

    for (int i = 0; i < fgList.count(); ++i)
    {
        QString fragmentFullName = fgList.at(i).getFileName();
        QString fragmentShortName = QFileInfo(fragmentFullName).fileName();

        QTreeWidgetItem * parent = new QTreeWidgetItem();
        parent->setText(0, fragmentShortName);
        parent->setData(0, Qt::UserRole, fragmentFullName);

        ONVIFSigningInformation inf = siMap[fragmentFullName];
        if (0 == inf.getSignCount() )  // no signature in current fragment
        {
            QTreeWidgetItem * item = new QTreeWidgetItem(parent);
            item->setText(0, tr("This file is not signed."));
            m_ui->listSignatures->addTopLevelItem(parent);
            continue;
        }

        for (int j = 0 ; j < inf.getSignCount(); ++j)
        {
            const SigningInformation&  refInf = inf.getSigningInformation(j);

            signatures_found = true;

            QTreeWidgetItem * item = new QTreeWidgetItem(parent);
            item->setData(0, Qt::UserRole, QVariant(&refInf));
            item->setText(0, QString("Signature %1").arg(j+1));
            {
                OXFVerifier ver;
                ver.initialize(&refInf, item->parent()->data(0, Qt::UserRole).toString());
                item->setText(1, ver.getCertificateIssuer());
            }
        }

        m_ui->listSignatures->addTopLevelItem(parent);
    } // end of for all fragments

    m_ui->listSignatures->expandAll();
    m_ui->listSignatures->resizeColumnToContents(0);
    m_ui->listSignatures->resizeColumnToContents(1);
    m_ui->listSignatures->setColumnWidth(0, m_ui->listSignatures->columnWidth(0) + 10);
    m_ui->listSignatures->setColumnWidth(1, m_ui->listSignatures->columnWidth(1) + 10);

    m_ui->btnVerifyAll->setEnabled(signatures_found);
    if(!signatures_found)
        m_ui->txtStatus->setText(tr("This file set is not valid OXF."));
}

void VerifyerDialog::onCancelClicked(void)
{
    m_cancel_operation = true;
    m_verifier->setCancelOperation(m_cancel_operation);
}

void VerifyerDialog::onSignatureItemClicked(QTreeWidgetItem * item, int column)
{
    Q_UNUSED(column);

    const SigningInformation*  refInf = reinterpret_cast<const SigningInformation*>(item->data(0, Qt::UserRole).value<void *>());
    if (refInf == nullptr)  // file name has been clicked
    {
        m_ui->btnAction->setEnabled(false);
        m_ui->btnVerify->setEnabled(false);
        m_ui->txtStatus->setText(strEmptyString);
        m_selected_tree_item = nullptr;
        return;
    }

    m_selected_tree_item = item;

    // init signature infoirmation
    m_ui->btnAction->setEnabled(true);
    m_ui->btnVerify->setEnabled(true);
    m_verifier->initialize(refInf, item->parent()->data(0, Qt::UserRole).toString());
}


void VerifyerDialog::onCertificateActionClicked(void)
{
    QString filter = BINARY_FORMAT;
    QString dir;
#ifdef WIN32
    dir = QDir::homePath() + WINP_APP_DATA_ROAMING + COMPANY_NAME + "/" + PRODUCT_NAME + "/" + CERTIFICATES_FOLDER + "/";
#endif //WIN32
#ifdef UNIX
    dir = QDir::homePath() + "/." + PRODUCT_NAME + "/" + CERTIFICATES_FOLDER + "/";
#endif //UNIX
    if(!QDir(dir).exists())
        QDir().mkpath(dir);
    QFileDialog save_file_dialog(this);
    save_file_dialog.setAcceptMode(QFileDialog::AcceptSave);
    save_file_dialog.setDirectory(dir);
    save_file_dialog.setOptions(QFileDialog::DontUseNativeDialog);
    save_file_dialog.setWindowTitle(tr("Save certificate to file..."));
    save_file_dialog.setNameFilter(QString(BINARY_FORMAT) + QString(";;") + QString(BASE64_FORMAT));
    save_file_dialog.selectFile(m_verifier->getCertificateIssuer());
    QString fileName;
    if(save_file_dialog.exec() == QDialog::Accepted &&
       save_file_dialog.selectedFiles().size())
    {
        fileName = save_file_dialog.selectedFiles()[0];
        filter = save_file_dialog.selectedNameFilter();
    }
    if (fileName.isEmpty())  // nothing selected
        return;

    QFileInfo  fi(fileName);
    if ( (fi.suffix()).isEmpty() )  // add selected filter
    {
        if(filter == BINARY_FORMAT)
            fileName.append(BINARY_EXT);
        else
            fileName.append(BASE64_EXT);
    }

    m_verifier->saveCertificateToFile(fileName);
}

// start verification for one signaure
void VerifyerDialog::onCertificateVerify(void)
{
    // set verifying item
    m_verifying_items.clear();
    QTreeWidgetItem* tree_item = m_ui->listSignatures->selectedItems().at(0);
    const SigningInformation* ref_inf = reinterpret_cast<const SigningInformation*>(tree_item->data(0, Qt::UserRole).value<void *>());
    if(ref_inf == nullptr)
        return;
    m_verifying_items.push_back(VerifyingStructure(ref_inf, tree_item->parent()->data(0, Qt::UserRole).toString(), tree_item));
    // set buttons state
    m_ui->btnVerify->hide();
    m_ui->btnVerifyAll->hide();
    m_ui->btnAction->setEnabled(false);
    m_ui->listSignatures->setEnabled(false);
    m_ui->btnCancel->show();
    m_cancel_operation = false;
    m_verifier->start();
}

void VerifyerDialog::onVerifyAllClicked(void)
{
    //set verifying items
    m_verifying_items.clear();
    for(int row = 0; row < m_ui->listSignatures->topLevelItemCount(); ++row)
    {
        QTreeWidgetItem* file_tree_item = m_ui->listSignatures->topLevelItem(row);
        for(int sub_row = 0; sub_row < file_tree_item->childCount(); ++sub_row)
        {
            QTreeWidgetItem* tree_item = file_tree_item->child(sub_row);
            const SigningInformation* ref_inf = reinterpret_cast<const SigningInformation*>(tree_item->data(0, Qt::UserRole).value<void *>());
            if(ref_inf == nullptr)
                continue;
            m_verifying_items.push_back(VerifyingStructure(ref_inf, file_tree_item->data(0, Qt::UserRole).toString(), tree_item));
        }
    }
    // set buttons state
    m_ui->btnVerify->hide();
    m_ui->btnVerifyAll->hide();
    m_ui->btnAction->setEnabled(false);
    m_ui->listSignatures->setEnabled(false);
    m_ui->btnCancel->show();
    m_cancel_operation = false;
    //start first
    VerifyingStructure ver_struct = m_verifying_items.first();
    m_verifier->initialize(ver_struct.m_ref_inf, ver_struct.m_file_name);
    m_verifier->start();
}

/*  handle signal from a verification thread about begining of operation
*/
void VerifyerDialog::onOperationStarted(int max)
{
    qDebug() << "starts with " << max << " steps";
    m_ui->verificationProgress->setRange(0, max);
    m_ui->verificationProgress->setValue(0);
    m_ui->verificationProgress->show();
    m_ui->txtStatus->clear();
}

void VerifyerDialog::onOperationCompleted(VerificationStatus result)
{
    //operation canceled
    if(result == vsCanceled ||
       result == vsNA)
    {
        m_verifying_items.clear();

        // cancel button is not needed anymore
        m_ui->btnCancel->hide();
        // restore verify button
        m_ui->btnVerify->show();
        m_ui->btnVerifyAll->show();
        m_ui->btnAction->setEnabled(true);
        // restore tree
        m_ui->listSignatures->setEnabled(true);
        //  hide progress
        m_ui->verificationProgress->setValue(m_ui->verificationProgress->maximum());
        m_ui->verificationProgress->hide();

        if(result == vsCanceled)
            m_ui->txtStatus->setText(tr("Operation is canceled by user"));
        else
            m_ui->txtStatus->setText(strEmptyString);

        return;
    }

    VerifyingStructure ver_struct = m_verifying_items.takeFirst();

    // signature
    QString signature_text;
    QColor signature_color = Qt::white;
    switch (result)
    {
        case vsOK:
            signature_text = tr("Verified");
            signature_color.setRgb(green);
            break;
        case vsCanceled:
            break;
        case vsFailed:
            signature_text = tr("Not verified");
            signature_color.setRgb(red);
            break;
        case vsNA:
            m_ui->txtStatus->setText(strEmptyString);
            break;
    }

    ver_struct.m_tree_item->setText(2, signature_text);
    ver_struct.m_tree_item->setBackground(2, signature_color);
    ver_struct.m_tree_item->setTextAlignment(2, Qt::AlignVCenter | Qt::AlignHCenter);

    // certificate
    bool certificate_known = CertificateStorage().isCertificateKnown(m_verifier->getBinaryCertificate());
    ver_struct.m_tree_item->setText(3, certificate_known ? tr("Known") : tr("Unknown"));
    ver_struct.m_tree_item->setBackground(3, certificate_known ? QColor(green) : QColor(yellow));
    ver_struct.m_tree_item->setTextAlignment(3, Qt::AlignVCenter | Qt::AlignHCenter);

    if(m_verifying_items.empty())
    {
        //restore buttons
        // cancel button is not needed anymore
        m_ui->btnCancel->hide();
        // restore verify button
        m_ui->btnVerify->show();
        m_ui->btnVerifyAll->show();
        m_ui->btnAction->setEnabled(true);
        // restore tree
        m_ui->listSignatures->setEnabled(true);
        //  hide progress
        m_ui->verificationProgress->setValue(m_ui->verificationProgress->maximum());
        m_ui->verificationProgress->hide();

        //set result text
        switch (result)
        {
            case vsOK:
                m_ui->txtStatus->setText(tr("Verification completed"));
                break;
            case vsCanceled:
                break;
            case vsFailed:
                m_ui->txtStatus->setText(tr("Verification completed"));
                break;
            case vsNA:
                break;
        }

        //restore verifyer with selected item
        if(m_selected_tree_item)
            onSignatureItemClicked(m_selected_tree_item, 0);
    }
    else
    {
        //start next
        VerifyingStructure ver_struct = m_verifying_items.first();
        m_verifier->initialize(ver_struct.m_ref_inf, ver_struct.m_file_name);
        m_verifier->start();
    }
}

//void VerifyerDialog::onOperationRunning(bool* isCanceled)
void VerifyerDialog::onOperationRunning(void)
{
    if (m_ui->verificationProgress->maximum() == 0)
    {
        m_ui->verificationProgress->show();
    }
    else
    {
        int val = m_ui->verificationProgress->value();
        if ( val < m_ui->verificationProgress->maximum())
            m_ui->verificationProgress->setValue(val + 1);
    }
    //*isCanceled = m_cancel_operation;
}
