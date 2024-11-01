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

#include "parserWidget.h"
#include "ui_parserWidget.h"

#include <QCommonStyle>

#include "parser/helpers/optional.hpp"
#include "parser/basic/mandatoryBox.h"
#include "parser/basic/unknownBox.h"

ParserWidget::ParserWidget(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::parserWidget)
{
    m_ui->setupUi(this);
    m_ui->boxTree->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_ui->boxTree, &QTreeWidget::currentItemChanged, this, &ParserWidget::onItemChanged);
    connect(m_ui->expandBoxes, &QPushButton::clicked, [this] () {
        this->m_ui->boxTree->expandAll();
        this->resizeTreeToContents(m_ui->boxTree);
    });
    connect(m_ui->collapseBoxes, &QPushButton::clicked, [this] () {
        this->m_ui->boxTree->collapseAll();
        this->resizeTreeToContents(m_ui->boxTree);
    });
    connect(m_ui->expandProperties, &QPushButton::clicked, [this] () {
        this->m_ui->propertyTree->expandAll();
        this->resizeTreeToContents(m_ui->propertyTree);
    });
    connect(m_ui->collapseProperties, &QPushButton::clicked, [this] () {
        this->m_ui->propertyTree->collapseAll();
        this->resizeTreeToContents(m_ui->propertyTree);
    });

    Qt::WindowFlags flags = this->windowFlags();
    flags &= ~(Qt::WindowContextHelpButtonHint);
    flags |= Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);
}

ParserWidget::~ParserWidget()
{
    delete m_ui;
}

void ParserWidget::showFilesetInformation(FilesetInformation fileset_information)
{
    if(fileset_information == m_fileset_information)
    {
        return;
    }
    else
    {
        clearContents();

        m_fileset_information = fileset_information;

        Q_FOREACH(QString filename, m_fileset_information.keys())
        {
            QTreeWidgetItem * item = new QTreeWidgetItem();
            item->setText(0, QFileInfo(filename).fileName());
            item->setText(1, "File");
            Q_FOREACH(Box * child, m_fileset_information[filename]->getChildren())
            {
                createBoxItem(child, item);
            }
            m_ui->boxTree->addTopLevelItem(item);
        }
        resizeTreeToContents(m_ui->boxTree);
        if (fileset_information.size() == 1) {
            m_ui->boxTree->expandToDepth(2);
        }
    }
}

void ParserWidget::clearContents()
{
    m_fileset_information.clear();
    m_ui->boxTree->clear();
    m_ui->propertyTree->clear();
}

QTreeWidgetItem * ParserWidget::createBoxItem(Box * box, QTreeWidgetItem * parent)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(parent);
    item->setData(0, Qt::UserRole, QVariant::fromValue((void*)box));
    item->setText(0, (QString)box->getBoxFourCC());
    item->setText(1, box->getBoxDescription());

    QString errorTooltip = generateErrorList(box).join("\r\n").trimmed();

    if(!errorTooltip.isEmpty())
    {
        item->setIcon(0, QCommonStyle().standardIcon(QStyle::SP_MessageBoxWarning));
        item->setToolTip(0, errorTooltip);
    }

    ChildrenMixin * super_box = dynamic_cast<ChildrenMixin *>(box);
    if(super_box != nullptr)
    {
		auto super_box_children = super_box->getChildren();
        for(auto it = super_box_children.begin(), end = super_box_children.end(); it != end; ++it)
        {
			Box * child = *it;
            createBoxItem(child, item);
        }
    }
    return item;
}

