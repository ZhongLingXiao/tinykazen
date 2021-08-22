#include <kazen/bitmap.h>
#include <OpenImageIO/imageio.h>

#define KAZEN_BITMAP_CHANNEL_COUNT 3

NAMESPACE_BEGIN(kazen)

Bitmap::Bitmap(const ScalarVector2i &size, ScalarFloat *data) : m_size(size) {
    m_data = std::unique_ptr<ScalarFloat[]>(new ScalarFloat[bufferSize()]);
    if (!m_data) {
        memcpy(m_data.get(), data, bufferSize());
    } else {
        clear();
    }
}

void Bitmap::clear() {
    memset(m_data.get(), 0, bufferSize());
}

size_t Bitmap::bufferSize() const {
    return pixelCount() * KAZEN_BITMAP_CHANNEL_COUNT * sizeof(ScalarFloat);
}

void Bitmap::saveEXR(const std::string &filename) {
    // TODO:

}

void Bitmap::savePNG(const std::string &filename) {
    std::cout << "Writing a " << m_size[0] << "x" << m_size[1]
         << " PNG file to \"" << filename << "\"" << std::endl;

    std::string path = filename + ".png";

    uint8_t *rgb8 = new uint8_t[3 * pixelCount()];
    uint8_t *dst = rgb8;
    for (int i = 0; i <  m_size.x(); ++i) {
        for (int j = 0; j <  m_size.y(); ++j) {
            auto index = i * m_size.x() + m_size.y();
            dst[0] = (uint8_t) std::clamp(255.f * enoki::linear_to_srgb(m_data[index]), 0.f, 255.f);
            dst[1] = (uint8_t) std::clamp(255.f * enoki::linear_to_srgb(m_data[index+1]), 0.f, 255.f);
            dst[2] = (uint8_t) std::clamp(255.f * enoki::linear_to_srgb(m_data[index+2]), 0.f, 255.f);
            dst += 3;
        }
    }

    std::unique_ptr<OIIO::ImageOutput> out = OIIO::ImageOutput::create(path);
    if (!out)
        return;
    OIIO::ImageSpec spec(m_size[0], m_size[1], KAZEN_BITMAP_CHANNEL_COUNT, OIIO::TypeDesc::UINT8);
    out->open(path, spec);
    out->write_image(OIIO::TypeDesc::UINT8, rgb8);
    out->close ();
    delete[] rgb8;
}

NAMESPACE_END(kazen)