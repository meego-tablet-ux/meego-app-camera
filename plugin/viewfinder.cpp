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
#include <QtLocation/QGeoCoordinate>
#include <QFutureWatcher>
#include <QFuture>
#include <QSettings>
#include <QApplication>
#include <QFile>

#include "cameraifadaptor.h"
#include "viewfinder.h"
#include "settings.h"
#include "exifdatafactory.h"
#include "jpegexiferizer.h"

namespace  {

QString addGeoTag(QString tmpPath, QString destPath, QGeoCoordinate coord, int orientation, bool frontFacingCamera)
{
    ExifDataFactory *factory = new ExifDataFactory(coord, orientation, frontFacingCamera);
    JpegExiferizer exifer(tmpPath, destPath);
    exifer.setExifDataFactory(factory);
    exifer.doIt();

    delete factory;
    QFile::remove(tmpPath);
    return destPath;
}

}

QTM_USE_NAMESPACE

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
    _started(false), _init(true),
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
    _settings (0),
    _currentOrientation(0), _lastPhotoOrientation(0)
{
  _settings = new Settings ();

  _viewFinder = new QGraphicsVideoItem (this);
  _viewFinder->setVideoRenderingMode(VideoRenderingHintOverlay);

  QTimer::singleShot(0, this, SLOT(init()));
}

void ViewFinder::init()
{
  foreach (const QByteArray &deviceName, QCamera::availableDevices ()) {
    QString description = QCamera::deviceDescription (deviceName);
#ifdef SHOW_DEBUG
    qDebug () << deviceName << " " << description;
#endif
  }

  QList<QByteArray> devs(QCamera::availableDevices ());
  _cameraCount = devs.count ();
  emit cameraCountChanged ();

  // Try to restore the last used camera
  QByteArray strDev(_settings->cameraDevice());
  int nFind;

  if (strDev.isEmpty() || 0 > (nFind = devs.indexOf(strDev))) {
      // previously used camera can't be found - set to the first available
      strDev = _cameraCount ? devs[0] : "";
      _settings->setCameraDevice(strDev);
  }
  else {
      _currentCamera = nFind;
  }

  setCamera (strDev);

  qRegisterMetaType<QGeoCoordinate>("QGeoCoordinate");
  connect(&photoThread, SIGNAL(lastCoordinate(QGeoCoordinate)), this, SLOT(setLastCoordinate(QGeoCoordinate)));
  photoThread.start();

  QTimer::singleShot(0, this, SLOT(initExtra()));
}


