#include <kazen/block.h>
#include <kazen/rfilter.h>
#include <tbb/tbb.h>

NAMESPACE_BEGIN(kazen)

ImageBlock::ImageBlock(const ScalarVector2i &size, const ReconstructionFilter *filter) : m_offset(0), m_size(0) {
    if (filter) {
        /* Tabulate the image reconstruction filter for performance reasons */
        m_filterRadius = filter->getRadius();
        m_borderSize = (int) std::ceil(m_filterRadius - 0.5f - 2.f * math::RayEpsilon<ScalarFloat>);
        m_filter = new ScalarFloat[KAZEN_FILTER_RESOLUTION + 1];
        for (int i=0; i<KAZEN_FILTER_RESOLUTION; ++i) {
            ScalarFloat pos = (m_filterRadius * i) / KAZEN_FILTER_RESOLUTION;
            m_filter[i] = enoki::scalar_cast(enoki::hmax(filter->eval(pos)));
        }
        m_filter[KAZEN_FILTER_RESOLUTION] = 0.0f;
        m_lookupFactor = KAZEN_FILTER_RESOLUTION / m_filterRadius;
        int weightSize = (int) std::ceil(2 * m_filterRadius) + 1;
        m_weightsX = new Float[2 * weightSize];
        m_weightsY = m_weightsX + weightSize;
        // m_weightsX = new Float[weightSize];
        // m_weightsY = new Float[weightSize];
        // memset(m_weightsX, 0, sizeof(ScalarFloat) * weightSize);
        // memset(m_weightsY, 0, sizeof(ScalarFloat) * weightSize);
    }

    /* Allocate space for pixels and border regions */
    setSize(size);
}

ImageBlock::~ImageBlock() {
    /* TODO: more safe deleter here*/
    if (m_filter)
        delete[] m_filter;
    if (m_weightsX)
        delete[] m_weightsX;
}

void ImageBlock::clear() {
    size_t size = KAZEN_CHANNEL_COUNT * enoki::hprod(m_size + 2 * m_borderSize);
    memset(m_data.data(), 0, size * sizeof(ScalarFloat));
}

void ImageBlock::setSize(const ScalarVector2i &size) {
    if (size == m_size)
        return;
    m_size = size;
    m_data = enoki::empty<DynamicBuffer<Float>>(KAZEN_CHANNEL_COUNT * enoki::hprod(size + 2 * m_borderSize));
}


// Bitmap *ImageBlock::toBitmap() const {
//     Bitmap *result = new Bitmap(m_size);
//     for (int y=0; y<m_size.y(); ++y)
//         for (int x=0; x<m_size.x(); ++x)
//             result->coeffRef(y, x) = coeff(y + m_borderSize, x + m_borderSize).divideByFilterWeight();
//     return result;
// }

// void ImageBlock::fromBitmap(const Bitmap &bitmap) {
//     if (bitmap.cols() != cols() || bitmap.rows() != rows())
//         throw NoriException("Invalid bitmap dimensions!");

//     for (int y=0; y<m_size.y(); ++y)
//         for (int x=0; x<m_size.x(); ++x)
//             coeffRef(y, x) << bitmap.coeff(y, x), 1;
// }


