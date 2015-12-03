#include <SoftRaster/CoreProcedures.h>

using namespace SoftRaster;



// The built in stages are a little strange
// because they will often need to pass information that they don't use
//
//
//


// Rasterize triangles stage
// REQUIRED:
//  - First argument is Vector3
class RasterizeTriangles : public StageProcedure {
  public:
    uint32_t InputBytesToRead() {
        return 
            GetVertexSize();
    }


    uint32_t OutputBytesToWrite() {
        return 
            GetVertexSize() +
            sizeof(int)*2
    }


    void operator()(RuntimeIO * io) {

        static count = 0;
        static uint8_t * vInstance = nullptr;
        
        // initial setup()
        if (GetCurrentIteration() == 0) {
            NewRun();    
        }

        if (count != 2) {
            
        } else {
            
        }
    }
    

  private:
    void NewRun() {
        count = 0;
        if (vInstance) delete[] vInstance;  
        vInstance = new uint8_t[
            GetVertexSize()
        ];   
    }



    uint8_t count;
    uint8_t vInstance;
};
