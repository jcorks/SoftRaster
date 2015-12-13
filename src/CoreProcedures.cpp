#include <SoftRaster/CoreProcedures.h>
#include <algorithm>
using namespace SoftRaster;






// converts from cartesian coords to barycentric coords
class BarycentricTransform {
  public:
    BarycentricTransform(Vector3 * v0, Vector3 * v1, Vector3 * v2, int w, int h) {

        // convert homogenous coords to cartesian equivalents
        cartV0x = w * (v0->x+1)/2.f;
        cartV0y = h * (v0->y+1)/2.f;

        cartV1x = w * (v1->x+1)/2.f;
        cartV1y = h * (v1->y+1)/2.f;

        cartV2x = w * (v2->x+1)/2.f;
        cartV2y = h * (v2->y+1)/2.f;

        float baryTrans[4];

        baryTrans[0] = (cartV0x - cartV2x);
        baryTrans[1] = (cartV1x - cartV2x);
        baryTrans[2] = (cartV0y - cartV2y);
        baryTrans[3] = (cartV1y - cartV2y);

        // calc inverse

        float det = 1 / (baryTrans[0]*baryTrans[3] - baryTrans[1]*baryTrans[2]);
        inverseBar[0] =  (baryTrans[3] * det);
        inverseBar[1] = -(baryTrans[1] * det);
        inverseBar[2] = -(baryTrans[2] * det);
        inverseBar[3] =  (baryTrans[0] * det);
    }


    
    // converts xy into "biases" towards each vertex
    void Transform(int x, int y, float * b0, float * b1, float * b2) {
        static float inVec[2];
        inVec[0] = x - cartV2x;
        inVec[1] = y - cartV2y;

        *b0 = inverseBar[0]*inVec[0] + inverseBar[1]*inVec[1];
        *b1 = inverseBar[2]*inVec[0] + inverseBar[3]*inVec[1];
        *b2 = 1 - *b0 - *b1;
    }

  private:
    

    int cartV0x;
    int cartV0y;
    
    int cartV1x;
    int cartV1y;

    int cartV2x;
    int cartV2y;

    float inverseBar[4];
};
    

class DepthBuffer {
  public:
    virtual void Reset(uint16_t, uint16_t) = 0;
    virtual bool Test(uint16_t, uint16_t, float) = 0;

};


class DepthBuffer8Bit : public DepthBuffer {
  public:
    DepthBuffer8Bit() {
        data = nullptr;
        numUnits = 0;
        w = 0;
        h = 0;
    }
    void Reset(uint16_t fbW, uint16_t fbH) {
        w = fbW;
        h = fbH;
        if (numUnits < fbW * fbH) {
            numUnits = fbW * fbH;
            data = (uint8_t*)realloc(data, numUnits);
        }
        memset(data, 0, numUnits);        
    }
    bool Test(uint16_t x, uint16_t y, float homogenousZ) {
        if (homogenousZ < -1.f || homogenousZ > 1.f) return false;
        uint8_t val = (homogenousZ = (homogenousZ+1.f)/2.f) * UINT8_MAX;
        if (val > data[x + y*w]) {
            data[x + y*w] = val;
            return true;
        }
        return false;
    }
    
  private:
    uint8_t * data;
    uint16_t w;
    uint16_t h;
    uint32_t numUnits;
};



// The built in stages are a little strange
// because they will often need to pass information that they don't use
//
//
//


class Rasterizer : public StageProcedure {
  public:
    Rasterizer(Polygon p, DepthBuffering d);

    SignatureIO InputSignature() const;
    SignatureIO OutputSignature() const;


    void operator()(RuntimeIO * io_);
    

  private:



    // sets up the stage for a new set of vertices
    void NewRun();



    // Rasterization of the triangle 
    // by testing if fragments lie within the triangle
    // using barycentric coordinates
    void (*PopulateFragments)(Rasterizer *);

    // returns false if all tris are outside of
    // clipping space. That is, returns false if
    // all xyz of each vertex have a magnitude greater than 1
    bool PassesClipTest();


    // impl
    static void PopulateFragments_Triangles(Rasterizer *);
    static void PopulateFragments_Lines(Rasterizer *);
    static void PopulateFragments_Points(Rasterizer *);



    



    int framebufferW;
    int framebufferH;    

    uint8_t * srcV[3];



    std::vector<Fragment> fragments;
    RuntimeIO * io;
    uint8_t count;
    uint8_t vertexCount;
    DepthBuffer * PassesDepth;

};








StageProcedure * SoftRaster::CreateRasterizer(Polygon p, DepthBuffering d) {
    return new Rasterizer(p, d);
}


















// Rasterizer impl

Rasterizer::Rasterizer(Polygon p, DepthBuffering d) {
    switch(p) {
      case Polygon::Triangles: 
        PopulateFragments = PopulateFragments_Triangles;
        vertexCount = 3; 
        break;

      case Polygon::Lines: 
        PopulateFragments = PopulateFragments_Lines;
        vertexCount = 2; 
        break;

      case Polygon::Points:
        PopulateFragments = PopulateFragments_Points;
        vertexCount = 1; 
        break;

    }

    switch(d) {
      case DepthBuffering::None:           PassesDepth = new DepthBuffer8Bit;  break;
      case DepthBuffering::BytePrecision:  PassesDepth = new DepthBuffer8Bit;  break;
      case DepthBuffering::FloatPrecision: PassesDepth = new DepthBuffer8Bit; break;
    }

    srcV[0] = nullptr;
    srcV[1] = nullptr;
    srcV[2] = nullptr;

}