QStringList ParserWidget::generateErrorList(Box *box)
{
    QStringList errorList;
    switch(box->getSizeError())
    {
    case Box::SizeInsufficient:
        errorList << "Actual box size is less, than expected. Data may be corrupted.";
        break;
    case Box::SizeExceeding:
        errorList << "Actual box size is more, than expected. Data may be corrupted.";
        break;
    default:
        break;
    }

    Box::ConsistencyError consistency_error = box->getConsistencyError();

    if(consistency_error & Box::HasNotEnoughBoxes)
    {
        errorList << "Box does not contain all mandatory boxes needed.";
    }
    if(consistency_error & Box::HasTooManyBoxes)
    {
        errorList << "Box contains exceeding count of child boxes of specific type.";
    }
    if(consistency_error & Box::HasConflictingBoxes)
    {
        errorList << "Box contains conflicting types of child boxes.";
    }
    if(consistency_error & Box::HasUnexpectedBoxes)
    {
        errorList << "Box contains unexpected boxes.";
    }
    if(consistency_error & Box::IsMandatoryBox)
    {
        if(is_a<MandatoryBox>(box))
        {
            errorList << "Parent box should contain at least one box of type '" + (QString)box->getBoxFourCC() + "'.";
        }
        else
        {
            QString errorString;
            MandatoryBoxes * mandatory_boxes = dynamic_cast<MandatoryBoxes *>(box);
            errorString = "Parent box should contain at least one box of one of the following types: ";
            auto mandatory_four_cc = mandatory_boxes->getMandatoryFourCC();
            for( auto it = mandatory_four_cc.begin(), end = mandatory_four_cc.end(); it != end; ++it )
            {
                FourCC & mandatory_four_cc = *it;
                errorString += "'" + (QString)mandatory_four_cc + "', ";
            }
            if(errorString.endsWith(", "))
            {
                errorString.replace(errorString.length() - 3, 2, ".");
            }
            errorList << errorString;
        }
    }
    if(consistency_error & Box::IsBoxWithExceedingCount)
    {
        errorList << "Parent box contains too many boxes of this type.";
    }
    if(consistency_error & Box::IsConflictingBox)
    {
        errorList << "Parent box contains conflicting types of child box.";
    }
    if(consistency_error & Box::IsUnexpectedBox)
    {
        if(is_a<UnknownBox>(box))
        {
            errorList << "Box type is unknown.";

        }
        errorList << "Box presence is not expected here.";
    }
    return errorList;
}

void ParserWidget::resizeTreeToContents(QTreeWidget *widget)
{
    if(widget != nullptr)
    {
        for(int i = 0, size = widget->columnCount(); i < size; ++i)
        {
            widget->resizeColumnToContents(i);
        }
    }
}

void ParserWidget::onItemChanged(QTreeWidgetItem *item)
{
    m_ui->propertyTree->clear();

    if((item != nullptr) && (item->data(0, Qt::UserRole).canConvert<void *>()))
    {
        Box * box = (Box*)item->data(0, Qt::UserRole).value<void *>();
        QList<QString> properties = box->getProperties();

        Q_FOREACH(QString errorString, generateErrorList(box))
        {
            QTreeWidgetItem * item = new QTreeWidgetItem();
            item->setText(0, errorString);
            item->setFirstColumnSpanned(true);
            item->setIcon(0, QCommonStyle().standardIcon(QStyle::SP_MessageBoxWarning));
            m_ui->propertyTree->addTopLevelItem(item);
        }

        for(auto it = properties.begin(), end = properties.end(); it != end; ++it)
        {
			QString & property = *it;
            m_ui->propertyTree->addTopLevelItem(createPropertyItem(property, box->getProperty(property)));
        }

        resizeTreeToContents(m_ui->propertyTree);
    }
}

QTreeWidgetItem * ParserWidget::createPropertyItem(QString name, const Property & value, QTreeWidgetItem *parent)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    QString string = value.getString();
    item->setText(1, string.isEmpty() ? QString("<Empty>") : string);
    if(value.isList())
    {
        int idx = 0;
        for(auto it = value.begin(), end = value.end(); it != end; ++it)
        {
			const Property & entry = *it;
            createPropertyItem( QString::number(idx++), entry, item );
        }
    }
    return item;
}
