#include <SoftRaster/CoreProcedures.h>

using namespace SoftRaster;



// converts from cartesian coords to barycentric coords
class BarycentricTransform {
  public:
    BarycentricTransform(Vertex3 * v0, Vertex3 * v1, Vertex3 * v2, int w, int h) {
        cartV0x = w * (v0.x+1)/2.f;
        cartV0y = w * (v0.y+1)/2.f;

        cartV1x = w * (v1.x+1)/2.f;
        cartV1y = w * (v1.y+1)/2.f;

        cartV2x = w * (v2.x+1)/2.f;
        cartV2y = w * (v2.y+1)/2.f;

        float baryTrans[4];

        baryTrans[0] = (cartV0x - cartV2x);
        baryTrans[1] = (cartV1x - cartV2x);
        baryTrans[2] = (cartV0y - cartV2y);
        baryTrans[3] = (cartV1y - cartV3y);

        // calc inverse

        float det = 1 / (baryTrans[0]*baryTrans[3] - baryTrans[1]*baryTrans[2]);
        inverseBar[0] =  (baryTrans[3] * det);
        inverseBar[1] = -(baryTrans[1] * det);
        inverseBar[2] =  (baryTrans[2] * det);
        inverseBar[3] = -(baryTrans[0] * det);
    }


    
    // converts xy into "biases" woards each vertex
    void Transform(int x, int y, float * b0, float * b1, float * b2) {
        static float inVec[2];
        inVec[0] = x - cartV2.x;
        inVec[1] = y - cartV2.y;

        *b0 = inverseBar[0]*inVec[0] + inverseBar[1]*inVec[1];
        *b1 = inverseBar[2]*inVec[0] + inverseBar[3]*inVec[1];
        *b3 = 1 - *b0 - *b1;
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
    SignatureIO InputSignature() {
        SignatureIO input;

        input.AddSlot(DataType::UserVertex);
    
        return input;
    }


    SignatureIO OutputSignature() {
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


    void operator()(Pipeline::Program::RuntimeIO * io_) {
        io = io_;

        // initial setup()
        if (io->GetCurrentIteration() == 0) {
            NewRun(io->SizeOf(DataType::UserVertex));    
        }
        

        if (count != 2) {
            PushVertex(io->ReadNext());
        } else {
            if (!PassesClipTest()) return;
        
            // Populates fragments
            PopulateFragments();

    
            // commit frags
            FragmentInfo * out;
            for(int i = 0; i < fragments.size(); ++i) { 
                out = &fragment[i];
                io->WriteNext(&out->x);
                io->WriteNext(&out->x);

                io->WriteNext(&out->bias0);
                io->WriteNext(&out->bias1);
                io->WriteNext(&out->bias2);
            
                io->WriteNext(srcV0);
                io->WriteNext(srcV1);
                io->WriteNext(srcV2);
    
                io->Commit();
            }
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
        if (srcV0) delete[] srcV0;  
        if (srcV1) delete[] srcV1;  
        if (srcV2) delete[] srcV2;  
        srcV0 = new uint8_t[io->Sizeof(DataType::UserVertex)];
        srcV1 = new uint8_t[io->Sizeof(DataType::UserVertex)];
        srcV2 = new uint8_t[io->Sizeof(DataType::UserVertex)];   

        framebufferW = io->GetFramebuffer()->GetW();
        framebufferH = io->GetFramebuffer()->GetH();
    }

    inline void PushVertex(uint8_t * data) const {
        memcpy(vInstance+GetVertexSize()*count, data, GetVertexSize());
    };


    // Rasterization of the triangle 
    // by testing if fragments lie within the triangle
    // using barycentric coordinates
    void PopulateFragments() {
        fragments.clear();

        static Vertex3 v0, v1, v2;
        static FragmentInfo frag;
        int boundXmin, boundXmax,
            boundYmin, boundYmax;
    



        v0 = *((Vertex3*)srcV0);
        v1 = *((Vertex3*)srcV1);
        v2 = *((Vertex3*)srcV2);

        // prepares the barycentric transfrom
        // used to test whether or not points are within the triangle
        // and to produce the varying biases. SO USEFUL
        BarycentricTransform baryTest(&v0, &v1, &v2
                                      framebufferW, framebufferH              
                                      );
        

        // first lets decide what texels should even be considered
        // A superset of the texels to test would be the bounding box of the triangle
        boundXmin = framebufferW * (std::min(std::min(std::min, v0.x), v1.x), v2.x))+1)/2.f;
        boundYmin = framebufferH * (std::min(std::min(std::min, v0.y), v1.y), v2.y))+1)/2.f;
        boundXmax = framebufferW * (std::max(std::max(std::max, v0.x), v1.x), v2.x))+1)/2.f;
        boundYmax = framebufferH * (std::max(std::max(std::max, v0.y), v1.y), v2.y))+1)/2.f;
        
                
        
        for(int y = boundYmin; y < boundYmax; ++y) {
            for(int x = boundXmin; x < boundXmax; ++x) {
    
                if (x < 0 || x >= framebufferW ||
                    y < 0 || y <= framebufferH) continue;                

                // transforms cartesion coords into barycentric coordinates
                baryTest.Transform(x, y, 
                    &frag.bias0,
                    &frag.bias1,
                    &frag.bias2);

                // TODO: make more precise?
                if ((frag.bias0 + frag.bias1 + frag.bias2) > 1.001f |
                    (frag.bias0 + frag.bias1 + frag.bias2) < .99f) continue;

                frag.x = x;
                frag.y = y;

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

    uint8_t * srcV0;
    uint8_t * srcV1;
    uint8_t * srcV2;



    std::vector<FragmentInfo> fragments;
    Pipeline::Program::RuntimeIO * io;
    uint8_t count;
};
