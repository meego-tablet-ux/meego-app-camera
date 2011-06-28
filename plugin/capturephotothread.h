#ifndef CAPTUREPHOTOTHREAD_H
#define CAPTUREPHOTOTHREAD_H

#include <QThread>
#include <QCameraImageCapture>
#include <QMutex>
#include <QWaitCondition>

#include <QtLocation/QGeoPositionInfoSource>
#include <QtLocation/QGeoPositionInfo>

QTM_USE_NAMESPACE

class CapturePhotoThread : public QThread
{
    Q_OBJECT
public:
    explicit CapturePhotoThread(QObject *parent = 0);
    void takePhoto(const QString& path, QCameraImageCapture* img);


protected:
    void run();

signals:
    void lastCoordinate(QGeoCoordinate);

public slots:

private:
    QString m_path;
    QCameraImageCapture *imgCapture;
    QMutex mutex;
    QWaitCondition waitCondition;

    QGeoPositionInfoSource *_positionSource;
    QGeoPositionInfo _lastPosition;
};

#endif // CAPTUREPHOTOTHREAD_H
