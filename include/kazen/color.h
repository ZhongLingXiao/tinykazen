#pragma once

#include <kazen/common.h>

NAMESPACE_BEGIN(kazen)

// =======================================================================
// Data types for RGB data
// =======================================================================
template <typename Value_, size_t Size_ = 3>
struct Color : enoki::StaticArrayImpl<Value_, Size_, false, Color<Value_, Size_>> {
    using Base = enoki::StaticArrayImpl<Value_, Size_, false, Color<Value_, Size_>>;

    /// Helper alias used to implement type promotion rules
    template <typename T> using ReplaceValue = Color<T, Size_>;

    using ArrayType = Color;
    using MaskType = enoki::Mask<Value_, Size_>;

    decltype(auto) r() const { return Base::x(); }
    decltype(auto) r() { return Base::x(); }

    decltype(auto) g() const { return Base::y(); }
    decltype(auto) g() { return Base::y(); }

    decltype(auto) b() const { return Base::z(); }
    decltype(auto) b() { return Base::z(); }

    decltype(auto) a() const { return Base::w(); }
    decltype(auto) a() { return Base::w(); }

    ENOKI_ARRAY_IMPORT(Base, Color)
};


// =======================================================================
// Masking support for color and spectrum data types
// =======================================================================
template <typename Value_, size_t Size_>
struct Color<enoki::detail::MaskedArray<Value_>, Size_>
    : enoki::detail::MaskedArray<Color<Value_, Size_>> {
    using Base = enoki::detail::MaskedArray<Color<Value_, Size_>>;
    using Base::Base;
    using Base::operator=;
    Color(const Base &b) : Base(b) { }
};


// =======================================================================
// Utility functions
// =======================================================================
template <typename Float> Float getLuminance(const Color<Float, 3> &c) {
    return c[0] * 0.212671f + c[1] * 0.715160f + c[2] * 0.072169f;
}

template <typename Float> Color<Float, 3> toSRGB(const Color<Float, 3> &c) {
    return Color<Float, 3>(enoki::linear_to_srgb(c.x()),
                        enoki::linear_to_srgb(c.y()),
                        enoki::linear_to_srgb(c.z()));
}

template <typename Float> Color<Float, 3> toLinearRGB(const Color<Float, 3> &c) {
    return Color<Float, 3>(enoki::srgb_to_linear(c.x()),
                        enoki::srgb_to_linear(c.y()),
                        enoki::srgb_to_linear(c.z()));
}

NAMESPACE_END(kazen)