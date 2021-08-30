#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <typeinfo>
#include <type_traits>

// fmt
#include <fmt/core.h>

// enoki
#include <enoki/array_traits.h>
#include <enoki/color.h>

// define
#include <kazen/define.h>


NAMESPACE_BEGIN(kazen)
/// Import the complete Enoki namespace
using namespace enoki;

/// fwd
template <typename Value, size_t Size>  struct Vector;
template <typename Value, size_t Size>  struct Point;
template <typename Value, size_t Size>  struct Normal;
template <typename Value, size_t Size>  struct Color;
template <typename Vector>              struct Frame;
template <typename Vector>              struct Ray;
template <typename Point>               struct BoundingBox;
template <typename Point>               struct Transform;

/// core types
template <typename Value, typename T = std::conditional_t<is_static_array_v<Value>, value_t<Value>, Value>>
using DynamicBuffer = std::conditional_t< is_dynamic_array_v<T>, T, DynamicArray<Packet<scalar_t<T>>>>;

template <typename Float_> struct TypeAliases {
    using Float   = Float_;
    using Int32   = int32_array_t<Float>;
    using UInt32  = uint32_array_t<Float>;
    using Mask    = mask_t<Float>;

    using Vector1i = Vector<Int32, 1>;
    using Vector2i = Vector<Int32, 2>;
    using Vector3i = Vector<Int32, 3>;
    using Vector4i = Vector<Int32, 4>;

    using Vector1u = Vector<UInt32, 1>;
    using Vector2u = Vector<UInt32, 2>;
    using Vector3u = Vector<UInt32, 3>;
    using Vector4u = Vector<UInt32, 4>;

    using Vector1f = Vector<Float, 1>;
    using Vector2f = Vector<Float, 2>;
    using Vector3f = Vector<Float, 3>;
    using Vector4f = Vector<Float, 4>;

    using Point1i = Point<Int32, 1>;
    using Point2i = Point<Int32, 2>;
    using Point3i = Point<Int32, 3>;
    using Point4i = Point<Int32, 4>;

    using Point1u = Point<UInt32, 1>;
    using Point2u = Point<UInt32, 2>;
    using Point3u = Point<UInt32, 3>;
    using Point4u = Point<UInt32, 4>;

    using Point1f = Point<Float, 1>;
    using Point2f = Point<Float, 2>;
    using Point3f = Point<Float, 3>;
    using Point4f = Point<Float, 4>;

    using Normal3f = Normal<Float, 3>;
    using Frame3f = Frame<Float>;
   
    using Color3f = Color<Float, 3>;
    using Color4f = Color<Float, 4>;
    
    using BoundingBox1f = BoundingBox<Point1f>;
    using BoundingBox2f = BoundingBox<Point2f>;
    using BoundingBox3f = BoundingBox<Point3f>;
    using BoundingBox4f = BoundingBox<Point4f>;

    using Transform3f   = Transform<Point3f>;
    using Transform4f   = Transform<Point4f>;

    using Ray3f = Ray<Point3f>;
    using DynamicBuffer = kazen::DynamicBuffer<Float>;
};

