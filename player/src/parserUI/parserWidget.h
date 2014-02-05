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

#ifndef PARSERWIDGET_H
#define PARSERWIDGET_H

#include "crosscompilation_cxx11.h"

#include <QDialog>
#include <QTreeWidgetItem>
#include <QStringList>
#include <memory>

#include "../parser/mediaParser.h"

namespace Ui {
class parserWidget;
}

//! Dialog window showing the fileset information.
class ParserWidget : public QDialog
{
    Q_OBJECT
    
public:
    explicit ParserWidget(QWidget *parent = 0);
    ~ParserWidget();

public:
    //! Shows the fileset parsing tree.
    void showFilesetInformation(FilesetInformation fileset_information);
    //! Clears the fileset parsing tree.
    void clearContents();

public slots:
    //! This slot is called when the box is selected and we need to show its content.
    void onItemChanged(QTreeWidgetItem * item);
    //! This slot is called when some item is expanded or collapsed.
    void onExpandCollaps(const QTreeWidgetItem* item);

private:
    //! Creates a tree item for a box.
    QTreeWidgetItem * createBoxItem(Box *box, QTreeWidgetItem *parent = nullptr);
    //! Creates a tree item for a box property.
    QTreeWidgetItem * createPropertyItem(QString name, const Property & value, QTreeWidgetItem *parent = nullptr);
    //! Generates a list of errors for a box
    QStringList generateErrorList(Box *box);
    //! Resizes a specified tree widget columns to fit its content
    void resizeTreeToContents(QTreeWidget *widget);
    
private:
    //! UI
    Ui::parserWidget* m_ui;
    //! Fileset information.
    FilesetInformation m_fileset_information;
};

#endif // PARSERWIDGET_H
