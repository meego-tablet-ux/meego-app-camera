#ifndef JPEGEXIFERIZER_H
#define JPEGEXIFERIZER_H

#include "exifdatafactory.h"
#include <QString>

class JpegExiferizer
{
public:
    JpegExiferizer(const QString &inFile, const QString &outFile);
    virtual ~JpegExiferizer();

    void setExifDataFactory(ExifDataFactory *factory);

    bool doIt();

private:
    const QString &m_inFile;
    const QString &m_outFile;
    ExifDataFactory *m_factory;
};

#endif // JPEGEXIFERIZER_H
