#ifndef H_SOFTRASTER_PRIMITIVES_INCLUDED
#define H_SOFTRASTER_PRIMITIVES_INCLUDED

namespace SoftRaster {

/// \brief A polygon rasterizer primitive.
///
enum class Polygon {
    Triangles, ///< Has 3 points and is filled
    Lines,     ///< Has 2 points.
    Points     ///< 1 point.
};

/// \brief Depth buffering mode.
///
enum class DepthBuffering {
    BytePrecision,  ///< Performs depth buffering using 8 bits of precision.
    ShortPrecision, ///< Performs depth buffering using 16 bits of precision.
    FloatPrecision, ///< Performs depth buffering using 32 bits or precision in floating point form.
    None            ///< Does not perform any depth buffering. All fragments pass the depth test.
};


/// \brief Enumeration of data type primitives.
///
enum class DataType {
    Null,          ///< Does nto represent an actual type.
    Float,         ///< Represents a 32-bit floating point number.
    Int,           ///< Represends a 32-bit signed integer.
    Vector2,       ///< Represents a Vector2 object.
    Vector3,       ///< Represents a Vector3 object.
    Vector4,       ///< Represents a Vector4 object.
    Mat4,          ///< Represents a Mat4 object.
    Fragment,      ///< Represents a Fragment
    UserVertex     ///< Represents the UserVertexT supplied to the Context when rendering. 
};


/// \brief An object consisting of 2 floating point numbers.
///
struct Vector2 {
    float x; ///< First float component.
    float y; ///< Second float component.
};

/// \brief An object consisting of 3 floating point numbers.
///
struct Vector3 {
    float x; ///< First float component.
    float y; ///< Second float component.
    float z; ///< Third float component.
};


/// \brief An object consisting of 4 floating point numbers.
///
struct Vector4 {
    float x; ///< First float component.
    float y; ///< Second float component.
    float z; ///< Third float component.
    float w; ///< Fourth float component.
};

/// \brief An object containing the result of a single rasterization pixel.
///
/// The bias attributes denote a gravitation towards a source vertex
/// from which the fragment was generated. This can be used to produce "varying" behavior.
struct Fragment {
    uint16_t x; ///< X position of the fragment on the target texture.
    uint16_t y; ///< Y position of the fragment on the target texture.

    float bias0; ///< The bias of the Fragment towards the first vertex.  From 0.f to 1.f;
    float bias1; ///< The bias of the Fragment towards the second vertex. From 0.f to 1.f;
    float bias2; ///< The bias of the Fragment towards the third vertex.  From 0.f to 1.f;

};


/// \brief A 4-by-4 matrix consisting of 16 floating point numbers
///
struct Mat4 {
    float x0; float x1; float x2; float x3;
    float y0; float y1; float y2; float y3;
    float z0; float z1; float z2; float z3;
    float w0; float w1; float w2; float w3;
};


/// \brief An RGBA Color. Each channel is expressed on the range 0.f to 1.f
///
struct Color {
    float r; ///< Red channel.
    float g; ///< Green channel.
    float b; ///< Blue channel.
    float a; ///< Alpha channel. 1.f is opaque.
};

}
#endif
