#include "filter.h"
#include "rgbframehelper.h"
#include <QFile>
#include <QDebug>

QVideoFilterRunnable *Filter::createFilterRunnable()
{
    return new FilterRunnable(this);
}


//////////////////////////////////////////////////////////////////////////////////////////////////7///
FilterRunnable::FilterRunnable(Filter *filter) : m_filter(filter), m_classifier(nullptr)
{

}

QVideoFrame FilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags)
{
    Q_UNUSED(surfaceFormat)
    Q_UNUSED(flags)

    // Supports YUV (I420 and NV12) and RGB. The GL path is readback-based and slow.
    if (!input->isValid() || (input->handleType() != QAbstractVideoBuffer::NoHandle && input->handleType() != QAbstractVideoBuffer::GLTextureHandle))
    {
        qWarning("Invalid input format");
        return QVideoFrame();
    }

    input->map(QAbstractVideoBuffer::ReadOnly);
    if (input->pixelFormat() == QVideoFrame::Format_YUV420P || input->pixelFormat() == QVideoFrame::Format_NV12)
    {
        m_yuv = true;
        m_mat = yuvFrameToMat8(*input);
    } else
    {
        m_yuv = false;
        QImage wrapper = imageWrapper(*input);
        if (wrapper.isNull()) {
            if (input->handleType() == QAbstractVideoBuffer::NoHandle)
                input->unmap();
            return *input;
        }
        m_mat = imageToMat8(wrapper);
    }
    ensureC3(&m_mat);
    if (input->handleType() == QAbstractVideoBuffer::NoHandle)
        input->unmap();


    processFaceClassifier(m_mat);

    return QVideoFrame(mat8ToImage(m_mat));
}

void FilterRunnable::processFaceClassifier(Mat frame)
{
    // 1. Convert to grayscale
    cvtColor(frame, frame, CV_BGR2GRAY);

    // 2. Flip if requested
    if (m_filter->property("flip").toBool())
        cv::flip(frame, frame, -1);

    // 3. Detect
    FilterResult *r = new FilterResult;

    QByteArray filename = m_filter->property("classifier").toString().toUtf8();
    if (filename != m_prevName)
    {
        delete m_classifier;
        m_classifier = nullptr;
        m_prevName = filename;
    }

    if(!filename.isEmpty())
    {
        if (!m_classifier)
        {
            if (!QFile::exists(filename))
                qWarning("Classifier does not exist: %s", qPrintable(filename));
            m_classifier = new cv::CascadeClassifier(filename.constData());
        }

        std::vector<cv::Rect> rects;
        float scaleFactor = qBound(1.1f, m_filter->property("scaleFactor").toFloat(), 5.0f);
        int minNeighbours = qBound(1, m_filter->property("minNeighbours").toInt(), 20);
        m_classifier->detectMultiScale(frame, rects, double(scaleFactor), minNeighbours, cv::CASCADE_SCALE_IMAGE);

        for (size_t i = 0; i < rects.size(); ++i)
            r->m_rects.append(QRect(rects[i].x, rects[i].y, rects[i].width, rects[i].height));
    }

    emit m_filter->finished(r);
}



