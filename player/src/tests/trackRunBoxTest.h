#ifndef TRACKRUNBOXTEST_H
#define TRACKRUNBOXTEST_H

#include "boxTestsCommon.h"

class TrackRunBoxTest : public QObject
{
    Q_OBJECT
public:
    explicit TrackRunBoxTest();
    
private Q_SLOTS:
    void readingTest_data();
    void readingTest();
};

#endif // TRACKRUNBOXTEST_H