void ViewFinder::initExtra()
{
  _thumbnailer = new Thumbnailer ();
  connect (_thumbnailer, SIGNAL (created (const QString &, const QString &)),
           this, SLOT (thumbnailCreated (const QString &, const QString &)));
  connect (_thumbnailer, SIGNAL (error (const QStringList &, const int &,
                                        const QString &)),
           this, SLOT (thumbnailError (const QStringList &, const int &,
                                       const QString &)));

  // Set up a DBus service
//  _cameraService = new CameraService;
//  new CameraIfAdaptor (_cameraService);

//  QDBusConnection connection = QDBusConnection::sessionBus ();
//  bool ret = connection.registerService ("com.meego.app.camera");
//  if (ret == false) {
//#ifdef SHOW_DEBUG
//    qDebug () << "Error registering service";
//#endif
//    return;
//  }

//  ret = connection.registerObject ("/", _cameraService);
//  if (ret == false) {
//#ifdef SHOW_DEBUG
//    qDebug () << "Error registering object";
//#endif
//    return;
//  }

  connect(&_futureWatcher, SIGNAL(finished()),this,SLOT(completeImage()));

  // retrieve pictures and videos location from user-dirs.dirs or user-dirs.defaults files
  QString strUserDirsFile(QDir::homePath() + "/.config/user-dirs.dirs");
  if (QFile::exists(strUserDirsFile)) {
    QSettings userDirs(strUserDirsFile, QSettings::NativeFormat);
    _strPicturesDir = QString::fromUtf8(userDirs.value("XDG_PICTURES_DIR").toByteArray());
    _strVideosDir = QString::fromUtf8(userDirs.value("XDG_VIDEOS_DIR").toByteArray());

  }

  if (_strPicturesDir.isEmpty() || _strVideosDir.isEmpty()) {
    strUserDirsFile = "/etc/xdg/user-dirs.defaults";
    if (QFile::exists(strUserDirsFile)) {
      QSettings userDirs(strUserDirsFile, QSettings::NativeFormat);
      if (_strPicturesDir.isEmpty())
        _strPicturesDir = QString::fromUtf8(userDirs.value("PICTURES").toByteArray());
      if (_strVideosDir.isEmpty())
        _strVideosDir = QString::fromUtf8(userDirs.value("VIDEOS").toByteArray());
    }

    if (_strPicturesDir.isEmpty())
      _strPicturesDir = "Pictures";
    if (_strVideosDir.isEmpty())
      _strVideosDir = "Videos";
  }

  _strPicturesDir.replace("$HOME/", "");
  _strVideosDir.replace("$HOME/", "");

  _strPicturesDir += "/Camera";
  _strVideosDir += "/Camera";

  if (QDir::home().mkpath (_strPicturesDir) == false) {
#ifdef SHOW_DEBUG
    qDebug () << "Error making camera directory: " << QDir::homePath () << "/Pictures/Camera";
#endif
  }

  if (QDir::home().mkpath (_strVideosDir) == false) {
#ifdef SHOW_DEBUG
    qDebug () << "Error making camera directory: " << QDir::homePath () << "/Pictures/Camera";
#endif
  }

  _strPicturesDir.prepend(QDir::homePath() + '/');
  _strVideosDir.prepend(QDir::homePath() + '/');

  _lastPhotoOrientation =_settings->lastCapturedPhotoOrientation();
  setImageLocation(_settings->lastCapturedPhotoPath());
}

ViewFinder::~ViewFinder ()
{
#ifdef SHOW_DEBUG
  qDebug () << "Shutting down";
#endif
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
#ifdef SHOW_DEBUG
    qDebug () << "Setting default camera";
#endif
    _camera = new QCamera;
  } else {
#ifdef SHOW_DEBUG
    qDebug () << "Setting camera to " << cameraDevice;
#endif
    _camera = new QCamera (cameraDevice);
  }

#ifdef SHOW_DEBUG
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
#endif

  _audioSource = new QAudioCaptureSource (_camera);

#ifdef SHOW_DEBUG
  foreach (const QString &audioInput, _audioSource->audioInputs ()) {
    qDebug () << "Audio input: " << audioInput << " - " << _audioSource->audioDescription (audioInput);
  }

  qDebug () << "Default source: " << _audioSource->defaultAudioInput ();
#endif

  QAudioEncoderSettings audioSettings;
  audioSettings.setCodec ("audio/vorbis");
  audioSettings.setQuality (QtMultimediaKit::HighQuality);

  QVideoEncoderSettings videoSettings;
  videoSettings.setCodec ("video/theora");

  _mediaRecorder = new QMediaRecorder (_camera);

#ifdef SHOW_DEBUG
  foreach (QSize resolution, _mediaRecorder->supportedResolutions ()) {
    qDebug () << "Supported video resolution: " << resolution.width () << "x" << resolution.height ();
  }
#endif

  if (_settings->videoWidth () != 0 && _settings->videoHeight () != 0) {
    videoSettings.setResolution (_settings->videoWidth (),
                                 _settings->videoHeight ());
#ifdef SHOW_DEBUG
    qDebug () << "Using requested resolution: " << _settings->videoWidth () << "x" << _settings->videoHeight ();
#endif
  }
  else if(_mediaRecorder->supportedResolutions ().contains(QSize(1280,720))) {
    videoSettings.setResolution (1280, 720);
#ifdef SHOW_DEBUG
    qDebug () << "Using preferred resolution: 1280x720";
#endif
  }
  else
  {
#ifdef SHOW_DEBUG
    qDebug () << "Using default resolution";
#endif
  }

