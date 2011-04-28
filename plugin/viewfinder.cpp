/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <sys/statvfs.h>

#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QAudioEncoderSettings>

#include "cameraifadaptor.h"
#include "viewfinder.h"
#include "settings.h"

bool compare_sizes (const QSize &s1,
                    const QSize &s2)
{
  int max_1, max_2;
  int min_1, min_2;

  if (s1.width () > s1.height ()) {
    max_1 = s1.width ();
    min_1 = s1.height ();
  } else {
    max_1 = s1.height ();
    min_1 = s1.width ();
  }

  if (s2.width () > s2.height ()) {
    max_2 = s2.width ();
    min_2 = s2.height ();
  } else {
    max_2 = s2.height ();
    min_2 = s2.width ();
  }

  if (max_1 > max_2) {
    return true;
  } else if (max_2 > max_1) {
    return false;
  } else {
    if (min_1 > min_2) {
      return true;
    } else {
      return false;
    }
  }
}

/* ViewFinder is an encapsulation of all the camera logic along with a
   QML element to display it */
ViewFinder::ViewFinder (QDeclarativeItem *_parent)
  : QDeclarativeItem (_parent),
    _ready (false),
    _cameraCount (0),
    _currentCamera (0),
    _recording (false),
    _duration (0),
    _zoom (0.0),
    _canFocus (false),
    _rotateAngle (0),
    _cameraHasFlash (true),
    _thumbnailer (0),
    _cameraService (0),
    _camera (0),
    _viewFinder (0),
    _imageCapture (0),
    _settings (0)
{
  _settings = new Settings ();

  foreach (const QByteArray &deviceName, QCamera::availableDevices ()) {
    QString description = QCamera::deviceDescription (deviceName);

    qDebug () << deviceName << " " << description;
  }

  _cameraCount = QCamera::availableDevices ().count ();
  emit cameraCountChanged ();

  _thumbnailer = new Thumbnailer ();
  connect (_thumbnailer, SIGNAL (created (const QString &, const QString &)),
           this, SLOT (thumbnailCreated (const QString &, const QString &)));
  connect (_thumbnailer, SIGNAL (error (const QStringList &, const int &,
                                        const QString &)),
           this, SLOT (thumbnailError (const QStringList &, const int &,
                                       const QString &)));

  _viewFinder = new QGraphicsVideoItem (this);

  // Set to the first camera in the list
  // FIXME: Store/restore the last used camera
  setCamera (QCamera::availableDevices () [0]);

  // Set up a DBus service
  _cameraService = new CameraService;
  new CameraIfAdaptor (_cameraService);

  QDBusConnection connection = QDBusConnection::sessionBus ();
  bool ret = connection.registerService ("com.meego.app.camera");
  if (ret == false) {
    qDebug () << "Error registering service";
    return;
  }

  ret = connection.registerObject ("/", _cameraService);
  if (ret == false) {
    qDebug () << "Error registering object";
    return;
  }
}

ViewFinder::~ViewFinder ()
{
  qDebug () << "Shutting down";
  _camera->stop ();

  if (_freeSpaceCheckTimer) {
    delete _freeSpaceCheckTimer;
  }

  delete _audioSource;
  delete _mediaRecorder;
  delete _imageCapture;
  delete _camera;
  delete _thumbnailer;
}

