/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QImage>
#include <QPainter>
#include <QDeclarativeItem>

class RoundedImage : public QDeclarativeItem
{
    Q_OBJECT;

  public:
    RoundedImage (QDeclarativeItem *parent = 0);

    Q_PROPERTY (QString source READ source WRITE setSource NOTIFY sourceChanged);
    QString source () { return _source; }
    void setSource (QString &s);

    void paint (QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                QWidget *widget);

  Q_SIGNALS:
    void sourceChanged ();

  private:
    QString _source;
    QImage _image;
};