ImageBlock::Mask ImageBlock::put(const Point2f &pos_, const Float *value, Mask active) {
 
    ScalarVector2i size = m_size + 2 * m_borderSize;

    // Convert to pixel coordinates within the image block
    Point2f pos = pos_ - (m_offset - m_borderSize + .5f);

    if (m_filterRadius > 0.5f + math::RayEpsilon<Float>) {
        // Determine the affected range of pixels
        Point2u lo = Point2u(enoki::max(enoki::ceil2int <Point2i>(pos - m_filterRadius), 0));
        Point2u hi = Point2u(enoki::min(enoki::floor2int<Point2i>(pos + m_filterRadius), size - 1));

        uint32_t n = enoki::ceil2int<uint32_t>((m_filterRadius - 2.f * math::RayEpsilon<ScalarFloat>) * 2.f);

        Point2f base = lo - pos;
        for (uint32_t i = 0; i < n; ++i) {
            Point2f p = base + i;

            auto eval_discretized = [&](Float x, Mask active = true) {
                Int32 index = enoki::min(Int32(enoki::abs(x * m_lookupFactor)), KAZEN_FILTER_RESOLUTION);
                return enoki::gather<Float>(m_filter, index, active);
            };

            m_weightsX[i] = eval_discretized(p.x(), active);
            m_weightsY[i] = eval_discretized(p.y(), active);
        }

        ENOKI_NOUNROLL for (uint32_t yr = 0; yr < n; ++yr) {
            UInt32 y = lo.y() + yr;
            Mask enabled = active && y <= hi.y();

            ENOKI_NOUNROLL for (uint32_t xr = 0; xr < n; ++xr) {
                UInt32 x = lo.x() + xr;
                UInt32 offset = KAZEN_CHANNEL_COUNT * (y * size.x() + x);
                Float weight = m_weightsY[yr] * m_weightsX[xr];

                enabled &= x <= hi.x();
                ENOKI_NOUNROLL for (uint32_t k = 0; k < KAZEN_CHANNEL_COUNT; ++k)
                    enoki::scatter_add(m_data, value[k] * weight, offset + k, enabled);
            }
        }

    } else {
        Point2u lo = enoki::ceil2int<Point2i>(pos - .5f);
        UInt32 offset = KAZEN_CHANNEL_COUNT * (lo.y() * size.x() + lo.x());

        Mask enabled = active && enoki::all(lo >= 0u && lo < size);
        ENOKI_NOUNROLL for (uint32_t k = 0; k < KAZEN_CHANNEL_COUNT; ++k)
            enoki::scatter_add(m_data, value[k], offset + k, enabled);
    }

    return active;
}

    
void ImageBlock::put(ImageBlock &block) {

    ScalarVector2i sourceSize   = block.getSize() + 2 * block.getBorderSize();
    ScalarVector2i targetSize   = getSize() + 2 * getBorderSize();

    ScalarPoint2i sourceOffset  = block.getOffset() - block.getBorderSize();
    ScalarPoint2i targetOffset = getOffset() - getBorderSize();

    accumulate_2d(
        block.data().data(), sourceSize,
        data().data(), targetSize,
        ScalarVector2i(0), sourceOffset - targetOffset,
        sourceSize, KAZEN_CHANNEL_COUNT
    );
}


std::string ImageBlock::toString() const {
    return "ImageBlock[]";
}


BlockGenerator::BlockGenerator(const Vector2i &size, int blockSize)
        : m_size(size), m_blockSize(blockSize) {
    m_numBlocks = Vector2i(enoki::ceil(Vector2f(m_size) / m_blockSize));
    m_blocksLeft = enoki::hprod(m_numBlocks);

    m_direction = ERight;
    m_block = Point2i(m_numBlocks / 2);
    m_stepsLeft = 1;
    m_numSteps = 1;
}

bool BlockGenerator::next(ImageBlock &block) {
    std::lock_guard<tbb::spin_mutex> lock(m_mutex);

    if (m_blocksLeft == 0)
        return false;

    Point2i offset = m_block * m_blockSize;
    block.setOffset(offset);
    block.setSize(enoki::min((int)m_blockSize, m_size - offset));

    if (--m_blocksLeft == 0)
        return true;

    // Prepare the next block's position along the spiral.
    do {
        switch (m_direction) {
            case ERight: ++m_block.x(); break;
            case EDown:  ++m_block.y(); break;
            case ELeft:  --m_block.x(); break;
            case EUp:    --m_block.y(); break;
        }

        if (--m_stepsLeft == 0) {
            m_direction = (m_direction + 1) % 4;
            if (m_direction == ELeft || m_direction == ERight)
                ++m_numSteps;
            m_stepsLeft = m_numSteps;
        }
    } while (enoki::any(m_block < 0 || m_block >= m_numBlocks));

    return true;
}


NAMESPACE_END(kazen)