#define KAZEN_BASE_TYPES_PREFIX(Float_, prefix)                                                     \
    using prefix ## TypeAliases     = kazen::TypeAliases<Float_>;                                   \
    using prefix ## Mask            = typename prefix ## TypeAliases::Mask;                         \
    using prefix ## Int32           = typename prefix ## TypeAliases::Int32;                        \
    using prefix ## UInt32          = typename prefix ## TypeAliases::UInt32;                       \
    using prefix ## Vector1i        = typename prefix ## TypeAliases::Vector1i;                     \
    using prefix ## Vector2i        = typename prefix ## TypeAliases::Vector2i;                     \
    using prefix ## Vector3i        = typename prefix ## TypeAliases::Vector3i;                     \
    using prefix ## Vector4i        = typename prefix ## TypeAliases::Vector4i;                     \
    using prefix ## Vector1u        = typename prefix ## TypeAliases::Vector1u;                     \
    using prefix ## Vector2u        = typename prefix ## TypeAliases::Vector2u;                     \
    using prefix ## Vector3u        = typename prefix ## TypeAliases::Vector3u;                     \
    using prefix ## Vector4u        = typename prefix ## TypeAliases::Vector4u;                     \
    using prefix ## Vector1f        = typename prefix ## TypeAliases::Vector1f;                     \
    using prefix ## Vector2f        = typename prefix ## TypeAliases::Vector2f;                     \
    using prefix ## Vector3f        = typename prefix ## TypeAliases::Vector3f;                     \
    using prefix ## Vector4f        = typename prefix ## TypeAliases::Vector4f;                     \
    using prefix ## Point1i         = typename prefix ## TypeAliases::Point1i;                      \
    using prefix ## Point2i         = typename prefix ## TypeAliases::Point2i;                      \
    using prefix ## Point3i         = typename prefix ## TypeAliases::Point3i;                      \
    using prefix ## Point4i         = typename prefix ## TypeAliases::Point4i;                      \
    using prefix ## Point1u         = typename prefix ## TypeAliases::Point1u;                      \
    using prefix ## Point2u         = typename prefix ## TypeAliases::Point2u;                      \
    using prefix ## Point3u         = typename prefix ## TypeAliases::Point3u;                      \
    using prefix ## Point4u         = typename prefix ## TypeAliases::Point4u;                      \
    using prefix ## Point1f         = typename prefix ## TypeAliases::Point1f;                      \
    using prefix ## Point2f         = typename prefix ## TypeAliases::Point2f;                      \
    using prefix ## Point3f         = typename prefix ## TypeAliases::Point3f;                      \
    using prefix ## Point4f         = typename prefix ## TypeAliases::Point4f;                      \
    using prefix ## Normal3f        = typename prefix ## TypeAliases::Normal3f;                     \
    using prefix ## Frame3f         = typename prefix ## TypeAliases::Frame3f;                      \
    using prefix ## Color3f         = typename prefix ## TypeAliases::Color3f;                      \
    using prefix ## Color4f         = typename prefix ## TypeAliases::Color4f;                      \
    using prefix ## BoundingBox1f   = typename prefix ## TypeAliases::BoundingBox1f;                \
    using prefix ## BoundingBox2f   = typename prefix ## TypeAliases::BoundingBox2f;                \
    using prefix ## BoundingBox3f   = typename prefix ## TypeAliases::BoundingBox3f;                \
    using prefix ## BoundingBox4f   = typename prefix ## TypeAliases::BoundingBox4f;                \
    using prefix ## Transform3f     = typename prefix ## TypeAliases::Transform3f;                  \
    using prefix ## Transform4f     = typename prefix ## TypeAliases::Transform4f;                  \
    using prefix ## Ray3f           = typename prefix ## TypeAliases::Ray3f;

#define KAZEN_BASE_TYPES()                                                                          \
    using ScalarFloat = scalar_t<Float>;                                                            \
    KAZEN_BASE_TYPES_PREFIX(ScalarFloat, Scalar)                                                    \
    KAZEN_BASE_TYPES_PREFIX(Float, /* no-prefix */)


/// kazen renderer types
class BSDF;
class Bitmap;
class BlockGenerator;
class Camera;
class ImageBlock;
class Integrator;
class KDTree;
class Emitter;
struct EmitterQueryRecord;
class Mesh;
class Object;
class ObjectFactory;
class PhaseFunction;
class ReconstructionFilter;
class Sampler;
class Scene;

/// Simple exception class, which stores a human-readable error description
class Exception : public std::runtime_error {
public:
    /// Variadic template constructor to support printf-style arguments
    template <typename... Args> Exception(const char *fmt, const Args &... args) 
        : std::runtime_error(fmt::format(fmt, args...)) { }
};


