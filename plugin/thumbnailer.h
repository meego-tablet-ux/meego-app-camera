/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef __THUMBNAILER_H__
#define __THUMBNAILER_H__

#include <QtCore/QtCore>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>

class Thumbnailer : public QObject
{
    Q_OBJECT;

  public:
    Thumbnailer (QObject *_parent = 0);
    ~Thumbnailer ();

    void requestThumbnail (QString &url, QString &mimetype);

  Q_SIGNALS:
    void created (const QString &, const QString &);
    void error (const QStringList &, const int &, const QString &);

  private slots:
    void tumblerReady (const unsigned int &handle, const QStringList &urls);
    void tumblerFinished (const unsigned int &handle);
    void tumblerError (const unsigned int &handle, const QStringList &urls,
                       const int &errorCode, const QString &message);

  private:
    QDBusInterface *_tumblerInterface;
};

#endif
