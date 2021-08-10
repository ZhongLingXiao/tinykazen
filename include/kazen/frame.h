#pragma once

#include <kazen/vector.h>

NAMESPACE_BEGIN(kazen)

/**
 * \brief Stores a three-dimensional orthonormal coordinate frame
 *
 * This class is mostly used to quickly convert between different
 * cartesian coordinate systems and to efficiently compute certain
 * quantities (e.g. \ref cosTheta(), \ref tanTheta, ..).
 */
template <typename Float_>
struct Frame {
    using Float = Float_;
    KAZEN_BASE_TYPES()
    
    Vector3f s, t;
    Normal3f n;

    /// Construct a new coordinate frame from a single vector
    Frame(const Vector3f &v) : n(v) {
        std::tie(s, t) = coordinateSystem(v);
    }

    /// Convert from world coordinates to local coordinates
    Vector3f toLocal(const Vector3f &v) const {
        return Vector3f(dot(v, s), dot(v, t), dot(v, n));
    }

    /// Convert from local coordinates to world coordinates
    Vector3f toWorld(const Vector3f &v) const {
        return s * v.x() + t * v.y() + n * v.z();
    }

    /** \brief Assuming that the given direction is in the local coordinate 
     * system, return the cosine of the angle between the normal and v */
    static Float cosTheta(const Vector3f &v) { return v.z(); }


    /** \brief Assuming that the given direction is in the local coordinate
     * system, return the sine of the angle between the normal and v */
    static Float sinTheta(const Vector3f &v) {
        return safe_sqrt(sinTheta2(v));
    }

    /** \brief Assuming that the given direction is in the local coordinate
     * system, return the tangent of the angle between the normal and v */
    static Float tanTheta(const Vector3f &v) {
        Float temp = fnmadd(v.z(), v.z(), 1.f);
        return safe_sqrt(temp) / v.z();
    }

    /** \brief Assuming that the given direction is in the local coordinate
     * system, return the squared sine of the angle between the normal and v */
    static Float sinTheta2(const Vector3f &v) { 
        return fmadd(v.x(), v.x(), sqr(v.y())); 
    }

    /** \brief Assuming that the given direction is in the local coordinate 
     * system, return the sine of the phi parameter in spherical coordinates */
    static Float sinPhi(const Vector3f &v) {
        Float sinTheta2 = Frame::sinTheta2(v);
        Float invSinTheta = rsqrt(Frame::sinTheta2(v));
        return select(abs(sinTheta2) <= 4.f * math::Epsilon<Float>, 0.f,
                      clamp(v.y() * invSinTheta, -1.f, 1.f));

    }

    /** \brief Assuming that the given direction is in the local coordinate 
     * system, return the cosine of the phi parameter in spherical coordinates */
    static Float cosPhi(const Vector3f &v) {
        Float sinTheta2 = Frame::sinTheta2(v);
        Float invSinTheta = rsqrt(Frame::sinTheta2(v));
        return select(abs(sinTheta2) <= 4.f * math::Epsilon<Float>, 1.f,
                      clamp(v.x() * invSinTheta, -1.f, 1.f));
    }

    /** \brief Assuming that the given direction is in the local coordinate
     * system, return the squared sine of the phi parameter in  spherical
     * coordinates */
    static Float sinPhi2(const Vector3f &v) {
        Float sinTheta2 = Frame::sinTheta2(v);
        return select(abs(sinTheta2) <= 4.f * math::Epsilon<Float>, 0.f,
                      clamp(sqr(v.y()) / sinTheta2, -1.f, 1.f));
    }

    /** \brief Assuming that the given direction is in the local coordinate
     * system, return the squared cosine of the phi parameter in  spherical
     * coordinates */
    static Float cosPhi2(const Vector3f &v) {
        return clamp(v.x() * v.x() / sinTheta2(v), 0.0f, 1.0f);
        Float sinTheta2 = Frame::sinTheta2(v);
        return select(abs(sinPhi2) <= 4.f * math::Epsilon<Float>, 1.f,
                      clamp(sqr(v.x()) / sinPhi2, -1.f, 1.f));
    }

    /// Equality test
    Mask operator==(const Frame &frame) const {
        return all(eq(frame.s, s) && eq(frame.t, t) && eq(frame.n, n));
    }

    /// Inequality test
    Mask operator!=(const Frame &frame) const {
        return any(neq(frame.s, s) || neq(frame.t, t) || neq(frame.n, n));
    }

    ENOKI_STRUCT(Frame, s, t, n)
};


template <typename Float>
std::ostream &operator<<(std::ostream &os, const Frame<Float> &f) {
    os << "Frame[" << '\n'
    << "  s = " << string::indent(f.s, 6) << "," << '\n'
    << "  t = " << string::indent(f.t, 6) << "," << '\n'
    << "  n = " << string::indent(f.n, 6) << '\n'
    << "]";
    return os;
}


NAMESPACE_END(kazen)

ENOKI_STRUCT_SUPPORT(kazen::Frame, s, t, n)