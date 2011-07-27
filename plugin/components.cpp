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
#include "roundedimage.h"
#include "viewfinder.h"
#include "shutteranimationcomponent.h"
#include "qmlpixmap.h"

components::components ()
{
  // Initialise the GObject type system
  g_type_init ();
}

void components::registerTypes(const char *uri)
{
  qmlRegisterType<RoundedImage>(uri, 0, 1, "RoundedImage");
  qmlRegisterType<ViewFinder>(uri, 0, 1, "ViewFinder");
  qmlRegisterType<ShutterAnimationComponent>(uri, 0, 1, "ShutterAnimationComponent");
  qmlRegisterType<QmlPixmap>(uri, 0, 1, "QmlPixmap");
}


Q_EXPORT_PLUGIN(components);
