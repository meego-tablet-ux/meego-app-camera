TEMPLATE = lib
TARGET = Camera
QT += declarative dbus
CONFIG += qt plugin mobility dbus link_pkgconfig
MOBILITY += multimedia
PKGCONFIG += gconf-2.0

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = $$TARGET
OBJECTS_DIR = .obj
MOC_DIR = .moc

# Input
SOURCES += 			\
	cameraifadaptor.cpp	\
	components.cpp 		\
	roundedimage.cpp	\
	settings.cpp		\
	thumbnailer.cpp		\
	viewfinder.cpp		\
	zoomarea.cpp
HEADERS += 			\
	cameraifadaptor.h	\
	cameraservice.h		\
	components.h		\
	launcher.h		\
	roundedimage.h		\
	settings.h		\
	thumbnailer.h		\
	viewfinder.h		\
	zoomarea.h

OTHER_FILES += \
    Camera/qmldir

qmldir.files += $$TARGET
qmldir.path += $$[QT_INSTALL_IMPORTS]/MeeGo/App
INSTALLS += qmldir