bool
ViewFinder::setCamera (const QByteArray &cameraDevice)
{
  // Shut down the camera
  if (_camera) {
    _camera->stop ();
  }

  delete _imageCapture;
  delete _audioSource;
  delete _mediaRecorder;
  delete _camera;

  if (cameraDevice.isEmpty ()) {
    qDebug () << "Setting default camera";
    _camera = new QCamera;
  } else {
    qDebug () << "Setting camera to " << cameraDevice;
    _camera = new QCamera (cameraDevice);
  }

  if (_camera->isMetaDataAvailable ()) {
    qDebug () << "Metadata is available";

    foreach (const QString &mdname, _camera->availableExtendedMetaData ()) {
      qDebug () << " --- " << mdname;
    }

    foreach (QtMultimediaKit::MetaData md, _camera->availableMetaData ()) {
      qDebug () << " +++ " << md;
    }

  } else {
    qDebug () << "Metadata is not available";
  }

  connect (_camera, SIGNAL (metaDataAvailableChanged (bool)),
           this, SLOT (metadataAvailableChanged (bool)));

  _audioSource = new QAudioCaptureSource (_camera);
  foreach (const QString &audioInput, _audioSource->audioInputs ()) {
    qDebug () << "Audio input: " << audioInput << " - " << _audioSource->audioDescription (audioInput);
  }
  qDebug () << "Default source: " << _audioSource->defaultAudioInput ();

  QAudioEncoderSettings audioSettings;
  audioSettings.setCodec ("audio/vorbis");
  audioSettings.setQuality (QtMultimediaKit::HighQuality);

  QVideoEncoderSettings videoSettings;
  videoSettings.setCodec ("video/theora");

  if (_settings->videoWidth () != 0 && _settings->videoHeight () != 0) {
    videoSettings.setResolution (_settings->videoWidth (),
                                 _settings->videoHeight ());
    qDebug () << "Using requested resolution: " << _settings->videoWidth () << "x" << _settings->videoHeight ();
  } else {
    qDebug () << "Using default resolution";
  }

  _mediaRecorder = new QMediaRecorder (_camera);

  foreach (qreal framerate, _mediaRecorder->supportedFrameRates ()) {
    qDebug () << "Frame rate: " << framerate;
  }
  if (_settings->videoFPS () != 0) {
    videoSettings.setFrameRate (_settings->videoFPS ());
    qDebug () << "Using requested FPS: " << _settings->videoFPS ();
  } else {
    qDebug () << "Using default FPS";
  }

  foreach (QSize resolution, _mediaRecorder->supportedResolutions ()) {
    qDebug () << "Video resolution: " << resolution.width () << "x" << resolution.height ();
  }

  QList<QStringList> preferredCodecCombos;
  preferredCodecCombos << QString("audio/mpeg, video/x-h264, mp4").split(", ");
  preferredCodecCombos << QString("audio/vorbis, video/theora, ogg").split(", ");
  QStringList audioCodecs;
  QStringList videoCodecs;
  QStringList containers;

  foreach (QString codec, _mediaRecorder->supportedAudioCodecs ()) {
    qDebug () << "Codec: " << codec;
    audioCodecs << codec;
  }
  foreach (QString codec, _mediaRecorder->supportedVideoCodecs ()) {
    qDebug () << "Codec: " << codec;
    videoCodecs << codec;
  }
  foreach (QString container, _mediaRecorder->supportedContainers ()) {
    qDebug () << "Container: " << container;
    containers.append(container);
  }
  bool foundMatch = false;
  foreach (QStringList codecCombo, preferredCodecCombos) {
    qDebug() << "tuple: " << codecCombo;
    qDebug() << "codecCombo[0]" << codecCombo[0];
    if (audioCodecs.contains(codecCombo[0]) &&
	videoCodecs.contains(codecCombo[1]) &&
	containers.contains(codecCombo[2])) {

      qDebug() << "preferred tuple found: " << codecCombo;
      foundMatch = true;
      audioSettings.setCodec(codecCombo[0]);
      videoSettings.setCodec(codecCombo[1]);
      _mediaRecorder->setEncodingSettings(audioSettings, videoSettings, codecCombo[2]);
      _videoFilenameExtension = codecCombo[2];
    }
  }
  if (!foundMatch) {
    qDebug() << "No codec combos found!  pretending that ogg works";
    videoSettings.setCodec ("video/theora");
    audioSettings.setCodec ("audio/vorbis");
    _mediaRecorder->setEncodingSettings (audioSettings, videoSettings, "ogg");
    _videoFilenameExtension = "ogg";
  }

  qDebug () << "Selected container: " << _mediaRecorder->containerMimeType ();
  connect (_mediaRecorder, SIGNAL (stateChanged (QMediaRecorder::State)),
           this, SLOT (mediaRecorderStateChanged (QMediaRecorder::State)));
  connect (_mediaRecorder, SIGNAL (error (QMediaRecorder::Error)),
           this, SLOT (mediaRecorderError (QMediaRecorder::Error)));
  connect (_mediaRecorder, SIGNAL (durationChanged (qint64)),
           this, SLOT (mediaRecorderDurationChanged (qint64)));

  _imageCapture = new QCameraImageCapture (_camera);

  QList <QSize> resolutions;
  resolutions = _imageCapture->supportedResolutions ();
  QSize imageSize;

  if (!resolutions.isEmpty ()) {
    qSort (resolutions.begin (), resolutions.end (), compare_sizes);
    foreach (QSize size, resolutions) {
      qDebug () << "Sorted resolution: " << size.width () << "x" << size.height ();
    }

    imageSize = resolutions.first ();
  } else {
    imageSize = QSize (1280, 800);
  }

  QImageEncoderSettings imageSettings;
  imageSettings.setResolution (imageSize);
  _imageCapture->setEncodingSettings (imageSettings);

  qDebug () << "Using resolution: " << _imageCapture->encodingSettings ().resolution ().width () << "x" << _imageCapture->encodingSettings ().resolution ().height ();

  _viewFinder->setSize (imageSize);
  _viewFinder->setTransformOriginPoint (imageSize.width () / 2, imageSize.height () / 2);

  // Centre this in the view
  QRectF itemBounds = this->boundingRect ();
  float x = (itemBounds.width () - imageSize.width ()) / 2;
  float y = (itemBounds.height () - imageSize.height ()) / 2;
  _viewFinder->setPos (x, y);

  _camera->setViewfinder (_viewFinder);
  _camera->setCaptureMode (QCamera::CaptureStillImage);

  connect (_camera, SIGNAL (stateChanged (QCamera::State)),
           this, SLOT (updateCameraState (QCamera::State)));
  connect (_camera, SIGNAL (error (QCamera::Error)),
           this, SLOT (displayCameraError ()));
  connect (_camera, SIGNAL (lockStatusChanged (QCamera::LockStatus,
                                               QCamera::LockChangeReason)),
           this, SLOT (updateLockStatus (QCamera::LockStatus,
                                         QCamera::LockChangeReason)));

  connect (_imageCapture, SIGNAL (imageCaptured (int, const QImage &)),
           this, SLOT (imageCaptured (int, const QImage &)));
  connect (_imageCapture, SIGNAL (imageSaved (int, const QString &)),
           this, SLOT (imageSaved (int, const QString &)));
  connect (_imageCapture, SIGNAL (error (int,
                                         QCameraImageCapture::Error,
                                         const QString &)),
           this, SLOT (imageCaptureError (int,
                                          QCameraImageCapture::Error,
                                          const QString &)));
  connect (_imageCapture, SIGNAL (readyForCaptureChanged (bool)),
           this, SLOT (imageReadyForCaptureChanged (bool)));

  updateCameraState (_camera->state ());
  updateLockStatus (_camera->lockStatus (), QCamera::UserRequest);
  imageReadyForCaptureChanged (_imageCapture->isReadyForCapture ());
  mediaRecorderStateChanged (_mediaRecorder->state ());

  _camera->start ();

  // Fire this on an idle so that the signal will be picked up when the
  // object has been created
  QTimer::singleShot(0, this, SLOT(checkSpace ()));
  return true;
}

