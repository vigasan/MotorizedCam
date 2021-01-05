#ifndef FILTER_H
#define FILTER_H

#include <QAbstractVideoFilter>
#include "opencvhelper.h"

#define STATUS_TRACKER_OFF      0
#define STATUS_TRACKER_START    1
#define STATUS_TRACKER_ON       2

class Filter : public QAbstractVideoFilter
{
    Q_OBJECT
public:
    QVideoFilterRunnable *createFilterRunnable() Q_DECL_OVERRIDE;

signals:
    void finished(QObject *e);

private:
    friend class FilterRunnable;
};

class FilterRunnable : public QVideoFilterRunnable
{
public:
    FilterRunnable(Filter *filter);
    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) Q_DECL_OVERRIDE;

private:
    void processFaceClassifier(Mat frame);

    Filter *m_filter;
    Mat m_mat;
    bool m_yuv;
    CascadeClassifier *m_classifier;
    QString m_prevName;

};

class FilterResult : public QObject
{
    Q_OBJECT

public slots:
    QVariantList rects() const { return m_rects; }

private:
    QVariantList m_rects;
    friend class FilterRunnable;
};

#endif
