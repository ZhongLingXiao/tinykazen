#pragma once

#include <kazen/object.h>

NAMESPACE_BEGIN(kazen)

/// Measures associated with probability distributions
enum EMeasure {
    EUnknownMeasure = 0,
    ESolidAngle,
    EDiscrete
};

/**
 * \brief Convenience data structure used to pass multiple
 * parameters to the evaluation and sampling routines in \ref BSDF
 */
struct BSDFQueryRecord {
    using Float = enoki::Packet<float>;
    KAZEN_BASE_TYPES()

    /// Incident direction (in the local frame)
    Vector3f wi;

    /// Outgoing direction (in the local frame)
    Vector3f wo;

    /// Relative refractive index in the sampled direction
    ScalarFloat eta;

    /// Measure associated with the sample
    EMeasure measure;

    /// Create a new record for sampling the BSDF
    BSDFQueryRecord(const Vector3f &wi)
        : wi(wi), eta(1.f), measure(EUnknownMeasure) { }

    /// Create a new record for querying the BSDF
    BSDFQueryRecord(const Vector3f &wi,
            const Vector3f &wo, EMeasure measure)
        : wi(wi), wo(wo), eta(1.f), measure(measure) { }
};

/**
 * \brief Superclass of all bidirectional scattering distribution functions
 */
class BSDF : public Object {
public:
    using Float = enoki::Packet<float>;
    KAZEN_BASE_TYPES()

    /**
     * \brief Sample the BSDF and return the importance weight (i.e. the
     * value of the BSDF * cos(theta_o) divided by the probability density
     * of the sample with respect to solid angles).
     *
     * \param bRec    A BSDF query record
     * \param sample  A uniformly distributed sample on \f$[0,1]^2\f$
     *
     * \return The BSDF value divided by the probability density of the sample
     *         sample. The returned value also includes the cosine
     *         foreshortening factor associated with the outgoing direction,
     *         when this is appropriate. A zero value means that sampling
     *         failed.
     */
    virtual Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample, Mask mask = true) const = 0;

    /**
     * \brief Evaluate the BSDF for a pair of directions and measure
     * specified in \code bRec
     *
     * \param bRec
     *     A record with detailed information on the BSDF query
     * \return
     *     The BSDF value, evaluated for each color channel
     */
    virtual Color3f eval(const BSDFQueryRecord &bRec, Mask mask = true) const = 0;

    /**
     * \brief Compute the probability of sampling \c bRec.wo
     * (conditioned on \c bRec.wi).
     *
     * This method provides access to the probability density that
     * is realized by the \ref sample() method.
     *
     * \param bRec
     *     A record with detailed information on the BSDF query
     *
     * \return
     *     A probability/density value expressed with respect
     *     to the specified measure
     */

    virtual float pdf(const BSDFQueryRecord &bRec, Mask mask = true) const = 0;

    /**
     * \brief Return the type of object (i.e. Mesh/BSDF/etc.)
     * provided by this instance
     * */
    EClassType getClassType() const { return EBSDF; }

    /**
     * \brief Return whether or not this BRDF is diffuse. This
     * is primarily used by photon mapping to decide whether
     * or not to store photons on a surface
     */
    virtual bool isDiffuse() const { return false; }
};

NAMESPACE_END(kazen)