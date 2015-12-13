#ifndef H_SOFTRASTER_PRIMITIVES_INCLUDED
#define H_SOFTRASTER_PRIMITIVES_INCLUDED

namespace SoftRaster {

///
enum class Polygon {
    Triangles,
    Lines,
    Points
};

enum class DepthBuffering {
    BytePrecision,
    ShortPrecision,
    FloatPrecision,
    None
};

enum class DataType {
    Null,
    Float,
    Int,
    Vector2,
    Vector3,
    Vector4,
    Mat4,
    Fragment,
    UserVertex
};


struct Vector2 {
    float x;
    float y;
};

struct Vector3 {
    float x;
    float y;
    float z;
};


struct Vector4 {
    float x;
    float y;
    float z;
    float w;
};

struct Fragment {
    uint16_t x;
    uint16_t y;

    float bias0;
    float bias1;
    float bias2;

};


struct Mat4 {
    float x0; float x1; float x2; float x3;
    float y0; float y1; float y2; float y3;
    float z0; float z1; float z2; float z3;
    float w0; float w1; float w2; float w3;
};


struct Color {
    float r;
    float g;
    float b;
    float a;
};

}
#endif
