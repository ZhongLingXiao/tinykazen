/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <kazen/sampler.h>
#include <kazen/block.h>
#include <enoki/random.h>

NAMESPACE_BEGIN(kazen)

/**
 * Independent sampling - returns independent uniformly distributed
 * random numbers on <tt>[0, 1)x[0, 1)</tt>.
 *
 * This class is essentially just a wrapper around the pcg32 pseudorandom
 * number generator. For more details on what sample generators do in
 * general, refer to the \ref Sampler class.
 */
class Independent final : public Sampler {    
    using Float = enoki::Packet<float>;
    KAZEN_BASE_TYPES()
public:
    Independent(const PropertyList &propList) {
        m_sampleCount = (size_t) propList.getInt("sampleCount", 1);
    }

    virtual ~Independent() { }

    std::unique_ptr<Sampler> clone() const override {
        std::unique_ptr<Independent> cloned(new Independent());
        cloned->m_sampleCount = m_sampleCount;
        cloned->m_random = m_random;
        return std::move(cloned);
    }

    void prepare(const ImageBlock &block) {
        m_random.seed(block.getOffset()[0], block.getOffset()[1]);
    }

    void generate() { /* No-op for this sampler */ }
    void advance()  { /* No-op for this sampler */ }

    Float next1D(Mask active = true) override {
        return m_random.next_float<Float>(active);
    }
    
    Point2f next2D(Mask active = true) {
        Float f1 = next1D();
        Float f2 = next1D();    
        return Point2f(f1, f2);
    }

    std::string toString() const {
        return fmt::format("Independent[sampleCount={}]", m_sampleCount);
    }
protected:
    Independent() { }

private:
    enoki::PCG32<UInt32> m_random;
};

KAZEN_REGISTER_CLASS(Independent, "independent");

NAMESPACE_END(kazen)
