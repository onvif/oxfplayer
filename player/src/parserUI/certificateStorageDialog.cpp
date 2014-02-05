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

#include "certificateStorageDialog.h"
#include "ui_certificateStorageDialog.h"

#include <QDir>
#include <QFileIconProvider>
#include <QFileDialog>
#include <QSettings>

#include "defines.h"

CertificateStorageDialog::CertificateStorageDialog(QWidget* parent) :
    QDialog(parent),
    m_ui(new Ui::CertificateStorageDialog)
{
    m_ui->setupUi(this);

    Qt::WindowFlags flags = this->windowFlags();
    flags &= ~(Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    //list files and fill tree
    fillFilesList();

    QObject::connect(m_ui->add, SIGNAL(clicked()), this, SLOT(onAdd()));
    QObject::connect(m_ui->remove, SIGNAL(clicked()), this, SLOT(onRemove()));
}

CertificateStorageDialog::~CertificateStorageDialog()
{
    delete m_ui;
}

void CertificateStorageDialog::fillFilesList()
{
    m_ui->files_tree->clear();
    m_storage.update();
    const QFileInfoList& files = m_storage.getFilesList();
    for(QFileInfoList::const_iterator cIter = files.constBegin(); cIter != files.constEnd(); ++cIter)
    {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(cIter->fileName());
        item->setIcon(QFileIconProvider().icon(*cIter));
        m_ui->files_tree->addItem(item);
    }
}

void CertificateStorageDialog::onAdd()
{
#ifdef WIN32
    QSettings settings(QDir::homePath() + WINP_APP_DATA_ROAMING + COMPANY_NAME + "/" + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat);
#endif //WIN32
#ifdef UNIX
    QSettings settings(QDir::homePath() + "/." + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat);
#endif //UNIX

    QString file_name = QFileDialog::getOpenFileName(this, tr("Add certificate file"), settings.value("lastOpenedCertificateFolder", QDir::homePath()).toString(), BINARY_FORMAT);
    if(file_name.isEmpty())
        return;

    settings.setValue("lastOpenedCertificateFolder", QFileInfo(file_name).absolutePath());

    if(QFile::copy(file_name, CertificateStorage::getCertificateFolder() + "/" + QFileInfo(file_name).fileName()))
        fillFilesList();
}

void CertificateStorageDialog::onRemove()
{
    QList<QListWidgetItem*> selected_items = m_ui->files_tree->selectedItems();
    if(selected_items.empty())
        return;
    int row = m_ui->files_tree->row(selected_items[0]);
    delete m_ui->files_tree->takeItem(row);
    m_storage.removeCertificate(row);
    fillFilesList();
}
