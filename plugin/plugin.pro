TEMPLATE = lib
TARGET = Camera
QT += declarative dbus
CONFIG += qt plugin mobility dbus link_pkgconfig
MOBILITY += multimedia location
PKGCONFIG += gconf-2.0 libexif

QMAKE_LFLAGS += -ljpeg

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = $$TARGET
OBJECTS_DIR = .obj
MOC_DIR = .moc
#DEFINES += SHOW_DEBUG

# Input
SOURCES += 			\
	cameraifadaptor.cpp	\
	components.cpp 		\
	roundedimage.cpp	\
	settings.cpp		\
	thumbnailer.cpp		\
	viewfinder.cpp		\
	zoomarea.cpp            \
        exifdatafactory.cpp     \
        jpegexiferizer.cpp \
        shutterslice.cpp \
        shutteranimationcomponent.cpp \
        capturephotothread.cpp

HEADERS += 			\
	cameraifadaptor.h	\
	cameraservice.h		\
        components.h		\
	roundedimage.h		\
	settings.h		\
	thumbnailer.h		\
	viewfinder.h		\
        zoomarea.h              \
        exifdatafactory.h       \
        jpegexiferizer.h \
        shutterslice.h \
        shutteranimationcomponent.h \
        capturephotothread.h

OTHER_FILES += \
    Camera/qmldir

qmldir.files += $$TARGET
qmldir.path += $$[QT_INSTALL_IMPORTS]/MeeGo/App
INSTALLS += qmldir
