/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef ZOOMAREA_H
#define ZOOMAREA_H

#include <QEvent>
#include <QGraphicsWidget>

class ZoomArea : public QGraphicsWidget
{
    Q_OBJECT;

    Q_PROPERTY (qreal scaleFactor READ scaleFactor NOTIFY scaleFactorChanged);
    qreal scaleFactor () { return _scaleFactor; }

  signals:
    void scaleFactorChanged ();

  public:
    ZoomArea (QGraphicsItem *_parent = 0);
    ~ZoomArea ();

    bool sceneEvent (QEvent *event);

    qreal _scaleFactor;
};

#endif