void
ViewFinder::updateCameraState (QCamera::State state)
{
  qDebug () << "Updated state: " << state;

  if (state != QCamera::ActiveState) {
    return;
  }

  qDebug () << "Supported maximum optical zoom" << _camera->focus ()->maximumOpticalZoom ();
  qDebug () << "Supported maximum digital zoom" << _camera->focus ()->maximumDigitalZoom ();

  _canFocus = _camera->focus ()->isAvailable ();
  emit canFocusChanged ();

  if (_canFocus) {
    // Recalculate the zoom as relative to the current camera's max zoom
    _camera->focus ()->zoomTo (1.0 + (_zoom * _camera->focus ()->maximumOpticalZoom ()), 1.0);
  }

  // Generate the flash menu
  _cameraHasFlash = _camera->exposure ()->isFlashModeSupported (QCameraExposure::FlashOn);
  emit cameraHasFlashChanged ();

  QStringList menu;
  _cameraHasAutoFlash = _camera->exposure ()->isFlashModeSupported (QCameraExposure::FlashAuto);
  if (_cameraHasAutoFlash) {
    menu.append (tr ("Auto"));
  }
  menu.append (tr ("Off"));
  menu.append (tr ("On"));
  _flashModel = QVariant::fromValue (menu);
  emit flashModelChanged ();
}

