/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef _VIEWFINDER_H_
#define _VIEWFINDER_H_

#include <qcamera.h>
#include <qcameraviewfinder.h>
#include <qcameraimagecapture.h>
#include <qaudiocapturesource.h>
#include <qmediarecorder.h>
#include <qgraphicsvideoitem.h>

#include <QDeclarativeItem>
#include <QtLocation/QGeoCoordinate>

#include <policy/resource-set.h>

#include "cameraservice.h"
#include "thumbnailer.h"

#include "capturephotothread.h"

class Settings;

QTM_USE_NAMESPACE

class ViewFinder : public QDeclarativeItem
{
    Q_OBJECT;

  public:
    ViewFinder (QDeclarativeItem *parent = 0);
    ~ViewFinder ();

    enum FlashMode {
        Auto,
        Off,
        On
    };

    enum CaptureMode {
        Still,
        Video
    };

    Q_ENUMS (FlashMode);
    Q_ENUMS (CaptureMode);

    Q_PROPERTY (bool ready READ ready NOTIFY readyChanged);
    bool ready () { return _ready; }

    Q_PROPERTY (int flashMode READ flashMode WRITE setFlashMode NOTIFY flashModeChanged);
    int flashMode ();
    void setFlashMode (int mode);

    Q_PROPERTY (int cameraCount READ cameraCount NOTIFY cameraCountChanged);
    int cameraCount () { return _cameraCount; }

    Q_PROPERTY (int currentCamera READ currentCamera NOTIFY cameraChanged);
    int currentCamera () { return _currentCamera; }

    Q_PROPERTY (ViewFinder::CaptureMode captureMode READ captureMode WRITE setCaptureMode NOTIFY captureModeChanged);
    ViewFinder::CaptureMode captureMode ();
    void setCaptureMode (ViewFinder::CaptureMode mode);

    Q_PROPERTY (bool recording READ recording NOTIFY recordingChanged);
    bool recording () { return _recording; }

    Q_PROPERTY (qint64 duration READ duration NOTIFY durationChanged);
    qint64 duration () { return _duration; }

    Q_PROPERTY(QString durationString READ durationString NOTIFY durationStringChanged)
    QString durationString();

    Q_PROPERTY (qreal maxZoom READ maxZoom NOTIFY maxZoomChanged);
    qreal maxZoom () { return _maxZoom; }

    Q_PROPERTY (qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged);
    qreal zoom () { return _zoom; }
    void setZoom (qreal z);

    Q_PROPERTY (QString imageLocation READ imageLocation WRITE setImageLocation NOTIFY imageLocationChanged);
    QString imageLocation () { return _imageLocation; }
    void setImageLocation(const QString & _str);

    Q_PROPERTY (bool canFocus READ canFocus NOTIFY canFocusChanged);
    bool canFocus () { return _canFocus; }

    Q_PROPERTY (bool canLockFocus READ canLockFocus);
    bool canLockFocus () const { return _canLockFocus; }

    Q_PROPERTY (int rotateAngle READ rotateAngle WRITE setRotateAngle NOTIFY rotateAngleChanged);
    int rotateAngle () { return _rotateAngle; }
    void setRotateAngle (int angle);

    Q_PROPERTY (QVariant flashModel READ flashModel NOTIFY flashModelChanged);
    QVariant flashModel () { return _flashModel; }

    Q_PROPERTY (bool cameraHasFlash READ cameraHasFlash NOTIFY cameraHasFlashChanged);
    bool cameraHasFlash () { return _cameraHasFlash; }

    Q_PROPERTY (int currentOrientation READ currentOrientation WRITE setCurrentOrientation NOTIFY currentOrientationChanged);
    int currentOrientation() { return _currentOrientation; }

    Q_PROPERTY (int lastPhotoOrientation READ lastPhotoOrientation);
    int lastPhotoOrientation() { return _lastPhotoOrientation; }

    Q_PROPERTY(QString capturedVideoLocation READ capturedVideoLocation NOTIFY capturedVideoLocationChanged);
    QString capturedVideoLocation() { return _currentLocation; }

    Q_PROPERTY(bool flashSettingsAvaliable READ flashSettingsAvaliable NOTIFY flashSettingsAvaliableChanged);
    bool flashSettingsAvaliable() { return _flashSettingsAvaliable; }

