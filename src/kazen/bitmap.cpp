#include <kazen/bitmap.h>

#define KAZEN_BITMAP_CHANNEL_COUNT 3

NAMESPACE_BEGIN(kazen)

Bitmap::Bitmap(const ScalarVector2i &size) : m_size(size) {
    if (!m_data) {
        m_data = std::unique_ptr<uint8_t[]>(new uint8_t[bufferSize()]);
    }
    clear();
}

void Bitmap::clear() {
    memset(m_data.get(), 0, bufferSize());
}

size_t Bitmap::bufferSize() {
    return m_size[0] * m_size[1] * KAZEN_BITMAP_CHANNEL_COUNT;
}

NAMESPACE_END(kazen)