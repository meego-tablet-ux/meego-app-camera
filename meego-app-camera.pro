VERSION = 0.8.0
TEMPLATE = subdirs
SUBDIRS += plugin

QML_FILES = *.qml

OTHER_FILES += $${QML_FILES}

qmlfiles.files += $${QML_FILES}
qmlfiles.path += $$INSTALL_ROOT/usr/share/$$TARGET

INSTALLS += qmlfiles

PROJECT_NAME = meego-app-camera

dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION} &&
dist.commands += git clone . $${PROJECT_NAME}-$${VERSION} &&
dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION}/.git &&
dist.commands += mkdir -p $${PROJECT_NAME}-$${VERSION}/ts &&
dist.commands += lupdate $${PROJECT_NAME}-$${VERSION} -ts $${PROJECT_NAME}-$${VERSION}/ts/$${PROJECT_NAME}.ts &&
dist.commands += tar jcpvf $${PROJECT_NAME}-$${VERSION}.tar.bz2 $${PROJECT_NAME}-$${VERSION} &&
dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION}
QMAKE_EXTRA_TARGETS += dist
