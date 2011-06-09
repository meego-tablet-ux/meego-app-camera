#include "exifdatafactory.h"

#include <libexif/exif-entry.h>
#include <libexif/exif-tag.h>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <math.h>

ExifDataFactory::ExifDataFactory(const QGeoCoordinate &coord, int orientation, bool frontFacingCamera) :
    m_coord(coord), m_serialized_data(0), m_serialized_data_size(0)
{
    m_mem = exif_mem_new_default();
    m_data = exif_data_new();
    m_order = exif_data_get_byte_order(m_data);

    //if orientation not in range do nothing
    if(orientation >= 0 && orientation < 4)
    {
        ExifContent *orientationContent = exif_content_new();
        m_data->ifd[EXIF_IFD_0] = orientationContent;
        ExifEntry *orientationEntry = exif_entry_new();
        orientationEntry->tag = (ExifTag)EXIF_TAG_ORIENTATION;
        orientationEntry->components = 1;
        orientationEntry->format = EXIF_FORMAT_SHORT;
        orientationEntry->size = exif_format_get_size(orientationEntry->format);
        orientationEntry->data = (unsigned char*)exif_mem_alloc(m_mem, orientationEntry->size);

        ExifShort val;
        if(frontFacingCamera)
        {
            switch(orientation)
            {
            case 1: val=2; break;
            case 0: val=7; break;
            case 3: val=4; break;
            case 2: val=5; break;
            default: val = 2;
            }
        }
        else
        {
            switch(orientation)
            {
            case 1: val=1; break;
            case 0: val=8; break;
            case 3: val=3; break;
            case 2: val=6; break;
            default: val = 1;
            }
        }

        exif_set_short(orientationEntry->data,m_order,val);
        exif_content_add_entry(orientationContent,orientationEntry);
    }

    if (m_coord.isValid()) {
        m_content = exif_content_new();

        m_data->ifd[EXIF_IFD_GPS] = m_content;
        createGPSVersionEntry();
        if (m_coord.type() == QGeoCoordinate::Coordinate3D) {
            createGPSAltitudeRefEntry();
            createGPSAltitudeEntry();
        }
        if (m_coord.type() != QGeoCoordinate::InvalidCoordinate) {
            createGPSLatitudeRefEntry();
            createGPSLatitudeEntry();
            createGPSLongitudeRefEntry();
            createGPSLongitudeEntry();
        }
    }
}

ExifDataFactory::~ExifDataFactory()
{
    if (m_serialized_data) {
        exif_mem_free(m_mem, (void*)m_serialized_data);
        m_serialized_data_size = 0;
    }
    exif_data_unref(m_data);
    exif_mem_unref(m_mem);
}

QByteArray ExifDataFactory::data()
{
    if (m_serialized_data) {
        exif_mem_free(m_mem, (void*)m_serialized_data);
        m_serialized_data_size = 0;
    }
    exif_data_save_data(m_data, &m_serialized_data, &m_serialized_data_size);
    return QByteArray::fromRawData((const char*)m_serialized_data, m_serialized_data_size);
}

void ExifDataFactory::dmsStringToComponents(const QString &string, int &degrees, int &minutes, double &seconds) const
{
    QStringList parts = string.split(QRegExp(".\\s"));
    degrees = qAbs(parts[0].toInt());
    minutes = parts[1].toInt();
    seconds = parts[2].toDouble();
}

void ExifDataFactory::createGPSVersionEntry()
{
    ExifEntry *entry;

    entry = exif_entry_new();
    entry->tag = (ExifTag)EXIF_TAG_GPS_VERSION_ID;
    entry->components = 4;
    entry->format = EXIF_FORMAT_BYTE;
    entry->size = exif_format_get_size(entry->format) * entry->components;
    entry->data = (unsigned char*)exif_mem_alloc(m_mem, entry->size);
    ((uint8_t*)(entry->data))[0] = 2;
    ((uint8_t*)(entry->data))[1] = 2;
    ((uint8_t*)(entry->data))[2] = 0;
    ((uint8_t*)(entry->data))[3] = 0;
    exif_content_add_entry(m_content, entry);
}

void ExifDataFactory::createGPSAltitudeRefEntry()
{
    ExifEntry *entry;

    entry = exif_entry_new();
    entry->tag = (ExifTag)EXIF_TAG_GPS_ALTITUDE_REF;
    entry->components = 1;
    entry->format = EXIF_FORMAT_BYTE;
    entry->size = exif_format_get_size(entry->format);
    entry->data = (unsigned char*)exif_mem_alloc(m_mem, entry->size);
    if (m_coord.altitude() < 0) {
      *((uint8_t*)(entry->data)) = 1;
    }
    exif_content_add_entry(m_content, entry);
}