#ifdef SHOW_DEBUG
  foreach (qreal framerate, _mediaRecorder->supportedFrameRates ()) {
    qDebug () << "Supported video frame rate: " << framerate;
  }
#endif

  if (_settings->videoFPS () != 0) {
    videoSettings.setFrameRate (_settings->videoFPS ());
#ifdef SHOW_DEBUG
    qDebug () << "Using requested FPS: " << _settings->videoFPS ();
#endif
  } else if(_mediaRecorder->supportedFrameRates ().contains(30)) {
    videoSettings.setFrameRate (30);
#ifdef SHOW_DEBUG
    qDebug () << "Using preferred FPS: 30";
#endif
  }
  else
  {
#ifdef SHOW_DEBUG
      qDebug () << "Using default FPS";
#endif
  }

  QList<QStringList> preferredCodecCombos;
  preferredCodecCombos << QString("audio/mpeg, video/x-h264, mp4").split(", ");
  preferredCodecCombos << QString("audio/vorbis, video/theora, ogg").split(", ");
  QStringList audioCodecs;
  QStringList videoCodecs;
  QStringList containers;

  foreach (QString codec, _mediaRecorder->supportedAudioCodecs ()) {
#ifdef SHOW_DEBUG
    qDebug () << "Codec: " << codec;
#endif
    audioCodecs << codec;
  }
  foreach (QString codec, _mediaRecorder->supportedVideoCodecs ()) {
#ifdef SHOW_DEBUG
    qDebug () << "Codec: " << codec;
#endif
    videoCodecs << codec;
  }
  foreach (QString container, _mediaRecorder->supportedContainers ()) {
#ifdef SHOW_DEBUG
    qDebug () << "Container: " << container;
#endif
    containers.append(container);
  }
  bool foundMatch = false;
  foreach (QStringList codecCombo, preferredCodecCombos) {
#ifdef SHOW_DEBUG
    qDebug() << "tuple: " << codecCombo;
    qDebug() << "codecCombo[0]" << codecCombo[0];
#endif
    if (audioCodecs.contains(codecCombo[0]) &&
	videoCodecs.contains(codecCombo[1]) &&
	containers.contains(codecCombo[2])) {

#ifdef SHOW_DEBUG
      qDebug() << "preferred tuple found: " << codecCombo;
#endif

      foundMatch = true;
      audioSettings.setCodec(codecCombo[0]);
      videoSettings.setCodec(codecCombo[1]);
      _mediaRecorder->setEncodingSettings(audioSettings, videoSettings, codecCombo[2]);
      _videoFilenameExtension = codecCombo[2];
    }
  }
  if (!foundMatch) {
#ifdef SHOW_DEBUG
    qDebug() << "No codec combos found!  pretending that ogg works";
#endif

    videoSettings.setCodec ("video/theora");
    audioSettings.setCodec ("audio/vorbis");
    _mediaRecorder->setEncodingSettings (audioSettings, videoSettings, "ogg");
    _videoFilenameExtension = "ogg";
  }

#ifdef SHOW_DEBUG
  qDebug () << "Selected container: " << _mediaRecorder->containerMimeType ();
#endif

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
#ifdef SHOW_DEBUG
    foreach (QSize size, resolutions) {
      qDebug () << "Sorted resolution: " << size.width () << "x" << size.height ();
    }
#endif

    imageSize = resolutions.first ();
  } else {
    imageSize = QSize (1280, 800);
  }

  QImageEncoderSettings imageSettings;
  imageSettings.setResolution (imageSize);
  _imageCapture->setEncodingSettings (imageSettings);

#ifdef SHOW_DEBUG
  qDebug () << "Using resolution: " << _imageCapture->encodingSettings ().resolution ().width () << "x" << _imageCapture->encodingSettings ().resolution ().height ();
#endif

  QSize viewFinderSize(1280, 800);
  _viewFinder->setSize (viewFinderSize);
  _viewFinder->setTransformOriginPoint (viewFinderSize.width () / 2, viewFinderSize.height () / 2);

  // Centre this in the view
  QRectF itemBounds = this->boundingRect ();
  float x = (itemBounds.width () - viewFinderSize.width ()) / 2;
  float y = (itemBounds.height () - viewFinderSize.height ()) / 2;
  _viewFinder->setPos (x, y);

  _camera->setViewfinder (_viewFinder);

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

