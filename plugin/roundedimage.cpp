/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "roundedimage.h"

#define IMAGE_SIZE 56

RoundedImage::RoundedImage (QDeclarativeItem *_parent)
  : QDeclarativeItem (_parent)
{
  setFlag(QGraphicsItem::ItemHasNoContents, false);
}

void
RoundedImage::paint (QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)
  painter->drawImage (QPoint (0.0, 0.0), _image);
}

#define CORNER_SIZE 5
static void
cutCorners (QImage &image)
{
  // These are the numbers of pixels that we remove from the ends of the
  // first and last CORNER_SIZE rows.
  int corners[CORNER_SIZE] = {7, 5, 3, 2, 1};
  QRgb cx = qRgba (0, 0, 0, 0);

  for (int i = 0; i < CORNER_SIZE; i++) {
    QRgb *s1 = (QRgb *) image.scanLine (i);
    QRgb *s2 = (QRgb *) image.scanLine (image.height () - (i + 1));

    for (int j = 0; j < corners[i]; j++) {
      *(s1 + j) = cx;
      *(s1 + (image.width () - (j + 1))) = cx;

      *(s2 + j) = cx;
      *(s2 + (image.width () - (j + 1))) = cx;
    }
  }
}

void
RoundedImage::setSource (QString &s)
{
  QImage _tmp, _scaled;

  if (s.isEmpty ()) {
    return;
  }

  _tmp = QImage (s);
  if (_tmp.isNull ()) {
    qDebug () << "Error loading " << s;
    return;
  }

  int sw, sh;
  int x, y;

  if (_tmp.width () > _tmp.height ()) {
    float ratio = (float) IMAGE_SIZE / (float) _tmp.height ();

    sw = _tmp.width () * ratio;
    sh = IMAGE_SIZE;

    x = (sw - IMAGE_SIZE) / 2;
    y = 0;
  } else {
    float ratio = (float) IMAGE_SIZE / (float) _tmp.width ();

    sw = IMAGE_SIZE;
    sh = _tmp.height () * ratio;

    x = 0;
    y = (sh - IMAGE_SIZE) / 2;
  }

  qDebug () << "Resizing to: " << sw << "x" << sh;
  qDebug () << "Cropping to: " << x << "," << y;

  _scaled = _tmp.scaled (sw, sh, Qt::IgnoreAspectRatio,
                         Qt::SmoothTransformation);
  _tmp = _scaled.copy (x, y, IMAGE_SIZE, IMAGE_SIZE);
  _image = _tmp.convertToFormat (QImage::Format_ARGB32_Premultiplied);

  // Cut the corners off the image

  // We don't need to cut corners off anything with the new assets
  // 2011-02-03
  // cutCorners (_image);

  // Force a redraw
  update (0, 0, IMAGE_SIZE, IMAGE_SIZE);
}

QML_DECLARE_TYPE(RoundedImage);
