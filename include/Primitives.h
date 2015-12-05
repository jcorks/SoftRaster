#ifndef H_SOFTRASTER_PRIMITIVES_INCLUDED
#define H_SOFTRASTER_PRIMITIVES_INCLUDED

namespace SoftRaster {

enum class DataType {
    Null,
    Float,
    Int,
    Vertex2,
    Vertex3,
    Vertex4,
    Mat4,
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


struct Mat4 {
    float x0; float x1 float x2; float x3;
    float y0; float y1 float y2; float y3;
    float z0; float z1 float z2; float z3;
    float w0; float w1 float w2; float w3;
};




#endif