void ExifDataFactory::createGPSAltitudeEntry()
{
    ExifEntry *entry;
    ExifRational r;

    entry = exif_entry_new();
    entry->tag = (ExifTag)EXIF_TAG_GPS_ALTITUDE;
    entry->components = 1;
    entry->format = EXIF_FORMAT_RATIONAL;
    entry->size = exif_format_get_size(EXIF_FORMAT_RATIONAL);
    entry->data = (unsigned char*)exif_mem_alloc(m_mem, entry->size);
    r.denominator = 10;
    r.numerator = trunc(m_coord.altitude() * 10);
    exif_set_rational(entry->data, m_order, r);
    exif_content_add_entry(m_content, entry);
}

void ExifDataFactory::createGPSLatitudeRefEntry()
{
    ExifEntry *entry;

    entry = exif_entry_new();
    entry->tag = (ExifTag)EXIF_TAG_GPS_LATITUDE_REF;
    entry->components = 2;
    entry->format = EXIF_FORMAT_ASCII;
    entry->size = exif_format_get_size(entry->format) * entry->components;
    entry->data = (unsigned char*)exif_mem_alloc(m_mem, entry->size);
    if (m_coord.latitude() > 0) {
        strncpy((char*)(entry->data), "N", entry->size);
    } else {
        strncpy((char*)(entry->data), "S", entry->size);
    }
    exif_content_add_entry(m_content, entry);
}

void ExifDataFactory::createGPSLongitudeRefEntry()
{
    ExifEntry *entry;

    entry = exif_entry_new();
    entry->tag = (ExifTag)EXIF_TAG_GPS_LONGITUDE_REF;
    entry->components = 2;
    entry->format = EXIF_FORMAT_ASCII;
    entry->size = exif_format_get_size(entry->format) * entry->components;
    entry->data = (unsigned char*)exif_mem_alloc(m_mem, entry->size);
    if (m_coord.longitude() > 0) {
        strncpy((char*)(entry->data), "E", entry->size);
    } else {
        strncpy((char*)(entry->data), "W", entry->size);
    }
    exif_content_add_entry(m_content, entry);
}


void ExifDataFactory::addDMSToEntry(ExifEntry *entry, const QString &dmsString) const
{
    ExifRational r;
    int degrees;
    int minutes;
    double seconds;

    Q_ASSERT(entry);
    dmsStringToComponents(dmsString, degrees, minutes, seconds);

    r.numerator = degrees;
    r.denominator = 1;
    exif_set_rational(entry->data + exif_format_get_size(entry->format) * 0,
                      m_order, r);

    r.numerator = minutes;
    r.denominator = 1;
    exif_set_rational(entry->data + exif_format_get_size(entry->format) * 1,
                      m_order, r);
    exif_content_add_entry(m_content, entry);

    r.numerator = seconds * 10;
    r.denominator = 10;
    exif_set_rational(entry->data + exif_format_get_size(entry->format) * 2,
                      m_order, r);
}

void ExifDataFactory::createGPSLatitudeEntry()
{
    ExifEntry *entry;

    entry = exif_entry_new();
    entry->tag = (ExifTag)EXIF_TAG_GPS_LATITUDE;
    entry->components = 3;
    entry->format = EXIF_FORMAT_RATIONAL;
    entry->size = exif_format_get_size(entry->format) * entry->components;
    entry->data = (unsigned char*)exif_mem_alloc(m_mem, entry->size);

    QStringList dmsTuples = m_coord.toString(QGeoCoordinate::DegreesMinutesSeconds).split(", ");
    addDMSToEntry(entry, dmsTuples[0]);

    exif_content_add_entry(m_content, entry);
}

void ExifDataFactory::createGPSLongitudeEntry()
{
    ExifEntry *entry;

    entry = exif_entry_new();
    entry->tag = (ExifTag)EXIF_TAG_GPS_LONGITUDE;
    entry->components = 3;
    entry->format = EXIF_FORMAT_RATIONAL;
    entry->size = exif_format_get_size(entry->format) * entry->components;
    entry->data = (unsigned char*)exif_mem_alloc(m_mem, entry->size);

    QStringList dmsTuples = m_coord.toString(QGeoCoordinate::DegreesMinutesSeconds).split(", ");
    addDMSToEntry(entry, dmsTuples[1]);

    exif_content_add_entry(m_content, entry);
}
