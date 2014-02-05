#ifndef TRACKHEADERBOXTEST_H
#define TRACKHEADERBOXTEST_H

#include "boxTestsCommon.h"

class TrackHeaderBoxTest : public QObject
{
    Q_OBJECT
public:
    explicit TrackHeaderBoxTest();
    
private Q_SLOTS:
    void readingTest_data();
    void readingTest();
};

#endif // TRACKHEADERBOXTEST_H
