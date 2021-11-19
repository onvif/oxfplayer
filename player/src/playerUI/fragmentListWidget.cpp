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

#include "fragmentListWidget.h"

FragmentListWidget::FragmentListWidget(QWidget* parent) :
    QListWidget(parent)
{
    setFlow(QListWidget::LeftToRight);
    setMovement(QListWidget::Static);
    setResizeMode(QListWidget::Adjust);
    setViewMode(QListWidget::IconMode);
    setSpacing(15);
    setIconSize(QSize(100, 100));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

FragmentListWidget::~FragmentListWidget()
{
    clear();
}

void FragmentListWidget::setFragmentsList(const SegmentList& fragments)
{
    clear();
    m_fragments = fragments;
    for(SegmentList::const_iterator cIter = m_fragments.constBegin(); cIter != m_fragments.constEnd(); ++cIter)
    {
        QListWidgetItem* item = new QListWidgetItem(QIcon(":/frame"), cIter->getName(), this);
        item->setData(Qt::UserRole, cIter - fragments.constBegin());
        addItem(item);
    }
    setCurrentRow(0);
    QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
}

void FragmentListWidget::clear()
{
    QObject::disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
    QListWidget::clear();
    m_fragments.clear();
}

void FragmentListWidget::selectFragment(const SegmentInfo& fragment_info)
{
    QObject::disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
    if(fragment_info.getFragmentNumber() < (uint32_t)m_fragments.size())
        setCurrentRow(fragment_info.getFragmentNumber());
    else
        setCurrentRow(0);
    QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
}

void FragmentListWidget::onSelectionChanged()
{
    QList<QListWidgetItem*> selected_items = selectedItems();
    if(selected_items.empty())
        return;
    QListWidgetItem* selected_item = selected_items.first();
    int index = selected_item->data(Qt::UserRole).toInt();
    if(m_fragments.size())
        emit fragmentSelected(m_fragments.at(index));
}
