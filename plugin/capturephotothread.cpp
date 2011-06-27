#include "capturephotothread.h"

CapturePhotoThread::CapturePhotoThread(QObject *parent) :
    QThread(parent)
{
}

void CapturePhotoThread::takePhoto(const QString &path, QCameraImageCapture *img)
{
    m_path = path;
    imgCapture = img;
    waitCondition.wakeAll();
}

void CapturePhotoThread::run()
{
    forever {
        mutex.lock();
        waitCondition.wait(&mutex);
        imgCapture->capture(m_path);
        mutex.unlock();
    }
}