//  updateCameraState (_camera->state ());
  updateLockStatus (_camera->lockStatus (), QCamera::UserRequest);
  imageReadyForCaptureChanged (_imageCapture->isReadyForCapture ());
  mediaRecorderStateChanged (_mediaRecorder->state ());

  // set capture mode from settings
  _camera->setCaptureMode (Video != _settings->captureMode () ? QCamera::CaptureStillImage : QCamera::CaptureVideo);
  _camera->start ();
  _started = true; // set the flag to avoid starting camera second time

  // Fire this on an idle so that the signal will be picked up when the
  // object has been created
  QTimer::singleShot(1000, this, SLOT(checkSpace ()));
  return true;
}

void
ViewFinder::updateCameraState (QCamera::State state)
{
#ifdef SHOW_DEBUG
  qDebug () << "Updated state: " << state;
#endif

  if (state != QCamera::ActiveState) {
    return;
  }

  _canFocus = _camera->focus ()->isAvailable ();
  emit canFocusChanged ();

  if (_canFocus) {
    _maxZoom = _camera->focus()->maximumDigitalZoom() * _camera->focus()->maximumOpticalZoom();

    // Recalculate the zoom as relative to the current camera's max zoom
    _camera->focus ()->zoomTo (1.0 + (_zoom * _camera->focus ()->maximumOpticalZoom ()), 1.0);
  }
  else {
      _maxZoom = 1.; // zoom not supported
  }
  emit maxZoomChanged ();

  // Generate the flash menu
  _cameraHasFlash = _camera->exposure ()->isFlashModeSupported (QCameraExposure::FlashOn);
  emit cameraHasFlashChanged ();

  QStringList menu;
  _cameraHasAutoFlash = _camera->exposure ()->isFlashModeSupported (QCameraExposure::FlashAuto);
  if (_cameraHasAutoFlash) {
    menu.append ("Auto");//menu.append (tr ("Auto"));
  }
  menu.append ("Off");//  menu.append (tr ("Off"));
  if (_cameraHasFlash) {
    menu.append ("On");//menu.append (tr ("On"));
  }
  _flashModel = QVariant::fromValue (menu);
  emit flashModelChanged ();

  if ((!_cameraHasFlash && Off != flashMode()) || (!_cameraHasAutoFlash && Auto == flashMode()))
    setFlashMode(Off);
  else
    setFlashMode(_settings->flashMode());
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
#ifdef SHOW_DEBUG
  qDebug () << "Camera lock changed: " << status << " (Reason " << reason << ")";
#endif
}

void
ViewFinder::takePhoto ()
{
  QString filename = generateTemporaryImageFilename();

#ifdef SHOW_DEBUG
  qDebug () << "Filename: " << filename;
#endif

  // FIXME: Use toUTF8?
  //_imageCapture->capture (filename.toAscii ());
  photoThread.takePhoto(filename,_imageCapture);

  // save orientation here as device can be rotated during long snapshot operation
  _settings->setLastCapturedPhotoOrientation( _lastPhotoOrientation = currentOrientation());
}

void
ViewFinder::imageCaptured (int id, const QImage &preview)
{
  Q_UNUSED(preview)
#ifdef SHOW_DEBUG
  qDebug () << "Image captured: " << id;
#endif

  //emit imageCapturedSig ();
}

void ViewFinder::completeImage ()
{
    QString filename = _futureWatcher.future().result();
#ifdef SHOW_DEBUG
  qDebug () << "Image completed: " << filename;
#endif

  setImageLocation(filename);

  //_cameraService->emitImageCaptured (filename);
}

void
ViewFinder::imageSaved (int id, const QString &filename)
{

    emit imageCapturedSig();

    QString realFileName = generateImageFilename();
#ifdef SHOW_DEBUG
  qDebug () << "Image saved: " << id << " - " << filename;
#endif

  QFuture<QString> future = QtConcurrent::run(addGeoTag, filename, realFileName, m_lastCoordinate, currentOrientation(), (currentCamera() == (cameraCount()-1)) );
  _futureWatcher.setFuture(future);

}

