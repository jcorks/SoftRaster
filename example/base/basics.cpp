#include "basics.h"


using namespace SoftRaster;


/// Basic Vertex shader that applies a modelview and projection matrix to input
/// vertex positions.
///

StageProcedure::SignatureIO VertexShader::InputSignature() const {
    SignatureIO input;
    input.AddSlot(DataType::UserVertex);
    return input;
}

StageProcedure::SignatureIO VertexShader::OutputSignature() const {
    SignatureIO output;
    output.AddSlot(DataType::UserVertex);
    return output;
}


void VertexShader::operator()(RuntimeIO * io) {
    Vertex v;
    io->ReadNext<Vertex>(&v);

    Vector4 in, out;
    in.x = v.x;
    in.y = v.y;
    in.z = v.z;
    in.w = 1.f;

    // transform here
    modelview.Transform((float*)&out, (float*)&in);
    in = out;
    projection.Transform((float*)&out, (float*)&in);

    v.x = out.x;
    v.y = out.y;
    v.z = out.z;


    io->WriteNext<Vertex>(&v);
    io->Commit();
}






StageProcedure::SignatureIO FragmentShader::InputSignature() const {
    SignatureIO input;

    input.AddSlot(DataType::Fragment);

    input.AddSlot(DataType::UserVertex);
    input.AddSlot(DataType::UserVertex);
    input.AddSlot(DataType::UserVertex);

    return input;
}

StageProcedure::SignatureIO FragmentShader::OutputSignature() const {
    SignatureIO output;
    return output;
}


void FragmentShader::operator()(RuntimeIO * io) {
    static Fragment frag;
    static Vertex  v[3];

    io->ReadNext<Fragment>(&frag);


    io->ReadNext<Vertex>(v);    
    io->ReadNext<Vertex>(v+1);    
    io->ReadNext<Vertex>(v+2);    


    static uint8_t color[4];
    color[0] = UINT8_MAX * (frag.bias0*v[0].r + frag.bias1*v[1].r + frag.bias2*v[2].r); 
    color[1] = UINT8_MAX * (frag.bias0*v[0].g + frag.bias1*v[1].g + frag.bias2*v[2].g); 
    color[2] = UINT8_MAX * (frag.bias0*v[0].b + frag.bias1*v[1].b + frag.bias2*v[2].b); 
    color[3] = UINT8_MAX * (frag.bias0*v[0].a + frag.bias1*v[1].a + frag.bias2*v[2].a); 


    io->GetFramebuffer()->PutPixel(frag.x, frag.y, color);  

}


char pixelToChar(uint8_t * pixel) {

    float weight = ((pixel[0] + pixel[1] + pixel[2] + pixel[3]) / (4.f*UINT8_MAX));
    if (weight < .01) return '.';
    return '0' + (((int)(weight*10))%10);
}

