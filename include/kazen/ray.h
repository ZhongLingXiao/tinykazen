#pragma once

#include <kazen/vector.h>
#include <kazen/color.h>

NAMESPACE_BEGIN(kazen)

/**
 * \brief Simple n-dimensional ray segment data structure
 * 
 * Along with the ray origin and direction, this data structure additionally
 * stores a ray segment [mint, maxt] (whose entries may include positive/negative
 * infinity), as well as the componentwise reciprocals of the ray direction.
 * That is just done for convenience, as these values are frequently required.
 *
 * \remark Important: be careful when changing the ray direction. You must call
 * \ref update() to compute the componentwise reciprocals as well, or kazen's
 * ray-object intersection code may produce undefined results.
 */
template <typename Point_> struct Ray {
    static constexpr size_t Size = array_size_v<Point_>;

    using Point     = Point_;
    using Float     = value_t<Point>;
    using Vector    = kazen::Vector<Float, Size>;

    // =============================================================
    // Fields
    // =============================================================
    Point o;                                ///< Ray origin
    Vector d;                               ///< Ray direction
    Vector dRcp;                            ///< Componentwise reciprocals of the ray direction
    Float mint = math::RayEpsilon<Float>;   ///< Minimum position on the ray segment
    Float maxt = math::Infinity<Float>;     ///< Maximum position on the ray segment
    Float time = 0.f;                       ///< Time value associated with this ray

    // =============================================================
    // Constructors, methods, etc.
    // =============================================================    

    /// Construct a new ray (o, d) with time
    Ray(const Point &o, const Vector &d, const Float &t)
        : o(o), d(d), time(t) {
        update();
    }

    /// Construct a new ray (o, d) with bounds
    Ray(const Point &o, const Vector &d, Float mint, Float maxt, Float time)
        : o(o), d(d), dRcp(rcp(d)), mint(mint), maxt(maxt), time(time) { }

    /// Copy a ray, but change the [mint, maxt] interval
    Ray(const Ray &r, Float mint, Float maxt)
        : o(r.o), d(r.d), dRcp(r.dRcp), mint(mint), maxt(maxt), time(r.time) { }

    /// Update the reciprocal ray directions after changing 'd'
    void update() { dRcp = rcp(d); }

    /// Return the position of a point along the ray
    Point operator() (Float t) const { return fmadd(d, t, o); }

    /// Return a ray that points into the opposite direction
    Ray reverse() const {
        Ray result;
        result.o            = o;
        result.d            = -d;
        result.dRcp         = -dRcp;
        result.mint         = mint;
        result.maxt         = maxt;
        result.time         = time;
        return result;
    }

    ENOKI_STRUCT(Ray, o, d, dRcp, mint, maxt, time)
};


NAMESPACE_END(kazen)
// Support for static & dynamic vectorization
ENOKI_STRUCT_SUPPORT(kazen::Ray, o, d, dRcp, mint, maxt, time)