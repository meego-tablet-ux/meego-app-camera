#include "jpegexiferizer.h"

#include <stdio.h>
#include <jpeglib.h>

JpegExiferizer::JpegExiferizer(const QString &inFile, const QString &outFile) :
    m_inFile(inFile), m_outFile(outFile)
{
}

JpegExiferizer::~JpegExiferizer()
{
}

void JpegExiferizer::setExifDataFactory(ExifDataFactory *factory)
{
    m_factory = factory;
}

bool JpegExiferizer::doIt()
{
    FILE* infile;
    FILE* outfile;
    struct jpeg_decompress_struct cinfo_i;
    struct jpeg_compress_struct cinfo_o;
    jvirt_barray_ptr* coefficients;
    struct jpeg_error_mgr jerr;

    if (!m_factory) {
        return false;
    }

    cinfo_i.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo_i);

    if ((infile = fopen(m_inFile.toLatin1().constData() , "rb")) == NULL) {
        return false;
    }
    jpeg_stdio_src(&cinfo_i, infile);
    jpeg_read_header(&cinfo_i, TRUE);
    coefficients = jpeg_read_coefficients(&cinfo_i);

    cinfo_o.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo_o);
    if ((outfile = fopen(m_outFile.toLatin1().constData(), "wb")) == NULL) {
      fclose(infile);
      return false;
    }
    jpeg_stdio_dest(&cinfo_o, outfile);

    jpeg_copy_critical_parameters(&cinfo_i, &cinfo_o);
    jpeg_write_coefficients(&cinfo_o, coefficients);

    QByteArray exifData = m_factory->data();

    jpeg_write_marker(&cinfo_o, JPEG_APP0 + 1,
                      (const JOCTET*)exifData.constData(), exifData.length());

    jpeg_finish_compress(&cinfo_o);
    jpeg_finish_decompress(&cinfo_i);

    fclose(outfile);
    fclose(infile);

    return true;
}
