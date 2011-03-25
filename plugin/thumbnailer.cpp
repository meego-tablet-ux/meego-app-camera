/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "thumbnailer.h"

#define TUMBLER_SERVICE       "org.freedesktop.thumbnails.Thumbnailer1"
#define TUMBLER_PATH          "/org/freedesktop/thumbnails/Thumbnailer1"
#define TUMBLER_INTERFACE     "org.freedesktop.thumbnails.Thumbnailer1"
#define DEFAULT_FLAVOR        "normal"
#define DEFAULT_SCHEDULER     "foreground"

Thumbnailer::Thumbnailer (QObject *_parent)
{
  Q_UNUSED (_parent);

  _tumblerInterface = new QDBusInterface (TUMBLER_SERVICE,
                                          TUMBLER_PATH,
                                          TUMBLER_INTERFACE);
  QDBusConnection::sessionBus ().connect
    (TUMBLER_SERVICE, TUMBLER_PATH, TUMBLER_INTERFACE,
     "Ready", this,
     SLOT (tumblerReady (const unsigned int, const QStringList)));
  QDBusConnection::sessionBus ().connect
    (TUMBLER_SERVICE, TUMBLER_PATH, TUMBLER_INTERFACE,
     "Error", this,
     SLOT (tumblerError (const unsigned int, const QStringList,
                         const int, const QString)));
  QDBusConnection::sessionBus ().connect
    (TUMBLER_SERVICE, TUMBLER_PATH, TUMBLER_INTERFACE,
     "Finished", this,
     SLOT (tumblerFinished (const unsigned int)));
}

Thumbnailer::~Thumbnailer ()
{
  delete _tumblerInterface;
}

void
Thumbnailer::requestThumbnail (QString &url,
                               QString &mimetype)
{
  QStringList uris, mimetypes;
  quint32 handle = 0;

  uris << url;
  mimetypes << mimetype;

  _tumblerInterface->asyncCall (QLatin1String ("Queue"), uris, mimetypes,
                                DEFAULT_FLAVOR, DEFAULT_SCHEDULER, handle);
}

void
Thumbnailer::tumblerFinished (const unsigned int &handle)
{
  Q_UNUSED (handle);
}

void
Thumbnailer::tumblerReady (const unsigned int &handle,
                           const QStringList  &urls)
{
  Q_UNUSED (handle);
  QByteArray h = QCryptographicHash::hash (urls[0].toUtf8 (),
                                           QCryptographicHash::Md5);
  QString md5 = QString (h.toHex ().constData ());
  emit created (urls[0], md5);
}

void
Thumbnailer::tumblerError (const unsigned int &handle,
                           const QStringList  &urls,
                           const int          &errorCode,
                           const QString      &message)
{
  Q_UNUSED (handle);
  emit error (urls, errorCode, message);
}
