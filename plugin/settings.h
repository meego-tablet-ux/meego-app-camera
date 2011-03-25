/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <QtCore>
#include <gconf/gconf-client.h>
#include "viewfinder.h"

class Settings : public QObject
{
  Q_OBJECT;

  public:
    Settings ();
    ~Settings ();

    enum ViewFinder::FlashMode flashMode () { return _flashMode; }
    void setFlashMode (enum ViewFinder::FlashMode fm);

    enum ViewFinder::CaptureMode captureMode () { return _captureMode; }
    void setCaptureMode (enum ViewFinder::CaptureMode cm);

    double videoFPS () { return _videoFPS; }
    int videoWidth () { return _videoWidth; }
    int videoHeight () { return _videoHeight; }

  private:
    enum ViewFinder::FlashMode _flashMode;
    enum ViewFinder::CaptureMode _captureMode;

    int _videoWidth, _videoHeight;
    double _videoFPS;

    GConfClient *_client;
};

#endif
