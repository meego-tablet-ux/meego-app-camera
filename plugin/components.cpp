/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <glib-object.h>
#include <QDir>

#include "components.h"
#include "launcher.h"
#include "roundedimage.h"
#include "viewfinder.h"

components::components ()
{
  // Initialise the GObject type system
  g_type_init ();

  if (QDir::home ().mkpath ("Pictures/Camera") == false) {
    qDebug () << "Error making camera directory: " << QDir::homePath () << "/Pictures/Camera";
  }

  if (QDir::home ().mkpath ("Videos/Camera") == false) {
    qDebug () << "Error making camera directory: " << QDir::homePath () << "/Pictures/Camera";
  }
}

void components::registerTypes(const char *uri)
{
  qmlRegisterType<Launcher>(uri, 0, 1, "Launcher");
  qmlRegisterType<RoundedImage>(uri, 0, 1, "RoundedImage");
  qmlRegisterType<ViewFinder>(uri, 0, 1, "ViewFinder");
}

Q_EXPORT_PLUGIN(components);
