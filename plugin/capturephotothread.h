#ifndef CAPTUREPHOTOTHREAD_H
#define CAPTUREPHOTOTHREAD_H

#include <QThread>
#include <QCameraImageCapture>
#include <QMutex>
#include <QWaitCondition>

class CapturePhotoThread : public QThread
{
    Q_OBJECT
public:
    explicit CapturePhotoThread(QObject *parent = 0);
    void takePhoto(const QString& path, QCameraImageCapture* img);


protected:
    void run();

signals:

public slots:

private:
    QString m_path;
    QCameraImageCapture *imgCapture;
    QMutex mutex;
    QWaitCondition waitCondition;

};

#endif // CAPTUREPHOTOTHREAD_H
