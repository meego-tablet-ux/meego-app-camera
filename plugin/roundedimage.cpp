/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "roundedimage.h"

#include <QImageReader>

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

#ifdef CUT_CORNERS
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
#endif

void
RoundedImage::setSource (QString &s)
{
  if (s.isEmpty ()) {
    return;
  }

  QImageReader imageReader(s);
  QSize imageSize = imageReader.size();
  imageSize.scale(QSize(width(), height()), Qt::KeepAspectRatioByExpanding);
  imageReader.setScaledSize(imageSize);
  _image = imageReader.read();

  // Cut the corners off the image

  // We don't need to cut corners off anything with the new assets
  // 2011-02-03
  // cutCorners (_image);

  // Force a redraw
  update (0, 0, width(), height());
}

QML_DECLARE_TYPE(RoundedImage);