  Q_SIGNALS:
    void readyChanged ();
    void flashModeChanged ();
    void cameraCountChanged ();
    void captureModeChanged ();
    void recordingChanged ();
    void durationChanged ();
    void durationStringChanged();
    void maxZoomChanged();
    void zoomChanged ();
    void imageLocationChanged ();
    void canFocusChanged ();
    void cameraChanged ();
    void rotateAngleChanged ();
    void flashModelChanged ();
    void cameraHasFlashChanged ();

    void imageCapturedSig ();
    void noSpaceOnDevice ();

    void focusLocked();

    void currentOrientationChanged();
    void capturedVideoLocationChanged();

    void flashSettingsAvaliableChanged();

  public slots:
    Q_INVOKABLE void takePhoto ();
    Q_INVOKABLE void startRecording ();
    Q_INVOKABLE void endRecording ();
    Q_INVOKABLE bool changeCamera ();
    Q_INVOKABLE void enterStandbyMode ();
    Q_INVOKABLE void leaveStandbyMode ();
    Q_INVOKABLE void setCurrentOrientation(int orientation);
    Q_INVOKABLE void startFocus();

  protected:
    virtual void geometryChanged (const QRectF &newGeometry,
                                  const QRectF &oldGeometry);

    void releaseCamera();

    // policy-aware support
    void initResourcePolicy();
    bool policyAware() const { return _bPolicyAware; }
  private slots:
    void acquire();
    void release();

  private slots:
    void init();
    void initExtra();

    void updateCameraState (QCamera::State);
    void displayCameraError ();
    void updateLockStatus (QCamera::LockStatus status,
                           QCamera::LockChangeReason reason);
    void locked();
    void imageCaptured (int id, const QImage &preview);
    void imageSaved (int id, const QString &filename);
    void imageCaptureError (int id,
                            QCameraImageCapture::Error error,
                            const QString &message);
    void imageReadyForCaptureChanged (bool ready);
    void readyForCaptureChanged(bool);

    void mediaRecorderStateChanged (QMediaRecorder::State state);
    void mediaRecorderError (QMediaRecorder::Error error);
    void mediaRecorderDurationChanged (qint64 duration);
    void checkSpace ();
    void thumbnailError (const QStringList &urls,
                         const int         &error_id,
                         const QString     &error_message);
    void thumbnailCreated (const QString &url,
                           const QString &md5sum);
    void metadataAvailableChanged (bool avail);

    void completeImage ();

    void setLastCoordinate(QGeoCoordinate _lastCoordinate) { m_lastCoordinate = _lastCoordinate; }

    // policy-aware support
    void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType> &);
    void resourceLostHandler();

  private:
    QString generateTemporaryImageFilename () const;
    QString generateBaseImageFilename () const;
    QString generateImageFilename () const;
    QString generateVideoFilename () const;

    const QString & picturesDir () const { return _strPicturesDir; }
    const QString & videosDir () const { return _strVideosDir; }

    void setRecording (bool r) { _recording = r; emit recordingChanged (); }
    void setDuration (qint64 d) { _duration = d; emit durationChanged (); emit durationStringChanged();}

    bool setCamera (const QByteArray &cameraDevice);
    void repositionViewFinder (const QRectF &geometry);

    bool _ready, _started, _init;
    int _cameraCount;
    int _currentCamera;
    bool _recording;
    qint64 _duration;
    qreal _zoom;
    qreal _maxZoom;
    QString _imageLocation;
    QString _currentLocation;
    bool _canFocus, _canLockFocus;
    bool _rotateAngle;

    bool _cameraHasFlash;
    bool _cameraHasAutoFlash;
    QVariant _flashModel;

    Thumbnailer *_thumbnailer;
    CameraService *_cameraService;
    QCamera *_camera;
    QGraphicsVideoItem *_viewFinder;
    QCameraImageCapture *_imageCapture;
    QAudioCaptureSource *_audioSource;
    QMediaRecorder *_mediaRecorder;
    QString _videoFilenameExtension;

    Settings *_settings;

    QTimer *_freeSpaceCheckTimer;
    QGeoCoordinate m_lastCoordinate;

    QFutureWatcher<QString> _futureWatcher;

    CapturePhotoThread photoThread;

    QString _strPicturesDir, _strVideosDir;

    int _currentOrientation, _lastPhotoOrientation;

    // policy-aware support
    bool _bPolicyAware;
    ResourcePolicy::ResourceSet *_resourceSet;
    bool _bSkipCameraReset;

    bool _flashSettingsAvaliable;
};

#endif
