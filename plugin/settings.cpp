/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDebug>
#include <glib.h>

#include "settings.h"

#define FLASHMODE_KEY "/meego/app/camera/flashmode"
#define CAPTUREMODE_KEY "/meego/app/camera/capturemode"
#define CAMERADEVICE_KEY "/meego/app/camera/device"

// Private undocumented keys
#define VIDEO_FPS_KEY "/meego/app/camera/video-fps"
#define VIDEO_RESOLUTION_WIDTH_KEY "/meego/app/camera/video-resolution-width"
#define VIDEO_RESOLUTION_HEIGHT_KEY "/meego/app/camera/video-resolution-height"

Settings::Settings () :
  _flashMode (ViewFinder::Auto),
  _captureMode (ViewFinder::Still),
  _videoFPS(0),
  _videoWidth(0),
  _videoHeight(0),
  _client (gconf_client_get_default ())
{
  GError *error = NULL;

  _flashMode = (enum ViewFinder::FlashMode) gconf_client_get_int
    (_client, FLASHMODE_KEY, &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error getting flashmode:" << error->message;
#endif
    g_error_free (error);
    error = NULL;
  }

  _captureMode = (enum ViewFinder::CaptureMode) gconf_client_get_int
    (_client, CAPTUREMODE_KEY, &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error getting capturemode:" << error->message;
#endif
    g_error_free (error);
    error = NULL;
  }

  gchar* s = gconf_client_get_string (_client, CAMERADEVICE_KEY, &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error getting camera device:" << error->message;
#endif
    g_error_free (error);
    error = NULL;
  }
  else {
    _strCameraDevice = s;
    g_free(s);
  }

  _videoWidth = gconf_client_get_int
    (_client, VIDEO_RESOLUTION_WIDTH_KEY, &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error getting video width:" << error->message;
#endif
    g_error_free (error);
    error = NULL;
  }

  _videoHeight = gconf_client_get_int
    (_client, VIDEO_RESOLUTION_HEIGHT_KEY, &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error getting video height:" << error->message;
#endif
    g_error_free (error);
    error = NULL;
  }

  _videoFPS = gconf_client_get_float (_client, VIDEO_FPS_KEY, &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error getting video fps:" << error->message;
#endif
    g_error_free (error);
    error = NULL;
  }

#ifdef SHOW_DEBUG
  qDebug () << "Settings*************";
  qDebug () << "Flash Mode: " << _flashMode;
  qDebug () << "Capture Mode: " << _captureMode;
#endif
}

Settings::~Settings ()
{
}

void
Settings::setFlashMode (enum ViewFinder::FlashMode fm)
{
  GError *error = NULL;

  gconf_client_set_int (_client, FLASHMODE_KEY, fm, &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error setting flashmode:" << error->message;
#endif
    g_error_free (error);
  }

  _flashMode = fm;
}

void
Settings::setCaptureMode (enum ViewFinder::CaptureMode cm)
{
  GError *error = NULL;

  gconf_client_set_int (_client, CAPTUREMODE_KEY, cm, &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error setting capturemode:" << error->message;
#endif
    g_error_free (error);
  }

  _captureMode = cm;
}

void
Settings::setCameraDevice(const QByteArray & _str)
{
  GError *error = NULL;

  gconf_client_set_string (_client, CAMERADEVICE_KEY, _str.data(), &error);
  if (error != NULL) {
#ifdef SHOW_DEBUG
    qDebug () << "Error setting camear device:" << error->message;
#endif
    g_error_free (error);
  }

  _strCameraDevice = _str;
}