StageProcedure::SignatureIO Rasterizer::InputSignature() const {
    SignatureIO input;

    input.AddSlot(DataType::UserVertex);

    return input;
}


StageProcedure::SignatureIO Rasterizer::OutputSignature() const {
    SignatureIO output;
    
    output.AddSlot(DataType::Fragment);

    for(uint32_t i = 0; i < vertexCount; ++i) {
        output.AddSlot(DataType::UserVertex);
    }

    return output;
}




// actually performs the 
void Rasterizer::operator()(RuntimeIO * io_) {

    // initial setup()
    if (io_->GetCurrentIteration() == 0) {
        io = io_;
        count = 0;

        // reallocate vertex stores
        for(uint32_t i = 0; i < vertexCount; ++i) {
            if (srcV[i]) delete[] srcV[i];  
            srcV[i] = new uint8_t[io->SizeOf(DataType::UserVertex)];   
        }
        framebufferW = io->GetFramebuffer()->Width();
        framebufferH = io->GetFramebuffer()->Height();

        PassesDepth->Reset(framebufferW, framebufferH);

    }

    // Copy the vertex into our stores
    memcpy(srcV[count++], io->GetReadPointer(), io->SizeOf(DataType::UserVertex));


    // If our polygon is complete, actually render
    if (count >= vertexCount){
        //if (!PassesClipTest()) return;
    
        // clear out old results
        fragments.clear();


        // Populates fragments
        PopulateFragments(this);


        // commit frags
        Fragment * out;
        const int offset = sizeof(Fragment);
        int sizeofVertex = io->SizeOf(DataType::UserVertex);
        float v0z = ((Vector3*)(srcV[0]))->z;
        float v1z = ((Vector3*)(srcV[1]))->z;
        float v2z = ((Vector3*)(srcV[2]))->z;
        for(int i = 0; i < fragments.size(); ++i) { 
            out = &fragments[i];

            // test the depth 
            if (!PassesDepth->Test(out->x, out->y, 
                out->bias0 * v0z + 
                out->bias1 * v1z +
                out->bias2 * v2z   )) continue;

            io->WriteNext<Fragment>(out);
            
            

            memcpy(io->GetWritePointer() +offset,                srcV[0], sizeofVertex);
            memcpy(io->GetWritePointer() +offset+sizeofVertex,   srcV[1], sizeofVertex);
            memcpy(io->GetWritePointer() +offset+sizeofVertex*2, srcV[2], sizeofVertex);

            

            io->Commit();
        }
        count = 0;
    }
}











// Mode specific implementation


// Rasterization of the triangle 
// by testing if fragments lie within the triangle
// using barycentric coordinates
void Rasterizer::PopulateFragments_Triangles(Rasterizer * r) {

    static Vector3 v0, v1, v2;
    static Fragment frag;
    int boundXmin, boundXmax,
        boundYmin, boundYmax;




    v0 = *((Vector3*)r->srcV[0]);
    v1 = *((Vector3*)r->srcV[1]);
    v2 = *((Vector3*)r->srcV[2]);


    // prepares the barycentric transfrom
    // used to test whether or not points are within the triangle
    // and to produce the varying biases. SO USEFUL
    BarycentricTransform baryTest(&v0, &v1, &v2,
                                  r->io->GetFramebuffer()->Width(), r->io->GetFramebuffer()->Height()              
                                  );


    // first lets decide what texels should even be considered
    // A superset of the texels to test would be the bounding box of the triangle
    boundXmin = r->framebufferW * ((std::min(std::min(v0.x, v1.x), v2.x))+1)/2.f;
    boundYmin = r->framebufferH * ((std::min(std::min(v0.y, v1.y), v2.y))+1)/2.f;
    boundXmax = r->framebufferW * ((std::max(std::max(v0.x, v1.x), v2.x))+1)/2.f;
    boundYmax = r->framebufferH * ((std::max(std::max(v0.y, v1.y), v2.y))+1)/2.f;
    

    
            
    
    for(int y = boundYmin; y < boundYmax; ++y) {
        if (y < 0 || y >= r->framebufferH) continue;   
        for(int x = boundXmin; x < boundXmax; ++x) {
            if (x < 0 || x >= r->framebufferW) continue;                

            // transforms cartesion coords into barycentric coordinates
            baryTest.Transform(x, y, 
                &frag.bias0,
                &frag.bias1,
                &frag.bias2);

            if (frag.bias0 < 0.f ||
                frag.bias1 < 0.f ||
                frag.bias2 < 0.f) continue;


            frag.x = x;
            frag.y = r->io->GetFramebuffer()->Height() - y-1;

            r->fragments.push_back(frag);
        
        }
    }


}


void Rasterizer::PopulateFragments_Lines(Rasterizer *) {
    // TODO
}


void Rasterizer::PopulateFragments_Points(Rasterizer *) {
    // TODO
}









