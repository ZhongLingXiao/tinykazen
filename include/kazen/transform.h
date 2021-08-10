#pragma once

#include <kazen/common.h>
#include <kazen/ray.h>
#include <enoki/transform.h>

NAMESPACE_BEGIN(kazen)

/**
 * \brief Encapsulates a 4x4 homogeneous coordinate transformation along with
 * its inverse transpose
 *
 * The Transform class provides a set of overloaded matrix-vector
 * multiplication operators for vectors, points, and normals (all of them
 * behave differently under homogeneous coordinate transformations, hence
 * the need to represent them using separate types)
 */
template <typename Point_> struct Transform {
    static constexpr size_t Size = Point_::Size;

    using Float   = value_t<Point_>;
    using Matrix  = enoki::Matrix<Float, Size>;
    using Mask    = mask_t<Float>;
    using Scalar  = scalar_t<Float>;

    // =============================================================
    // Fields
    // =============================================================
    Matrix matrix           = enoki::detail::identity<Matrix>();
    Matrix inverseTranspose = enoki::detail::identity<Matrix>();

    // =============================================================
    // Constructors, methods, etc.
    // =============================================================

    /// Initialize the transformation from the given matrix (and compute its inverse transpose)
    Transform(const Matrix &value)
        : matrix(value),
          inverseTranspose(enoki::inverse_transpose(value)) { }

    /// Concatenate transformations
    KAZEN_INLINE Transform operator*(const Transform &other) const {
        return Transform(matrix * other.matrix,
                         inverseTranspose * other.inverseTranspose);
    }

    /// Compute the inverse of this transformation (involves just shuffles, no arithmetic)
    KAZEN_INLINE Transform inverse() const {
        return Transform(transpose(inverseTranspose), transpose(matrix));
    }

    /// Get the translation part of a matrix
    Vector<Float, Size - 1> translation() const {
        return head<Size - 1>(matrix.coeff(Size - 1));
    }

    /// Equality comparison operator
    bool operator==(const Transform &t) const {
        return matrix == t.matrix && inverseTranspose == t.inverseTranspose;
    }

    /// Inequality comparison operator
    bool operator!=(const Transform &t) const {
        return matrix != t.matrix || inverseTranspose != t.inverseTranspose;
    }

    /**
     * \brief Transform a 3D vector/point/normal/ray by a transformation that
     * is known to be an affine 3D transformation (i.e. no perspective)
     */
    template <typename T>
    KAZEN_INLINE auto affine(const T &input) const {
        return operator*(input);
    }

    /// Transform a point (handles affine/non-perspective transformations only)
    template <typename T, typename Expr = expr_t<Float, T>>
    KAZEN_INLINE Point<Expr, Size - 1> affine(const Point<T, Size - 1> &arg) const {
        Array<Expr, Size> result = matrix.coeff(Size - 1);

        ENOKI_UNROLL for (size_t i = 0; i < Size - 1; ++i)
            result = fmadd(matrix.coeff(i), arg.coeff(i), result);

        return head<Size - 1>(result); // no-op
    }

    /**
     * \brief Transform a 3D point
     */
    template <typename T, typename Expr = expr_t<Float, T>>
    KAZEN_INLINE Point<Expr, Size - 1> operator*(const Point<T, Size - 1> &arg) const {
        Array<Expr, Size> result = matrix.coeff(Size - 1);

        ENOKI_UNROLL for (size_t i = 0; i < Size - 1; ++i)
            result = fmadd(matrix.coeff(i), arg.coeff(i), result);

        return head<Size - 1>(result) / result.coeff(Size - 1);
    }

    /**
     * \brief Transform a 3D vector
     */
    template <typename T, typename Expr = expr_t<Float, T>>
    KAZEN_INLINE Vector<Expr, Size - 1> operator*(const Vector<T, Size - 1> &arg) const {
        Array<Expr, Size> result = matrix.coeff(0);
        result *= arg.x();

        ENOKI_UNROLL for (size_t i = 1; i < Size - 1; ++i)
            result = fmadd(matrix.coeff(i), arg.coeff(i), result);

        return head<Size - 1>(result); // no-op
    }

    /**
     * \brief Transform a 3D normal vector
     */
    template <typename T, typename Expr = expr_t<Float, T>>
    KAZEN_INLINE Normal<Expr, Size - 1> operator*(const Normal<T, Size - 1> &arg) const {
        Array<Expr, Size> result = inverseTranspose.coeff(0);
        result *= arg.x();

        ENOKI_UNROLL for (size_t i = 1; i < Size - 1; ++i)
            result = fmadd(inverseTranspose.coeff(i), arg.coeff(i), result);

        return head<Size - 1>(result); // no-op
    }

    // // TODO
    // /// Transform a ray (for perspective transformations)
    // template <typename T, typename Spectrum, typename Expr = expr_t<Float, T>,
    //           typename Result = Ray<Point<Expr, Size - 1>, Spectrum>>
    // KAZEN_INLINE Result operator*(const Ray<Point<T, Size - 1>, Spectrum> &ray) const {
    //     return Result(operator*(ray.o), operator*(ray.d), ray.mint,
    //                   ray.maxt, ray.time, ray.wavelengths);
    // }

    // /// Transform a ray (for affine/non-perspective transformations)
    // template <typename T, typename Spectrum, typename Expr = expr_t<Float, T>,
    //           typename Result = Ray<Point<Expr, Size - 1>, Spectrum>>
    // KAZEN_INLINE Result transform_affine(const Ray<Point<T, Size - 1>, Spectrum> &ray) const {
    //     return Result(transform_affine(ray.o), transform_affine(ray.d),
    //                   ray.mint, ray.maxt, ray.time, ray.wavelengths);
    // }

    /// Create a translation transformation
    static Transform translate(const Vector<Float, Size - 1> &v) {
        return Transform(enoki::translate<Matrix>(v),
                         transpose(enoki::translate<Matrix>(-v)));
    }

    /// Create a scale transformation
    static Transform scale(const Vector<Float, Size - 1> &v) {
        return Transform(enoki::scale<Matrix>(v),
                         // No need to transpose a diagonal matrix.
                         enoki::scale<Matrix>(rcp(v)));
    }

    /// Create a rotation transformation around an arbitrary axis in 3D. The angle is specified in degrees
    template <size_t N = Size, enable_if_t<N == 4> = 0>
    static Transform rotate(const Vector<Float, Size - 1> &axis, const Float &angle) {
        Matrix matrix = enoki::rotate<Matrix>(axis, deg_to_rad(angle));
        return Transform(matrix, matrix);
    }

    /// Create a rotation transformation in 2D. The angle is specified in degrees
    template <size_t N = Size, enable_if_t<N == 3> = 0>
    static Transform rotate(const Float &angle) {
        Matrix matrix = enoki::rotate<Matrix>(deg_to_rad(angle));
        return Transform(matrix, matrix);
    }

    /** \brief Create a perspective transformation.
     *   (Maps [near, far] to [0, 1])
     *
     *  Projects vectors in camera space onto a plane at z=1:
     *
     *  x_proj = x / z
     *  y_proj = y / z
     *  z_proj = (far * (z - near)) / (z * (far-near))
     *
     *  Camera-space depths are not mapped linearly!
     *
     * \param fov Field of view in degrees
     * \param near Near clipping plane
     * \param far  Far clipping plane
     */
    template <size_t N = Size, enable_if_t<N == 4> = 0>
    static Transform perspective(Float fov, Float near_, Float far_) {
        Float recip = 1.f / (far_ - near_);

        /* Perform a scale so that the field of view is mapped to the interval [-1, 1] */
        Float tan = enoki::tan(deg_to_rad(fov * .5f)),
              cot = 1.f / tan;

        Matrix trafo = diag<Matrix>(Vector<Float, Size>(cot, cot, far_ * recip, 0.f));
        trafo(2, 3) = -near_ * far_ * recip;
        trafo(3, 2) = 1.f;

        Matrix invTrafo = diag<Matrix>(Vector<Float, Size>(tan, tan, 0.f, rcp(near_)));
        invTrafo(2, 3) = 1.f;
        invTrafo(3, 2) = (near_ - far_) / (far_ * near_);

        return Transform(trafo, transpose(invTrafo));
    }

    /** \brief Create an orthographic transformation, which maps Z to [0,1]
     * and leaves the X and Y coordinates untouched.
     *
     * \param near Near clipping plane
     * \param far  Far clipping plane
     */
    template <size_t N = Size, enable_if_t<N == 4> = 0>
    static Transform orthographic(Float near_, Float far_) {
        return scale({1.f, 1.f, 1.f / (far_ - near_)}) *
               translate({ 0.f, 0.f, -near_ });
    }

    /** \brief Create a look-at camera transformation
     *
     * \param origin Camera position
     * \param target Target vector
     * \param up     Up vector
     */
    template <size_t N = Size, enable_if_t<N == 4> = 0>
    static Transform lookAt(const Point<Float, 3> &origin,
                             const Point<Float, 3> &target,
                             const Vector<Float, 3> &up) {
        using Vector3 = Vector<Float, 3>;

        Vector3 dir = normalize(target - origin);
        dir = normalize(dir);
        Vector3 left = normalize(cross(up, dir));

        Vector3 newUp = cross(dir, left);

        Matrix result = Matrix::from_cols(
            concat(left, Scalar(0)),
            concat(newUp, Scalar(0)),
            concat(dir, Scalar(0)),
            concat(origin, Scalar(1))
        );

        Matrix inverse = Matrix::from_rows(
            concat(left, Scalar(0)),
            concat(newUp, Scalar(0)),
            concat(dir, Scalar(0)),
            Vector<Float, 4>(0.f, 0.f, 0.f, 1.f)
        );

        inverse[3] = inverse * concat(-origin, Scalar(1));

        return Transform(result, transpose(inverse));
    }

    /// Creates a transformation that converts from the standard basis to 'frame'
    template <typename Value, size_t N = Size, enable_if_t<N == 4> = 0>
    static Transform toFrame(const Frame<Value> &frame) {
        Matrix result = Matrix::from_cols(
            concat(frame.s, Scalar(0)),
            concat(frame.t, Scalar(0)),
            concat(frame.n, Scalar(0)),
            Vector<Float, 4>(0.f, 0.f, 0.f, 1.f)
        );

        return Transform(result, result);
    }

    /// Creates a transformation that converts from 'frame' to the standard basis
    template <typename Value, size_t N = Size, enable_if_t<N == 4> = 0>
    static Transform fromFrame(const Frame<Value> &frame) {
        Matrix result = Matrix::from_rows(
            concat(frame.s, Scalar(0)),
            concat(frame.t, Scalar(0)),
            concat(frame.n, Scalar(0)),
            Vector<Float, 4>(0.f, 0.f, 0.f, 1.f)
        );

        return Transform(result, result);
    }


    ENOKI_STRUCT(Transform, matrix, inverseTranspose)
};


template <typename Point>
std::ostream &operator<<(std::ostream &os, const Transform<Point> &t) {
    os << t.matrix;
    return os;
}


NAMESPACE_END(kazen)

ENOKI_STRUCT_SUPPORT(kazen::Transform, matrix, inverseTranspose)