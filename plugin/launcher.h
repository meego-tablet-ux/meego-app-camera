/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QProcess>
#include <QObject>
#include "uxdaemoninterface.h"

class Launcher : public QObject
{
    Q_OBJECT;

  public:
    Launcher (QObject *parent = 0): QObject(parent)
    {
        uxDaemonInterface = new com::lockstatus::query("com.lockstatus", "/query", QDBusConnection::sessionBus(), this);
    }

    Q_INVOKABLE void launch (QString desktopFilePath, QString cmd = "", QString cdata = "" ) {
        uxDaemonInterface->launchDesktopByName(desktopFilePath, cmd, cdata);
    }

private:
    com::lockstatus::query *uxDaemonInterface;
};
