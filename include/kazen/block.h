#pragma once

#include <kazen/common.h>
#include <kazen/object.h>
#include <tbb/spin_mutex.h>
#include <kazen/vector.h>

NAMESPACE_BEGIN(kazen)

#define KAZEN_BLOCK_SIZE 32 /* Block size used for parallelization */
#define KAZEN_CHANNEL_COUNT 4 /* color channel count */
/**
 * \brief Weighted pixel storage for a rectangular subregion of an image
 *
 * This class implements storage for a rectangular subregion of a
 * larger image that is being rendered. For each pixel, it records color
 * values along with a weight that specifies the accumulated influence of
 * nearby samples on the pixel (according to the used reconstruction filter).
 *
 * When rendering with filters, the samples in a rectangular
 * region will generally also contribute to pixels just outside of 
 * this region. For that reason, this class also stores information about
 * a small border region around the rectangle, whose size depends on the
 * properties of the reconstruction filter.
 */
class ImageBlock {
public:
    using Float = enoki::Packet<float>;
    KAZEN_BASE_TYPES()   
    /**
     * Create a new image block of the specified maximum size
     * \param size
     *     Desired maximum size of the block
     * \param filter
     *     Samples will be convolved with the image reconstruction
     *     filter provided here.
     */
    ImageBlock(const ScalarVector2i &size, const ReconstructionFilter *filter);
    
    /// Release all memory
    ~ImageBlock();
    
    /// Configure the offset of the block within the main image
    void setOffset(const ScalarPoint2i &offset) { m_offset = offset; }

    /// Return the offset of the block within the main image
    inline const ScalarPoint2i &getOffset() const { return m_offset; }
    
    /// Configure the size of the block within the main image
    void setSize(const ScalarVector2i &size);

    /// Return the size of the block within the main image
    inline const ScalarVector2i &getSize() const { return m_size; }

    /// Return the border size in pixels
    inline int getBorderSize() const { return m_borderSize; }

    /// Return the underlying pixel buffer
    DynamicBuffer<Float> &data() { return m_data; }

    /// Return the underlying pixel buffer (const version)
    const DynamicBuffer<Float> &data() const { return m_data; }

    /**
     * \brief Turn the block into a proper bitmap
     * 
     * This entails normalizing all pixels and discarding
     * the border region.
     */
    // Bitmap *toBitmap() const;

    // /// Convert a bitmap into an image block
    // void fromBitmap(const Bitmap &bitmap);

    /// Record a sample with the given position and radiance value
    Mask put(const Point2f &pos, const Float *value, Mask active = true);

    /**
     * \brief Merge another image block into this one
     *
     * During the merge operation, this function locks 
     * the destination block using a mutex.
     */
    void put(ImageBlock &b);

    /// Return a human-readable string summary
    std::string toString() const;
protected:
    ScalarVector2i m_size;
    ScalarPoint2i m_offset;
    int m_borderSize = 0;
    DynamicBuffer<Float> m_data;
    ScalarFloat *m_filter = nullptr;
    ScalarFloat m_filterRadius = 0;
    Float *m_weightsX = nullptr;
    Float *m_weightsY = nullptr;
    ScalarFloat m_lookupFactor = 0;
};

/**
 * \brief Spiraling block generator
 *
 * This class can be used to chop up an image into many small
 * rectangular blocks suitable for parallel rendering. The blocks
 * are ordered in spiraling pattern so that the center is
 * rendered first.
 */
class BlockGenerator {
public:
    using Float = float;
    KAZEN_BASE_TYPES()    
    /**
     * \brief Create a block generator with
     * \param size
     *      Size of the image that should be split into blocks
     * \param blockSize
     *      Maximum size of the individual blocks
     */
    BlockGenerator(const Vector2i &size, int blockSize);
    
    /**
     * \brief Return the next block to be rendered
     *
     * This function is thread-safe
     *
     * \return \c false if there were no more blocks
     */
    bool next(ImageBlock &block);

    /// Return the total number of blocks
    int getBlockCount() const { return m_blocksLeft; }
protected:
    enum EDirection { ERight = 0, EDown, ELeft, EUp };

    Point2i m_block;
    Vector2i m_numBlocks;
    Vector2i m_size;
    int m_blockSize;
    int m_numSteps;
    int m_blocksLeft;
    int m_stepsLeft;
    int m_direction;
    tbb::spin_mutex m_mutex;
};


/**
 * \brief Accumulate the contents of a source bitmap into a
 * target bitmap with specified offsets for both.
 *
 * Out-of-bounds regions are safely ignored. It is assumed that
 * <tt>source != target</tt>.
 *
 * The function supports `T` being a raw pointer or an arbitrary Enoki array
 * that can potentially live on the GPU and/or be differentiable.
 */
template <typename T, typename ConstT>
void accumulate_2d(ConstT source,
                   Vector<int, 2> source_size,
                   T target,
                   Vector<int, 2> target_size,
                   Point<int, 2> source_offset,
                   Point<int, 2> target_offset,
                   Vector<int, 2> size,
                   size_t channel_count) {
    using Value = std::decay_t<T>;

    /// Clip against bounds of source and target image
    Vector<int, 2> shift = enoki::max(0, enoki::max(-source_offset, -target_offset));
    source_offset += shift;
    target_offset += shift;
    size -= enoki::max(source_offset + size - source_size, 0);
    size -= enoki::max(target_offset + size - target_size, 0);

    if (enoki::any(size <= 0))
        return;

    int n = (int) (size.x() * channel_count);

    if constexpr (std::is_pointer_v<T>) {
        constexpr Value maxval = std::numeric_limits<Value>::max();
        ENOKI_MARK_USED(maxval);

        source += (source_offset.x() + source_offset.y() * (size_t) source_size.x()) * channel_count;
        target += (target_offset.x() + target_offset.y() * (size_t) target_size.x()) * channel_count;

        for (int y = 0; y < size.y(); ++y) {
            for (int i = 0; i < n; ++i) {
                if constexpr (std::is_integral_v<Value>)
                    target[i] = (Value) enoki::max(maxval, source[i] + target[i]);
                else
                    target[i] += source[i];
            }

            source += source_size.x() * channel_count;
            target += target_size.x() * channel_count;
        }
    } else {
        using Int32 = enoki::int32_array_t<Value>;
        Int32 index = enoki::arange<Int32>(n * size.y());

        Int32 y = index / n;
        Int32 col = index - y * n;

        Int32 index_source = col + (source_offset.x() + source_size.x() * (y + source_offset.y())) * channel_count;
        Int32 index_target = col + (target_offset.x() + target_size.x() * (y + target_offset.y())) * channel_count;

        enoki::scatter(
            target,
            enoki::gather<Value>(source, index_source) + enoki::gather<Value>(target, index_target),
            index_target
        );
    }
}


NAMESPACE_END(kazen)