void
ViewFinder::displayCameraError ()
{
  qDebug () << "Camera error: " << _camera->errorString ();
}

void
ViewFinder::updateLockStatus (QCamera::LockStatus status,
                              QCamera::LockChangeReason reason)
{
  qDebug () << "Camera lock changed: " << status << " (Reason " << reason << ")";
}

void
ViewFinder::takePhoto ()
{
  QString filename = ViewFinder::generateImageFilename ();

  qDebug () << "Filename: " << filename;

  // FIXME: Use toUTF8?
  _imageCapture->capture (filename.toAscii ());
}

void
ViewFinder::imageCaptured (int id, const QImage &preview)
{
  Q_UNUSED(preview)
  qDebug () << "Image captured: " << id;

  emit imageCaptured ();
}

void ViewFinder::completeImage (const QString &filename)
{
  qDebug () << "Image completed: " << filename;

  _imageLocation = filename;
  emit imageLocationChanged ();

  _cameraService->emitImageCaptured (filename);
}

void
ViewFinder::imageSaved (int id, const QString &filename)
{
  qDebug () << "Image saved: " << id << " - " << filename;

  completeImage (filename);
}

void
ViewFinder::imageCaptureError (int id,
                               QCameraImageCapture::Error error,
                               const QString &message)
{
  if (error == QCameraImageCapture::OutOfSpaceError) {
    emit noSpaceOnDevice ();
  }

  qDebug () << "Image error: " << id << " - " << message << "(" << error << ")";
}

void
ViewFinder::imageReadyForCaptureChanged (bool ready)
{
  qDebug () << "Image ready for capture: " << (ready ? "true" : "false");
  _ready = ready;
  emit readyChanged ();
}

/*
void
ViewFinder::checkSupportedFlashModes ()
{
}
*/
void
ViewFinder::setFlashMode (int mode)
{
  QCameraExposure::FlashMode m = QCameraExposure::FlashOff;

  // This is an ugly hack to prevent the flash when the user-facing camera
  // is selected. Qt-mobility cannot associate a camera with a flash
  if (_currentCamera == 1) {
    return;
  }

  // If the flash menu doesn't have an Auto option then the mode will be
  // off by one.
  if (_cameraHasAutoFlash == false) {
    mode += 1;
  }

  switch (mode) {
  case ViewFinder::Off:
    m = QCameraExposure::FlashOff;
    break;

  case ViewFinder::On:
    m = QCameraExposure::FlashOn;
    break;

  case ViewFinder::Auto:
    m = QCameraExposure::FlashAuto;
    break;
  }

  qDebug () << "Setting flash to " << mode << "(" << m << ")";

  _settings->setFlashMode ((ViewFinder::FlashMode) mode);
  _camera->exposure ()->setFlashMode (m);
  emit (flashModeChanged ());
}

