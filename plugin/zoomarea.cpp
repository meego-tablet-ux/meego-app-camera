/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDebug>
#include <QtDeclarative/qdeclarative.h>

#include "zoomarea.h"

ZoomArea::ZoomArea (QGraphicsItem *_parent) :
  QGraphicsWidget (_parent),
  _scaleFactor (1)
{
  qDebug () << "Created zoom area";
  setAcceptTouchEvents (true);
}

ZoomArea::~ZoomArea ()
{
}

bool
ZoomArea::sceneEvent (QEvent *event)
{
  if (event->type () == QEvent::TouchBegin ||
      event->type () == QEvent::TouchEnd ||
      event->type () == QEvent::TouchUpdate) {
    QList<QTouchEvent::TouchPoint> touchPoints =
      static_cast<QTouchEvent *> (event)->touchPoints ();

    if (touchPoints.count () >= 2) {
      const QTouchEvent::TouchPoint &tp1 = touchPoints[0];
      const QTouchEvent::TouchPoint &tp2 = touchPoints[1];

      _scaleFactor *= (QLineF (tp1.pos (), tp2.pos ()).length () /
                       QLineF (tp1.startPos (), tp2.startPos ()).length ());

      emit scaleFactorChanged ();
    }
  }

  return QGraphicsWidget::sceneEvent (event);
}

QML_DECLARE_TYPE (ZoomArea);
