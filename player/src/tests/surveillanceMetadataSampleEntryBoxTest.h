#ifndef SURVEILLANCEMETADATASAMPLEENTRYBOX_H
#define SURVEILLANCEMETADATASAMPLEENTRYBOX_H

#include "boxTestsCommon.h"

class SurveillanceMetadataSampleEntryBoxTest : public QObject
{
    Q_OBJECT
public:
    explicit SurveillanceMetadataSampleEntryBoxTest();
    
private Q_SLOTS:
    void readingTest_data();
    void readingTest();
};

#endif // SURVEILLANCEMETADATASAMPLEENTRYBOX_H