/// util
NAMESPACE_BEGIN(util)
    /// Determine the width of the terminal window that is used to run kazen
    extern int terminalWidth();
    /// Convert a time value in milliseconds into a human-readable string
    extern std::string timeString(double time, bool precise = false);
    /// Turn a memory size into a human-readable string
    extern std::string memString(size_t size, bool precise = false);   
    /// Return human-readable information about the version
    extern std::string copyright();    
NAMESPACE_END(util)


/// math 
NAMESPACE_BEGIN(math)
    template <typename T> constexpr auto E               = scalar_t<T>(2.71828182845904523536);
    template <typename T> constexpr auto Pi              = scalar_t<T>(3.14159265358979323846);
    template <typename T> constexpr auto TwoPi           = scalar_t<T>(6.28318530717958647692);
    template <typename T> constexpr auto InvPi           = scalar_t<T>(0.31830988618379067154);
    template <typename T> constexpr auto InvTwoPi        = scalar_t<T>(0.15915494309189533577);
    template <typename T> constexpr auto InvFourPi       = scalar_t<T>(0.07957747154594766788);
    template <typename T> constexpr auto SqrtPi          = scalar_t<T>(1.77245385090551602793);
    template <typename T> constexpr auto InvSqrtPi       = scalar_t<T>(0.56418958354775628695);
    template <typename T> constexpr auto SqrtTwo         = scalar_t<T>(1.41421356237309504880);
    template <typename T> constexpr auto InvSqrtTwo      = scalar_t<T>(0.70710678118654752440);
    template <typename T> constexpr auto SqrtTwoPi       = scalar_t<T>(2.50662827463100050242);
    template <typename T> constexpr auto InvSqrtTwoPi    = scalar_t<T>(0.39894228040143267794);
    template <typename T> constexpr auto Infinity        = std::numeric_limits<scalar_t<T>>::infinity();
    template <typename T> constexpr auto Min             = std::numeric_limits<scalar_t<T>>::min();
    template <typename T> constexpr auto Max             = std::numeric_limits<scalar_t<T>>::max();
    template <typename T> constexpr auto OneMinusEpsilon = scalar_t<T>(sizeof(scalar_t<T>) == 8
                                                                    ? 0x1.fffffffffffffp-1
                                                                    : 0x1.fffffep-1);
    template <typename T> constexpr auto RecipOverflow   = scalar_t<T>(sizeof(scalar_t<T>) == 8
                                                                    ? 0x1p-1024 : 0x1p-128);
    template <typename T> constexpr auto Epsilon         = std::numeric_limits<scalar_t<T>>::epsilon() / 2;
    template <typename T> constexpr auto RayEpsilon      = Epsilon<T> * 1500;
    template <typename T> constexpr auto ShadowEpsilon   = RayEpsilon<T> * 10;
NAMESPACE_END(math)


/// simd
NAMESPACE_BEGIN(simd)
    /// Convenience function which computes an array size/type suffix (like '2u' or '3fP')
    template <typename T> std::string type_suffix() {
        using B = scalar_t<T>;

        std::string id = std::to_string(array_size_v<T>);

        if (std::is_floating_point_v<B>) {
            if (std::is_same_v<B, enoki::half>) {
                id += 'h';
            } else {
                if constexpr (is_float_v<B>)
                    id += std::is_same_v<B, float> ? 'f' : 'd';
                else
                    id += std::is_same_v<B, double> ? 'f' : 's';
            }
        } else {
            if (std::is_signed_v<B>)
                id += 'i';
            else
                id += 'u';
        }

        if (is_static_array_v<value_t<T>>)
            id += 'P';
        else if (is_diff_array_v<value_t<T>>)
            id += 'D';
        else if (is_cuda_array_v<value_t<T>>)
            id += 'C';
        else if (is_dynamic_array_v<value_t<T>>)
            id += 'X';

        return id;
    }

    /// Round an integer to a multiple of the current packet size
    template <typename Float>
    inline size_t round_to_packet_size(size_t size) {
        constexpr size_t PacketSize = Float::Size;
        return (size + PacketSize - 1) / PacketSize * PacketSize;
    }
