#pragma once
#include <xmmintrin.h>
#include <cmath>
#include <cstdint>

// The `colour` class represents an RGB colour with floating-point precision.
// It provides various utilities for manipulating and converting colours.
class colour {
    union {
        struct {
            float r, g, b; // Red, Green, and Blue components of the colour
        };
        float rgb[3];     // Array representation of the RGB components
    };

public:
    // Enum for indexing the RGB components
    enum Colour { RED = 0, GREEN = 1, BLUE = 2 };

    // Constructor to initialize the colour with specified RGB values.
    // Default values are 0 (black).
    // Input Variables:
    // - _r: Red component (default 0.0f)
    // - _g: Green component (default 0.0f)
    // - _b: Blue component (default 0.0f)
    colour(float _r = 0, float _g = 0, float _b = 0) : r(_r), g(_g), b(_b) {}

    // Sets the RGB components of the colour.
    // Input Variables:
    // - _r: Red component
    // - _g: Green component
    // - _b: Blue component
    inline void set(float _r, float _g, float _b) { r = _r, g = _g, b = _b; }

    // Accesses the specified component of the colour by index.
    // Input Variables:
    // - c: Index of the component (RED, GREEN, or BLUE)
    // Returns a reference to the specified component.
    float& operator[] (Colour c) { return rgb[c]; }

    // Assigns the values of another colour to this one.
    // Input Variables:
    // - c: The source color
    void operator = (colour c) {
        r = c.r;
        g = c.g;
        b = c.b;
    }

    // Clamps the RGB components of the colour to the range [0, 1].
    void clampColour() {
        r = min(r, 1.0f);
        g = min(g, 1.0f);
        b = min(b, 1.0f);
    }

    // Converts the floating-point RGB values to integer values (0-255).
    // Output Variables:
    // - cr: Red component as an unsigned char
    // - cg: Green component as an unsigned char
    // - cb: Blue component as an unsigned char
    void toRGB(unsigned char& cr, unsigned char& cg, unsigned char& cb) {
        cr = static_cast<unsigned char>(std::floor(r * 255));
        cg = static_cast<unsigned char>(std::floor(g * 255));
        cb = static_cast<unsigned char>(std::floor(b * 255));
    }

    // Scales the RGB components of the colour by a scalar value.
    // Input Variables:
    // - scalar: The scaling factor
    // Returns a new `colour` object with scaled components.
    colour operator * (const float scalar) {
        colour c;
        __m128 vec = _mm_set_ps(0, b, g, r);          // 加载 r, g, b 到 SIMD 寄存器
        vec = _mm_mul_ps(vec, _mm_set1_ps(scalar));   // 并行乘以标量
        float tmp[4];
        _mm_storeu_ps(tmp, vec);
        c.r = tmp[0];
        c.g = tmp[1];
        c.b = tmp[2];
        return c;
    }

    // Multiplies the RGB components of this colour with another colour.
    // Input Variables:
    // - col: The other color to multiply with
    // Returns a new `colour` object with multiplied components.
    colour operator * (const colour& col) {
        colour c;
        c.r = r * col.r;
        c.g = g * col.g;
        c.b = b * col.b;
        return c;
    }

    // Adds the RGB components of another colour to this one.
    // Input Variables:
    // - _c: The other colour to add
    // Returns a new `colour` object with added components.
    colour operator + (const colour& _c) {
        colour c;
        __m128 a = _mm_loadu_ps(rgb);    // 加载当前颜色的 RGB 分量到 SIMD 寄存器
        __m128 b = _mm_loadu_ps(_c.rgb); // 加载传入颜色的 RGB 分量到 SIMD 寄存器
        __m128 result = _mm_add_ps(a, b); // SIMD 并行加法
        _mm_storeu_ps(c.rgb, result);    // 将结果存回 c.rgb
        return c;
    }
};
