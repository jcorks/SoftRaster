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
    


// The built in stages are a little strange
// because they will often need to pass information that they don't use
//
//
//


// Rasterize triangles stage
// IN:
//  - Vertex (3 first elements should be x, y, z
// Out:
//  - uint16_t (fragment x)
//  - uint16_t (fragment y)
//  - float    (Bias towards vertex 0);
//  - float    (Bias towards vertex 1);
//  - float    (Bias towards vertex 2);
//  - Vertex   (0)
//  - Vertex   (1)
//  - Vertex   (2)
// Bias allows for varying behavior
class RasterizeTriangles : public StageProcedure {
  public:
    SignatureIO InputSignature() const {
        SignatureIO input;

        input.AddSlot(DataType::UserVertex);
    
        return input;
    }


    SignatureIO OutputSignature() const {
        SignatureIO output;
        
        output.AddSlot(DataType::Int);
        output.AddSlot(DataType::Int);

        output.AddSlot(DataType::Float);
        output.AddSlot(DataType::Float);
        output.AddSlot(DataType::Float);

        output.AddSlot(DataType::UserVertex);
        output.AddSlot(DataType::UserVertex);
        output.AddSlot(DataType::UserVertex);




        return output;
    }


    void operator()(RuntimeIO * io_) {
        io = io_;

        // initial setup()
        if (io->GetCurrentIteration() == 0) {
            NewRun();    
        }
        

        PushVertex();
        count++;

        if (count >= 3){
            if (!PassesClipTest()) return;
        
            // Populates fragments
            PopulateFragments();

    
            // commit frags
            FragmentInfo * out;
            const int offset = sizeof(int)*2 + sizeof(float)*3;
            int sizeofVertex = io->SizeOf(DataType::UserVertex);
            for(int i = 0; i < fragments.size(); ++i) { 
                out = &fragments[i];
                
                io->WriteNext<int>(&out->x);
                io->WriteNext<int>(&out->y);

                io->WriteNext<float>(&out->bias0);
                io->WriteNext<float>(&out->bias1);
                io->WriteNext<float>(&out->bias2);
                
                memcpy(io->GetWritePointer() +offset,                srcV[0], sizeofVertex);
                memcpy(io->GetWritePointer() +offset+sizeofVertex,   srcV[1], sizeofVertex);
                memcpy(io->GetWritePointer() +offset+sizeofVertex*2, srcV[2], sizeofVertex);
    
                io->Commit();
            }
            count = 0;
        }
    }
    

  private:
    struct FragmentInfo {
        int x; int y;
        float bias0; 
        float bias1;
        float bias2;
    };


    // sets up the stage for a new set of vertices
    void NewRun() {
        count = 0;
        if (srcV[0]) delete[] srcV[0];  
        if (srcV[1]) delete[] srcV[1];  
        if (srcV[2]) delete[] srcV[2];  
        srcV[0] = new uint8_t[io->SizeOf(DataType::UserVertex)];
        srcV[1] = new uint8_t[io->SizeOf(DataType::UserVertex)];
        srcV[2] = new uint8_t[io->SizeOf(DataType::UserVertex)];   

        framebufferW = io->GetFramebuffer()->Width();
        framebufferH = io->GetFramebuffer()->Height();
    }

    inline void PushVertex() const {
        memcpy(srcV[count], io->GetReadPointer(), io->SizeOf(DataType::UserVertex));
    };


    // Rasterization of the triangle 
    // by testing if fragments lie within the triangle
    // using barycentric coordinates
    void PopulateFragments() {
        fragments.clear();

        static Vector3 v0, v1, v2;
        static FragmentInfo frag;
        int boundXmin, boundXmax,
            boundYmin, boundYmax;
    



        v0 = *((Vector3*)srcV[0]);
        v1 = *((Vector3*)srcV[1]);
        v2 = *((Vector3*)srcV[2]);


        // prepares the barycentric transfrom
        // used to test whether or not points are within the triangle
        // and to produce the varying biases. SO USEFUL
        BarycentricTransform baryTest(&v0, &v1, &v2,
                                      io->GetFramebuffer()->Width(), io->GetFramebuffer()->Height()              
                                      );


        // first lets decide what texels should even be considered
        // A superset of the texels to test would be the bounding box of the triangle
        boundXmin = framebufferW * ((std::min(std::min(v0.x, v1.x), v2.x))+1)/2.f;
        boundYmin = framebufferH * ((std::min(std::min(v0.y, v1.y), v2.y))+1)/2.f;
        boundXmax = framebufferW * ((std::max(std::max(v0.x, v1.x), v2.x))+1)/2.f;
        boundYmax = framebufferH * ((std::max(std::max(v0.y, v1.y), v2.y))+1)/2.f;
        

        
                
        
        for(int y = boundYmin; y < boundYmax; ++y) {
            if (y < 0 || y >= framebufferH) continue;   
            for(int x = boundXmin; x < boundXmax; ++x) {
                if (x < 0 || x >= framebufferW) continue;                

                // transforms cartesion coords into barycentric coordinates
                baryTest.Transform(x, y, 
                    &frag.bias0,
                    &frag.bias1,
                    &frag.bias2);

                // TODO: make more precise?
                //if (((frag.bias0 + frag.bias1 + frag.bias2) > 1.001f) ||
                //    ((frag.bias0 + frag.bias1 + frag.bias2) < .99f)) continue;
                if (frag.bias0 < 0.f ||
                    frag.bias1 < 0.f ||
                    frag.bias2 < 0.f) continue;


                frag.x = x;
                frag.y = io->GetFramebuffer()->Height() - y-1;

                fragments.push_back(frag);
            
            }
        }
    

    }

    // returns false if all tris are outside of
    // clipping space. That is, returns false if
    // all xyz of each vertex have a magnitude greater than 1
    bool PassesClipTest() {
        // TODO
        return true;
    }



    



    int framebufferW;
    int framebufferH;    

    uint8_t * srcV[3];



    std::vector<FragmentInfo> fragments;
    RuntimeIO * io;
    uint8_t count;
};








StageProcedure * SoftRaster::CreateRasterizer(Polygon p, DepthBuffering d) {
    return new RasterizeTriangles();
}
