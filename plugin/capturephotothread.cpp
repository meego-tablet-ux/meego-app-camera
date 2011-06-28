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
     _positionSource = QGeoPositionInfoSource::createDefaultSource (0);//this);
     if (_positionSource)
       _positionSource->startUpdates ();

    forever {
        mutex.lock();
        waitCondition.wait(&mutex);

        if (_positionSource) {
          _lastPosition = _positionSource->lastKnownPosition();
      #ifdef SHOW_DEBUG
            qDebug () << "last position: " << _lastPosition;
      #endif
            emit lastCoordinate(_lastPosition.coordinate());
        }


        imgCapture->capture(m_path);
        mutex.unlock();
    }
}