NAMESPACE_END(simd)


/// string
NAMESPACE_BEGIN(string)
    /// Indent every line of a string by some number of spaces
    extern std::string indent(const std::string &string, size_t amount = 2);

    /// Turn a type into a string representation and indent every line by some number of spaces
    template <typename T>
    inline std::string indent(const T &value, size_t amount = 2) {
        std::ostringstream oss;
        oss << value;
        std::string string = oss.str();
        return string::indent(string, amount);
    }

NAMESPACE_END(string)


/// variant
NAMESPACE_BEGIN()
    /**
     * \brief Basic C++11 variant data structure
     *
     * Significantly redesigned version of the approach described at
     * http://www.ojdip.net/2013/10/implementing-a-variant-type-in-cpp
     */

    NAMESPACE_BEGIN(detail)
        template <size_t Arg1, size_t... Args> struct static_max;

        template <size_t Arg> struct static_max<Arg> {
            static const size_t value = Arg;
        };

        template <size_t Arg1, size_t Arg2, size_t... Args> struct static_max<Arg1, Arg2, Args...> {
            static const size_t value = Arg1 >= Arg2 ? static_max<Arg1, Args...>::value:
            static_max<Arg2, Args...>::value;
        };

        template<typename... Args> struct variant_helper;

        template <typename T, typename... Args> struct variant_helper<T, Args...> {
            static bool copy(const std::type_info *type_info, const void *source, void *target)
                noexcept(std::is_nothrow_copy_constructible_v<T> &&
                    noexcept(variant_helper<Args...>::copy(type_info, source, target))) {
                if (type_info == &typeid(T)) {
                    new (target) T(*reinterpret_cast<const T *>(source));
                    return true;
                } else {
                    return variant_helper<Args...>::copy(type_info, source, target);
                }
            }

            static bool move(const std::type_info *type_info, void *source, void *target)
                noexcept(std::is_nothrow_move_constructible_v<T> &&
                    noexcept(variant_helper<Args...>::move(type_info, source, target))) {
                if (type_info == &typeid(T)) {
                    new (target) T(std::move(*reinterpret_cast<T *>(source)));
                    return true;
                } else {
                    return variant_helper<Args...>::move(type_info, source, target);
                }
            }

            static void destruct(const std::type_info *type_info, void *ptr)
                noexcept(std::is_nothrow_destructible_v<T> &&
                    noexcept(variant_helper<Args...>::destruct(type_info, ptr))) {
                if (type_info == &typeid(T))
                    reinterpret_cast<T*>(ptr)->~T();
                else
                    variant_helper<Args...>::destruct(type_info, ptr);
            }

            static bool equals(const std::type_info *type_info, const void *v1, const void *v2) {
                if (type_info == &typeid(T))
                    return (*reinterpret_cast<const T *>(v1)) == (*reinterpret_cast<const T *>(v2));
                else
                    return variant_helper<Args...>::equals(type_info, v1, v2);
            }

            template <typename Visitor>
            static auto visit(const std::type_info *type_info, void *ptr, Visitor &v)
                -> decltype(v(std::declval<T>())) {
                if (type_info == &typeid(T))
                    return v(*reinterpret_cast<T*>(ptr));
                else
                    return variant_helper<Args...>::visit(type_info, ptr, v);
            }
        };

        template <> struct variant_helper<>  {
            static bool copy(const std::type_info *, const void *, void *) noexcept(true) { return false; }
            static bool move(const std::type_info *, void *, void *) noexcept(true) { return false; }
            static void destruct(const std::type_info *, void *) noexcept(true) { }
            static bool equals(const std::type_info *, const void *, const void *) { return false; }
            template <typename Visitor>
            static auto visit(const std::type_info *, void *, Visitor& v) -> decltype(v(nullptr)) {
                return v(nullptr);
            }
        };

    NAMESPACE_END(detail)

    template <typename... Args> struct variant {
    private:
        static const size_t data_size = detail::static_max<sizeof(Args)...>::value;
        static const size_t data_align = detail::static_max<alignof(Args)...>::value;

        using storage_type = typename std::aligned_storage<data_size, data_align>::type;
        using helper_type = detail::variant_helper<Args...>;

    private:
        storage_type data;
        const std::type_info *type_info = nullptr;

    public:
        variant() { }

        variant(const variant<Args...> &v)
            noexcept(noexcept(helper_type::copy(type_info, &v.data, &data)))
            : type_info(v.type_info) {
            helper_type::copy(type_info, &v.data, &data);
        }

        variant(variant<Args...>&& v)
            noexcept(noexcept(helper_type::move(type_info, &v.data, &data)))
            : type_info(v.type_info) {
            helper_type::move(type_info, &v.data, &data);
            helper_type::destruct(type_info, &v.data);
            v.type_info = nullptr;
        }

        ~variant() noexcept(noexcept(helper_type::destruct(type_info, &data))) {
            helper_type::destruct(type_info, &data);
        }

        variant<Args...>& operator=(variant<Args...> &v)
            noexcept(noexcept(operator=((const variant<Args...> &) v))) {
            return operator=((const variant<Args...> &) v);
        }

        variant<Args...>& operator=(const variant<Args...> &v)
            noexcept(
                noexcept(helper_type::copy(type_info, &v.data, &data)) &&
                noexcept(helper_type::destruct(type_info, &data))
            ) {
            helper_type::destruct(type_info, &data);
            type_info = v.type_info;
            helper_type::copy(type_info, &v.data, &data);
            return *this;
        }

        variant<Args...>& operator=(variant<Args...> &&v)
            noexcept(
                noexcept(helper_type::move(type_info, &v.data, &data)) &&
                noexcept(helper_type::destruct(type_info, &data))
            ) {
            helper_type::destruct(type_info, &data);
            type_info = v.type_info;
            helper_type::move(type_info, &v.data, &data);
            helper_type::destruct(type_info, &v.data);
            v.type_info = nullptr;
            return *this;
        }

        template <typename T> variant<Args...>& operator=(T &&value)
            noexcept(
                noexcept(helper_type::copy(type_info, &value, &data)) &&
                noexcept(helper_type::move(type_info, &value, &data)) &&
                noexcept(helper_type::destruct(type_info, &data))
            ) {
            helper_type::destruct(type_info, &data);
            type_info = &typeid(T);
            bool success;
            if (std::is_rvalue_reference_v<T&&>)
                success = helper_type::move(type_info, &value, &data);
            else
                success = helper_type::copy(type_info, &value, &data);
            if (!success)
                throw std::bad_cast();
            return *this;
        }

        const std::type_info &type() const {
            return *type_info;
        }

        template <typename T> bool is() const {
            return type_info == &typeid(T);
        }

        bool empty() const { return type_info == nullptr; }

        template <typename Visitor>
        auto visit(Visitor &&v) -> decltype(helper_type::visit(type_info, &data, v)) {
            return helper_type::visit(type_info, &data, v);
        }

    #if defined(__GNUG__)
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wstrict-aliasing"
    #endif

        template <typename T> operator T&() {
            if (!is<T>())
                throw std::bad_cast();
            return *reinterpret_cast<T *>(&data);
        }

        template <typename T> operator const T&() const {
            if (!is<T>())
                throw std::bad_cast();
            return *reinterpret_cast<const T *>(&data);
        }

    #if defined(__GNUG__)
    #  pragma GCC diagnostic pop
    #endif

        bool operator==(const variant<Args...> &other) const {
            if (type_info != other.type_info)
                return false;
            else
                return helper_type::equals(type_info, &data, &other.data);
        }

        bool operator!=(const variant<Args...> &other) const {
            return !operator==(other);
        }
    };

NAMESPACE_END()



NAMESPACE_END(kazen)