int
ViewFinder::flashMode () {
  int m = (int) _settings->flashMode ();
  if (_cameraHasAutoFlash == false) {
    m -= 1;
  }

  return m;
}

bool
ViewFinder::changeCamera ()
{
  int nextCamera = _currentCamera + 1;

  if (nextCamera >= _cameraCount) {
    nextCamera = 0;
  }

  qDebug () << "Switching from camera " << _currentCamera << " to " << nextCamera;

  if (nextCamera == _currentCamera) {
    // If we're still on the same camera then we don't need to do anything
    return true;
  }

  if (setCamera (QCamera::availableDevices ()[nextCamera]) == true) {
    _currentCamera = nextCamera;

#if 0
    // Compiling this will pretend that the camera can focus.
    _canFocus = true;
    emit canFocusChanged ();
#endif

    emit cameraChanged ();
    return true;
  }

  return false;
}

void
ViewFinder::setCaptureMode (ViewFinder::CaptureMode mode)
{
  qDebug () << "Setting capture mode: " << mode;

  switch (mode) {
  case ViewFinder::Still:
    qDebug () << "Image capture";
    _camera->setCaptureMode (QCamera::CaptureStillImage);
    break;

  case ViewFinder::Video:
    qDebug () << "Video capture";
    _camera->setCaptureMode (QCamera::CaptureVideo);
    break;

  default:
    qDebug () << "Unknown capture mode";
    break;
  }

  _settings->setCaptureMode (mode);
  emit captureModeChanged ();
}

ViewFinder::CaptureMode
ViewFinder::captureMode () {
  return _settings->captureMode ();
}

void
ViewFinder::setZoom (qreal z)
{
  _zoom = z;
  _camera->focus ()->zoomTo (1.0 + (z * _camera->focus ()->maximumOpticalZoom ()), 1.0);

  qDebug () << "Setting zoom to:" << _zoom;
  emit zoomChanged ();
}

void
ViewFinder::startRecording ()
{
  QString filename = ViewFinder::generateVideoFilename ();
  QUrl url;
  qDebug () << "Starting recording" << filename;

  url = QUrl::fromLocalFile (filename);
  _currentLocation = url.toString ();
  _mediaRecorder->setOutputLocation (QUrl (filename));

  // Check if we're running out of space every 5 seconds
  _freeSpaceCheckTimer = new QTimer (this);
  connect (_freeSpaceCheckTimer, SIGNAL (timeout ()),
           this, SLOT (checkSpace ()));
  _freeSpaceCheckTimer->start (5000);

  // _camera->searchAndLock ();
  _mediaRecorder->record ();
  setRecording (true);
}

void
ViewFinder::endRecording ()
{
  QString mimetype = QString ("video/mpeg");
  qDebug () << "Ending recording";

  _freeSpaceCheckTimer->stop ();
  delete _freeSpaceCheckTimer;
  _freeSpaceCheckTimer = 0;

  setRecording (false);
  _mediaRecorder->stop ();
  //_camera->unlock ();

  _thumbnailer->requestThumbnail (_currentLocation, mimetype);
}

QString
ViewFinder::generateImageFilename ()
{
  QString path = QDir::homePath ().append ("/Pictures/Camera/");
  QDateTime now = QDateTime::currentDateTime ();

  return path.append (now.toString ("yyyy.MM.dd-hh.mm.ss'.jpg'"));
}

QString
ViewFinder::generateVideoFilename ()
{
  QString path = QDir::homePath ().append ("/Videos/Camera/");
  QDateTime now = QDateTime::currentDateTime ();

  return path.append (now.toString ("yyyy.MM.dd-hh.mm.ss")).append (".%1").arg(_videoFilenameExtension);
}

void
ViewFinder::mediaRecorderStateChanged (QMediaRecorder::State state)
{
  qDebug () << "Media recorder state changed: " << state;
}

