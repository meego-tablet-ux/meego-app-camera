#ifndef EXIFDATAFACTORY_H
#define EXIFDATAFACTORY_H

#include <QtLocation/QGeoCoordinate>
#include <QByteArray>
#include <libexif/exif-data.h>
#include <libexif/exif-mem.h>
#include <libexif/exif-content.h>

QTM_USE_NAMESPACE

class ExifDataFactory
{
public:
    ExifDataFactory(const QGeoCoordinate &coord, int orientation);
    virtual ~ExifDataFactory();
    QByteArray data();

private:
    void dmsStringToComponents(const QString &string, int &degrees, int &minutes, double &seconds) const;
    void addDMSToEntry(ExifEntry *entry, const QString &dmsString) const;
    void createGPSVersionEntry();
    void createGPSAltitudeRefEntry();
    void createGPSAltitudeEntry();
    void createGPSLatitudeRefEntry();
    void createGPSLongitudeRefEntry();
    void createGPSLatitudeEntry();
    void createGPSLongitudeEntry();

    const QGeoCoordinate &m_coord;
    ExifData *m_data;
    ExifMem *m_mem;
    ExifByteOrder m_order;
    ExifContent *m_content;
    unsigned char *m_serialized_data;
    unsigned int m_serialized_data_size;

    ExifDataFactory(const ExifDataFactory &other);
};

#endif // EXIFDATAFACTORY_H
