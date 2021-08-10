#pragma once

#include <kazen/common.h>
#include <kazen/frame.h>
#include <kazen/vector.h>

NAMESPACE_BEGIN(kazen)

// =======================================================================
// A collection of useful warping functions for importance sampling
// =======================================================================
NAMESPACE_BEGIN(warp)

/// Tent
template <typename Value>
Value intervalToTent(Value sample) {
    sample -= .5f;
    return copysign(1.f - safe_sqrt(fmadd(abs(sample), -2.f, 1.f)), sample);
}

template <typename Value>
Point<Value, 2> squareToTent(const Point<Value, 2> &sample) {
    return intervalToTent(sample);
}

template <typename Value>
Value squareToTentPdf(const Point<Value, 2> &p) {
    auto p_ = abs(p);

    return select(p_.x() <= 1 && p_.y() <= 1,
                  (1.f - p_.x()) * (1.f - p_.y()),
                  zero<Value>());
}


/// Uniform Disk
template <typename Value>
KAZEN_INLINE Point<Value, 2> squareToUniformDisk(const Point<Value, 2> &sample) {
    Value r = sqrt(sample.y());
    auto [s, c] = sincos(math::TwoPi<Value> * sample.x());
    return { c * r, s * r };
}

template <typename Value>
KAZEN_INLINE Value squareToUniformDiskPdf(const Point<Value, 2> &p) {
    return math::InvPi<Value>;
}


/// Uniform Sphere
template <typename Value>
Value circ(Value x) { return safe_sqrt(fnmadd(x, x, 1.f)); }

template <typename Value>
KAZEN_INLINE Vector<Value, 3> squareToUniformSphere(const Point<Value, 2> &sample) {
    Value z = fnmadd(2.f, sample.y(), 1.f),
          r = circ(z);
    auto [s, c] = sincos(2.f * math::Pi<Value> * sample.x());
    return { r * c, r * s, z };
}

template <typename Value>
KAZEN_INLINE Value squareToUniformSpherePdf(const Vector<Value, 3> &v) {
    return math::InvFourPi<Value>;
}


/// Uniform Disk Concentric
/// Low-distortion concentric square to disk mapping by Peter Shirley
template <typename Value>
KAZEN_INLINE Point<Value, 2> squareToUniformDiskConcentric(const Point<Value, 2> &sample) {
    using Mask   = mask_t<Value>;

    Value x = fmsub(2.f, sample.x(), 1.f),
          y = fmsub(2.f, sample.y(), 1.f);

    Mask is_zero = eq(x, zero<Value>()) && eq(y, zero<Value>()),
         quadrant_1_or_3 = abs(x) < abs(y);

    Value r  = select(quadrant_1_or_3, y, x),
          rp = select(quadrant_1_or_3, x, y);

    Value phi = .25f * math::Pi<Value> * rp / r;
    masked(phi, quadrant_1_or_3) = .5f * math::Pi<Value> - phi;
    masked(phi, is_zero) = zero<Value>();

    auto [s, c] = sincos(phi);
    return { r * c, r * s };
}

template <typename Value>
KAZEN_INLINE Value squareToUniformDiskConcentricPdf(const Point<Value, 2> &p) {
    return math::InvPi<Value>;
}


/// Uniform Hemisphere
template <typename Value>
KAZEN_INLINE Vector<Value, 3> squareToUniformHemisphere(const Point<Value, 2> &sample) {
#if 0
    // Approach 1: warping method based on standard disk mapping
    Value z   = sample.y(),
          tmp = circ(z);
    auto [s, c] = sincos(Scalar(2 * math::Pi) * sample.x());
    return { c * tmp, s * tmp, z };
#else
    // Approach 2: low-distortion warping technique based on concentric disk mapping
    Point<Value, 2> p = squareToUniformDiskConcentric(sample);
    Value z = 1.f - squared_norm(p);
    p *= sqrt(z + 1.f);
    return { p.x(), p.y(), z };
#endif
}

template <typename Value>
KAZEN_INLINE Value squareToUniformHemispherePdf(const Vector<Value, 3> &v) {
    return math::InvTwoPi<Value>;
}


/// Cosine Hemisphere
template <typename Value>
KAZEN_INLINE Vector<Value, 3> squareToCosineHemisphere(const Point<Value, 2> &sample) {
    // Low-distortion warping technique based on concentric disk mapping
    Point<Value, 2> p = squareToUniformDiskConcentric(sample);

    // Guard against numerical imprecisions
    Value z = safe_sqrt(1.f - squared_norm(p));

    return { p.x(), p.y(), z };
}

template <typename Value>
KAZEN_INLINE Value squareToCosineHemispherePdf(const Vector<Value, 3> &v) {
    return math::InvPi<Value> * v.z();
}


/// Beckmann distribution
template <typename Value>
KAZEN_INLINE Vector<Value, 3> squareToBeckmann(const Point<Value, 2> &sample, const Value &alpha) {
#if 0
    // Approach 1: warping method based on standard disk mapping
    auto [s, c] = sincos(math::TwoPi<Value> * sample.x());

    Value tan_theta_m_sqr = -sqr(alpha) * log(1.f - sample.y());
    Value cos_theta_m = rsqrt(1 + tan_theta_m_sqr),
          sin_theta_m = circ(cos_theta_m);

    return { sin_theta_m * c, sin_theta_m * s, cos_theta_m };
#else
    // Approach 2: low-distortion warping technique based on concentric disk mapping
    Point<Value, 2> p = squareToUniformDiskConcentric(sample);
    Value r2 = squared_norm(p);

    Value tan_theta_m_sqr = -sqr(alpha) * log(1.f - r2);
    Value cos_theta_m = rsqrt(1.f + tan_theta_m_sqr);
    p *= safe_sqrt(fnmadd(cos_theta_m, cos_theta_m, 1.f) / r2);

    return { p.x(), p.y(), cos_theta_m };
#endif
}


template <typename Value>
KAZEN_INLINE Value squareToBeckmannPdf(const Vector<Value, 3> &m, const Value &alpha) {
    using Frame = Frame<Value>;

    Value temp = Frame::tan_theta(m) / alpha,
          ct   = Frame::cos_theta(m);

    Value result = exp(-sqr(temp)) / (math::Pi<Value> * sqr(alpha * ct) * ct);

    return select(ct < 1e-9f, zero<Value>(), result);
}

NAMESPACE_END(warp)
NAMESPACE_END(kazen)