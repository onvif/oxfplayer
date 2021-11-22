/************************************************************************************
* Copyright (c) 2021 ONVIF.
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

#include "queuedMetadataDecoder.h"

#include "ffmpeg.h"
#include "avFrameWrapper.h"
#include <string>
#include <sstream>
#include <unordered_map>

#include "../../ext/pugixml/src/pugixml.hpp"

#include <QDebug>
#include <qpainter.h>

static bool hasLocalname(pugi::xml_node node, const char* name) {
    const char* n = node.name();
    for (int i = 0; i < 6 && n[i]; i++) {
        if (n[i] == ':') {
            n += i + 1;
            break;
        }
    }
    return strcmp(n, name) == 0;
}

static pugi::xml_node read(pugi::xml_node& node, const char* localname) {
    if (!hasLocalname(node, localname)) return pugi::xml_node();
    pugi::xml_node ret = node;
    node = node.next_sibling();
    return ret;
}
/**
 * Recursive method for building widget tree from XML.
 * Special handling for simple and element item to nicely display.
 */
static void addChildren(pugi::xml_node& node, QTreeWidgetItem *parent)
{
    for (auto attr = node.first_attribute(); attr; attr = attr.next_attribute()) {
        auto item = new QTreeWidgetItem(parent);
        item->setText(0, attr.name());
        item->setText(1, attr.value());
    }
    for (auto child = node.first_child(); child; child = child.next_sibling()) {
        const char* name = strchr(child.name(), ':');
        name = name ? name + 1 : child.name();          // skip prefix if present
        if (!strcmp(name, "SimpleItem")) {
            auto item = new QTreeWidgetItem(parent);
            for (auto attr = child.first_attribute(); attr; attr = attr.next_attribute()) {
                item->setText(attr.name()[0] == 'V' ? 1 : 0, attr.value());
            }
        }
        else if (!strcmp(name, "ElementItem")) {
            addChildren(child, parent);
        }
        else {
            auto item = new QTreeWidgetItem(parent);
            item->setText(0, name);
            if (child.first_child()) addChildren(child, item);
            item->setText(1, child.value());
        }
    }
}

void MetadataDecoder::processPacket(AVPacket* packet, int* readed_frames)
{
    //create frame
    VideoFrame video_frame;
    video_frame.calcTime(packet->pts, m_stream->time_base);

    video_frame.m_isOverlay = true;
    if (m_decoder) {
        m_frame_height = m_decoder->frameHeight();
        m_frame_width = m_decoder->frameWidth();
        QImage image = parseMetadata(packet->buf->data, packet->buf->size, packet->pts);
        video_frame.m_image = image;
        m_queue.push(video_frame);
        (*readed_frames)++;
    }
}

QImage MetadataDecoder::parseMetadata(const unsigned char* buffer, size_t bytes, int time)
{
    QImage image(m_frame_width, m_frame_height, QImage::Format_RGB32);
    image.fill(Qt::black);
    QPainter painter(&image);
    QPen redpen(Qt::red, 5
    );
    painter.setPen(redpen);
    double fx = m_frame_width / 2.0;
    double fy = m_frame_height / 2.0;

    pugi::xml_document doc;
    if (doc.load_buffer(buffer, bytes, pugi::encoding_utf8).status == 0) {
        for (pugi::xml_node n = doc.first_child().first_child(); n; n = n.next_sibling()) {
            if (hasLocalname(n, "VideoAnalytics")) {
                for (pugi::xml_node f = n.first_child(); f; f = f.next_sibling()) {
                    if (hasLocalname(f, "Frame")) {
                        auto fparam = f.first_child();
                        auto ptzStatus = read(fparam, "PTZStatus");
                        auto transform = read(fparam, "Transformation");
                        while (auto obj = read(fparam, "Object")) {
                            auto appearance = obj.first_child().first_child();
                            auto trans2 = read(appearance, "Transformation");
                            auto shape = read(appearance, "Shape").first_child();
                            auto bounds = read(shape, "BoundingBox");
                            double top = 0, left = 0, right = 0, bottom = 0;
                            for (auto attr = bounds.attributes_begin(); attr != bounds.attributes_end(); attr++) {
                                switch (attr->name()[0]) {
                                case 't': top = (-strtod(attr->value(), 0) + 1.0) * fy; break;
                                case 'l': left = (strtod(attr->value(), 0) + 1.0) * fx; break;
                                case 'r': right = (strtod(attr->value(), 0) + 1.0) * fx; break;
                                case 'b': bottom = (-strtod(attr->value(), 0) + 1.0) * fy; break;
                                }
                            }
                            if (bounds && right > left) {
                                painter.drawRect(left, top, right - left, bottom - top);
                            }
                        }
                    }
                }
            }
            else if (hasLocalname(n, "Event")) {
                for (pugi::xml_node m = n.first_child(); m; m = m.next_sibling()) {
                    if (hasLocalname(m, "NotificationMessage")) {
                        auto mc = m.first_child();
                        auto ref = read(mc, "SubscriptionReference");
                        auto topic = read(mc, "Topic");
                        auto msg = read(mc, "Message");
                        if (msg && topic) {
                            std::stringstream ss;
                            m.print(ss);
                            std::hash<std::string> hasher;
                            auto test = ss.str();
                            EventInfo ei(time);
                            ei.item = new EventItem(time, hasher(ss.str()));
                            ei.item->setText(0, "Event");
                            ei.item->setText(1, topic.first_child().value());
                            addChildren(msg.first_child(), ei.item);
                            m_eventQueue.push(ei);
                        }
                    }
                }
            }
        }
    }
    painter.end();
    return image;
}
