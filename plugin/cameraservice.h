/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef _CAMERA_SERVICE_H_
#define _CAMERA_SERVICE_H_

#include <QObject>

class CameraService : public QObject
{
    Q_OBJECT;

  public:
    CameraService (QObject *parent = 0): QObject(parent) {}
    void emitImageCaptured (const QString &filename) {
        emit Captured (filename);
    }

  Q_SIGNALS:
    void Captured (const QString &filename);
};

#endif
