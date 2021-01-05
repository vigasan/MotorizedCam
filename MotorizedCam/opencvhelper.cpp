/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "opencvhelper.h"

cv::Mat imageToMat8(const QImage &image)
{
    QImage img = image.convertToFormat(QImage::Format_RGB32).rgbSwapped();
    cv::Mat tmp(img.height(), img.width(), CV_8UC4, (void *) img.bits(), img.bytesPerLine());
    return tmp.clone();
}

void ensureC3(cv::Mat *mat)
{
    Q_ASSERT(mat->type() == CV_8UC3 || mat->type() == CV_8UC4);
    if (mat->type() != CV_8UC3) {
        cv::Mat tmp;
        cvtColor(*mat, tmp, CV_BGRA2BGR);
        *mat = tmp;
    }
}

QImage mat8ToImage(const cv::Mat &mat)
{
    switch (mat.type()) {
    case CV_8UC1:
    {
        QVector<QRgb> ct;
        for (int i = 0; i < 256; ++i)
            ct.append(qRgb(i, i, i));
        QImage result(mat.data, mat.cols, mat.rows, int(mat.step), QImage::Format_Indexed8);
        result.setColorTable(ct);
        return result.copy();
    }
    case CV_8UC3:
    {
        cv::Mat tmp;
        cvtColor(mat, tmp, CV_BGR2BGRA);
        return mat8ToImage(tmp);
    }
    case CV_8UC4:
    {
        QImage result(mat.data, mat.cols, mat.rows, int(mat.step), QImage::Format_RGB32);
        return result.rgbSwapped();
    }
    default:
        qWarning("Unhandled Mat format %d", mat.type());
        return QImage();
    }
}

cv::Mat yuvFrameToMat8(const QVideoFrame &frame)
{
    Q_ASSERT(frame.handleType() == QAbstractVideoBuffer::NoHandle && frame.isReadable());
    Q_ASSERT(frame.pixelFormat() == QVideoFrame::Format_YUV420P || frame.pixelFormat() == QVideoFrame::Format_NV12);

    cv::Mat tmp(frame.height() + frame.height() / 2, frame.width(), CV_8UC1, (uchar *) frame.bits());
    cv::Mat result(frame.height(), frame.width(), CV_8UC3);
    cvtColor(tmp, result, frame.pixelFormat() == QVideoFrame::Format_YUV420P ? CV_YUV2BGR_YV12 : CV_YUV2BGR_NV12);
    return result;
}

class YUVBuffer : public QAbstractVideoBuffer
{
public:
    YUVBuffer(cv::Mat *mat) : QAbstractVideoBuffer(NoHandle), m_mode(NotMapped) {
        m_yuvMat.reset(mat);
    }
    MapMode mapMode() const Q_DECL_OVERRIDE { return m_mode; }
    uchar *map(MapMode mode, int *numBytes, int *bytesPerLine) Q_DECL_OVERRIDE {
        if (mode != NotMapped && m_mode == NotMapped) {
            if (numBytes)
                *numBytes = m_yuvMat->rows * m_yuvMat->cols;
            if (bytesPerLine)
                *bytesPerLine = m_yuvMat->cols;
            m_mode = mode;
            return m_yuvMat->data;
        }
        return 0;
    }
    void unmap() Q_DECL_OVERRIDE { m_mode = NotMapped; }
    QVariant handle() const Q_DECL_OVERRIDE { return 0; }

private:
    MapMode m_mode;
    QScopedPointer<cv::Mat> m_yuvMat;
};

QVideoFrame mat8ToYuvFrame(const cv::Mat &mat)
{
    Q_ASSERT(mat.type() == CV_8UC3 || mat.type() == CV_8UC4);

    cv::Mat *m = new cv::Mat;
    cvtColor(mat, *m, mat.type() == CV_8UC4 ? CV_BGRA2YUV_YV12 : CV_BGR2YUV_YV12);
    return QVideoFrame(new YUVBuffer(m), QSize(mat.cols, mat.rows), QVideoFrame::Format_YUV420P);
}

void mat8ToYuvFrame(const cv::Mat &mat, uchar *dst)
{
    cv::Mat m(mat.rows + mat.rows / 2, mat.cols, CV_8UC1, dst);
    cvtColor(mat, m, mat.type() == CV_8UC4 ? CV_BGRA2YUV_YV12 : CV_BGR2YUV_YV12);
}

cv::Mat showHistogram(cv::Mat& img)
{
    vector<Mat> bgr_planes;
    split(img, bgr_planes );

    /// Establish the number of bins
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 } ;
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;
    Mat b_hist, g_hist, r_hist;

    /// Compute the histograms:
    calcHist( &bgr_planes[0], 1, nullptr, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, nullptr, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, nullptr, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

    // Draw the histograms for B, G and R
    //int hist_w = 512; int hist_h = 400;
    int hist_w = 300; int hist_h = 180;
    int bin_w = cvRound(double(hist_w / histSize));

    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

     /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {
        line(histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                      Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                      Scalar( 255, 0, 0), 1, 8, 0  );
        line(histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                      Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                      Scalar( 0, 255, 0), 1, 8, 0  );
        line(histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                      Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                      Scalar( 0, 0, 255), 1, 8, 0  );
    }

    return histImage;

}
