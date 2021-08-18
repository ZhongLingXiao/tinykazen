#pragma once

#include <kazen/common.h>
#include <kazen/vector.h>

NAMESPACE_BEGIN(kazen)

/**
 * \brief Stores a RGB high dynamic-range bitmap
 *
 * The bitmap class provides I/O support using the OpenEXR file format
 */
class Bitmap {
public:
    using Float = float;
    KAZEN_BASE_TYPES()    

    Bitmap(const ScalarVector2i &size, ScalarFloat *data = nullptr);

    /// Clear the bitmap to zero
    void clear();

    /// Save the bitmap as an EXR file with the specified filename
    void saveEXR(const std::string &filename);

    /// Save the bitmap as a PNG file (with sRGB tonemapping) with the specified filename
    void savePNG(const std::string &filename);

private:
    /// Return the bitmap size in bytes (excluding metadata)
    size_t bufferSize() const;    

private:
    ScalarVector2i m_size;
    std::unique_ptr<ScalarFloat[]> m_data;
};

NAMESPACE_END(kazen)


