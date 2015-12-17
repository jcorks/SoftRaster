#include <SoftRaster/SoftRaster.h>
#include "TransformMatrix.h"

struct Vertex : public SoftRaster::Vector3 {
    Vertex(){}
    Vertex(float f0, float f1, float f2,   float f3, float f4, float f5, float f6) :
        r{f3}, g{f4}, b{f5}, a{f6}
    {
        x = f0; y = f1; z = f2;
    }
    
    
    float r;
    float g;
    float b;
    float a;
};


class VertexShader : public SoftRaster::StageProcedure {
  public:
    SoftRaster::StageProcedure::SignatureIO InputSignature()const ;
    SoftRaster::StageProcedure::SignatureIO OutputSignature()const;

    TransformMatrix modelview;
    TransformMatrix projection;

    void operator()(SoftRaster::RuntimeIO *);

  private:

};


class FragmentShader : public SoftRaster::StageProcedure {
  public:
    SoftRaster::StageProcedure::SignatureIO InputSignature()const;
    SoftRaster::StageProcedure::SignatureIO OutputSignature()const;

    void operator()(SoftRaster::RuntimeIO *);

  private:

};


char pixelToChar(uint8_t *);