void
ViewFinder::mediaRecorderError (QMediaRecorder::Error error)
{
  if (error == QMediaRecorder::ResourceError) {
    // We don't get a more specific error so it might be out of space
    checkSpace ();
  }

  qDebug () << "Media recorder error: " << _mediaRecorder->errorString ();
}

void
ViewFinder::mediaRecorderDurationChanged (qint64 duration)
{
  qDebug () << "Duration: " << duration;
  setDuration (duration);
}

void
ViewFinder::repositionViewFinder (const QRectF &geometry)
{
  QSizeF size = _viewFinder->size ();
  qDebug () << "Viewfinder size: " << size.width () << "x" << size.height ();

  float x, y;

  x = (geometry.width () - size.width ()) / 2;
  y = (geometry.height () - size.height ()) / 2;

  _viewFinder->setPos (x, y);

  /* This was trying to be too clever, maybe on day it'll be useful
  float r1 = geometry.width () / size.width ();
  float r2 = geometry.height () / size.height ();
  float r = size.width () / size.height ();
  float w, h;

  if (r1 > r2) {
    h = size.height () * r1;
    w = h * r;
  } else {
    w = size.width () * r2;
    h = w * r;
  }

  qDebug () << "Setting size as " << w << "x" << h;
  _viewFinder->setSize (QSizeF (w, h));
  _viewFinder->setTransformOriginPoint (w / 2, h / 2);
  */
}

void
ViewFinder::geometryChanged (const QRectF &newGeometry,
                             const QRectF &oldGeometry)
{
  qDebug () << "Geometry changed: " << oldGeometry.width () << "x" << oldGeometry.height () << " -> " << newGeometry.width () << "x" << newGeometry.height ();

  if (newGeometry != oldGeometry) {
    if (newGeometry.width () > 0 && newGeometry.height () > 0) {
      repositionViewFinder (newGeometry);
    }
  }

  QDeclarativeItem::geometryChanged (newGeometry, oldGeometry);
}

void
ViewFinder::checkSpace ()
{
  QString homePath = QDir::homePath ();
  struct statvfs buf;

  if (statvfs (homePath.toAscii (), &buf) < 0) {
    qDebug () << " Error stating filesystem";
    emit noSpaceOnDevice ();
    return;
  }

  qDebug () << "Free space: " << buf.f_bsize * buf.f_bavail << "(" <<
    buf.f_bavail << " * " << buf.f_bsize << ")";

  // Limit to 5mb space
  if (buf.f_bsize * buf.f_bavail < (1024 * 1024 * 5)) {
    qDebug () << "No free space";
    emit noSpaceOnDevice ();
    return;
  }
}

void
ViewFinder::thumbnailCreated (const QString &url,
                              const QString &md5sum)
{
  Q_UNUSED(url)
  _imageLocation = QString ("%1/.thumbnails/normal/%2.jpeg").arg (QDir::homePath ()).arg (md5sum);

  qDebug () << "Thumbnail completed: " << _imageLocation;
  emit imageLocationChanged ();
}

void
ViewFinder::thumbnailError (const QStringList &urls,
                            const int         &errorId,
                            const QString     &errorMessage)
{
  Q_UNUSED(urls)
  qDebug () << "Thumbnail Error: " << errorMessage << "(" << errorId << ")";
}

void
ViewFinder::metadataAvailableChanged (bool avail)
{
  qDebug () << "Metadata available: " << avail;
}

void
ViewFinder::enterStandbyMode ()
{
  if (_camera) {
    _camera->stop ();
  }
}

void
ViewFinder::leaveStandbyMode ()
{
  if (_camera) {
    _camera->start ();
  }
}

void
ViewFinder::setRotateAngle (int angle)
{
  if (_rotateAngle == angle) {
    return;
  }

  _rotateAngle = angle;

  _viewFinder->setRotation (angle);
}

//QML_DECLARE_TYPE(ViewFinder);