void
ViewFinder::imageCaptureError (int id,
                               QCameraImageCapture::Error error,
                               const QString &message)
{
  if (error == QCameraImageCapture::OutOfSpaceError) {
    emit noSpaceOnDevice ();
  }

#ifdef SHOW_DEBUG
  qDebug () << "Image error: " << id << " - " << message << "(" << error << ")";
#endif
}

void
ViewFinder::imageReadyForCaptureChanged (bool ready)
{
#ifdef SHOW_DEBUG
  qDebug () << "Image ready for capture: " << (ready ? "true" : "false");
#endif
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
  if (_currentCamera == 1 && Off != mode) {
    return;
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

#ifdef SHOW_DEBUG
  qDebug () << "Setting flash to " << mode << "(" << m << ")";
#endif

  _settings->setFlashMode ((ViewFinder::FlashMode) mode);
  _camera->exposure ()->setFlashMode (m);
  emit (flashModeChanged ());
}

int
ViewFinder::flashMode ()
{
  return _settings->flashMode ();
}

bool
ViewFinder::changeCamera ()
{
  int nextCamera = _currentCamera + 1;

  if (nextCamera >= _cameraCount) {
    nextCamera = 0;
  }

#ifdef SHOW_DEBUG
  qDebug () << "Switching from camera " << _currentCamera << " to " << nextCamera;
#endif

  if (nextCamera == _currentCamera) {
    // If we're still on the same camera then we don't need to do anything
    return true;
  }

  QByteArray strDev;
  if (setCamera (strDev = QCamera::availableDevices ()[nextCamera]) == true) {
      _settings->setCameraDevice(strDev);
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
#ifdef SHOW_DEBUG
  qDebug () << "Setting capture mode: " << mode;
#endif

  if (!_camera)
      return;

  switch (mode) {
  case ViewFinder::Still:
#ifdef SHOW_DEBUG
    qDebug () << "Image capture";
#endif
    _camera->setCaptureMode (QCamera::CaptureStillImage);
    break;

  case ViewFinder::Video:
#ifdef SHOW_DEBUG
    qDebug () << "Video capture";
#endif
    _camera->setCaptureMode (QCamera::CaptureVideo);
    break;

  default:
#ifdef SHOW_DEBUG
    qDebug () << "Unknown capture mode";
#endif
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
  if (!_canFocus || _maxZoom <= 1) // return if zoom is unsupported
      return;

  _zoom = z;
  _camera->focus ()->zoomTo (1.0 + (z * _camera->focus ()->maximumOpticalZoom ()), 1.0);

#ifdef SHOW_DEBUG
  qDebug () << "Setting zoom to:" << _zoom;
#endif
  emit zoomChanged ();
}

void
ViewFinder::startRecording ()
{
  QString filename = generateVideoFilename ();
  QUrl url;
#ifdef SHOW_DEBUG
  qDebug () << "Starting recording" << filename;
#endif

  url = QUrl::fromLocalFile (filename);
  _currentLocation = url.toString ();
  emit capturedVideoLocationChanged();
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
#ifdef SHOW_DEBUG
  qDebug () << "Ending recording";
#endif

  _freeSpaceCheckTimer->stop ();
  delete _freeSpaceCheckTimer;
  _freeSpaceCheckTimer = 0;

  setRecording (false);
  _mediaRecorder->stop ();
  //_camera->unlock ();

  _thumbnailer->requestThumbnail (_currentLocation, mimetype);
}

QString
ViewFinder::generateTemporaryImageFilename () const
{
  return QString("/var/tmp/%1").arg(generateBaseImageFilename ());
}

QString
ViewFinder::generateImageFilename () const
{
  QString path(picturesDir() + '/');

  return path.append(generateBaseImageFilename ());
}

QString
ViewFinder::generateBaseImageFilename () const
{
  QDateTime now = QDateTime::currentDateTime ();

  return now.toString ("yyyy.MM.dd-hh.mm.ss'.jpg'");
}

QString
ViewFinder::generateVideoFilename () const
{
  QString path(videosDir() + '/');
  QDateTime now = QDateTime::currentDateTime ();

  return path.append (now.toString ("yyyy.MM.dd-hh.mm.ss")).append (".%1").arg(_videoFilenameExtension);
}

void
ViewFinder::mediaRecorderStateChanged (QMediaRecorder::State state)
{
#ifdef SHOW_DEBUG
  qDebug () << "Media recorder state changed: " << state;
#endif
}

void
ViewFinder::mediaRecorderError (QMediaRecorder::Error error)
{
  if (error == QMediaRecorder::ResourceError) {
    // We don't get a more specific error so it might be out of space
    checkSpace ();
  }
#ifdef SHOW_DEBUG
  qDebug () << "Media recorder error: " << _mediaRecorder->errorString ();
#endif
}

void
ViewFinder::mediaRecorderDurationChanged (qint64 duration)
{
#ifdef SHOW_DEBUG
  qDebug () << "Duration: " << duration;
#endif
  setDuration (duration);
}

QString ViewFinder::durationString()
{
    qint64 val = _duration / 1000;
    return QObject::tr("%1:%2:%3").arg((val / 3600), 0).arg(((val % 3600) / 60), 2, 'g', -1, '0').arg((val % 60), 2, 'g', -1, '0');
}

void
ViewFinder::repositionViewFinder (const QRectF &geometry)
{
  QSizeF size = _viewFinder->size ();
#ifdef SHOW_DEBUG
  qDebug () << "Viewfinder size: " << size.width () << "x" << size.height ();
#endif

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
#ifdef SHOW_DEBUG
  qDebug () << "Geometry changed: " << oldGeometry.width () << "x" << oldGeometry.height () << " -> " << newGeometry.width () << "x" << newGeometry.height ();
#endif

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
#ifdef SHOW_DEBUG
    qDebug () << " Error stating filesystem";
#endif
    emit noSpaceOnDevice ();
    return;
  }
#ifdef SHOW_DEBUG
  qDebug () << "Free space: " << buf.f_bsize * buf.f_bavail << "(" <<
    buf.f_bavail << " * " << buf.f_bsize << ")";
#endif

  // Limit to 5mb space
  if (buf.f_bsize * buf.f_bavail < (1024 * 1024 * 5)) {
#ifdef SHOW_DEBUG
    qDebug () << "No free space";
#endif
    emit noSpaceOnDevice ();
    return;
  }
}

void
ViewFinder::thumbnailCreated (const QString &url,
                              const QString &md5sum)
{
  Q_UNUSED(url)
  _settings->setLastCapturedPhotoOrientation(1); // no rotation

  // find a thumbnail - need to look into dir as we don't know file extention
  QString strDir(QString("%1/.thumbnails/normal").arg (QDir::homePath ()));
  QDir d(strDir);
  QStringList list(d.entryList (QStringList() << (md5sum + ".*")));
  setImageLocation(!list.isEmpty() ? strDir + '/' + list[0] : QString::null);
#ifdef SHOW_DEBUG
  qDebug () << "Thumbnail completed: " << _imageLocation;
#endif
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
#ifdef SHOW_DEBUG
  qDebug () << "Metadata available: " << avail;
#endif
}

void
ViewFinder::enterStandbyMode ()
{
  // check that camera is started and the application actualy lost focus
  if (_camera && _started && 0 == qApp->activeWindow ()) {
    _camera->stop ();
    _started = false;
  }
}

void
ViewFinder::leaveStandbyMode ()
{
  // don't start camera if it's already started
  if (_camera && !_started && 0 != qApp->activeWindow ()) {
    _camera->start ();
    _started = true;
  }
}

void
ViewFinder::setCurrentOrientation(int orientation)
{
    _currentOrientation = orientation;
    emit currentOrientationChanged();
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

void ViewFinder::setImageLocation(const QString & _str) { _imageLocation = _str; emit imageLocationChanged(); _settings->setLastCapturedPhotoPath(_imageLocation);}

//QML_DECLARE_TYPE(ViewFinder);
