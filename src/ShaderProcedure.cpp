#include "ShaderProcedure.h"




//example shader procedures

class VertexShader : public ShaderProcedure {

    uint32_t InputBytesToRead() {
        return sizeof(Vertex3) + sizeof(Vertex4);
    }

    uint32_t OutputBytesToWrite() {
        return sizeof(Vertex3) + sizeof(Vertex4);
    }



    void operator()(RuntimeIO * io) {
        Vertex3 * v     = io->Read<Vertex3>();
        Vertex4 * color = io->Read<Vertex4>();

        v = transform * (*v);

        io->Commit<Vertex3>(v);
        io->Commit<Vertex4>(color);
    
    }

    TransformMatrix transform;
}:



class TriangleRasterizer : public ShaderProcedure {

    uint32_t InputBytesToRead() {
        return sizeof(Vertex3) + sizeof(Vertex4);
    }

    uint32_t OutputBytesToWrite() {
        return sizeof(Texture*);
    }


    void operator()(RuntimeIO * io) {
        static count = 0;
        static VertexPC* v[3]; // VertexPC = {Vertex3 + Vertex4};

        if (count<3) {

            v[count] = io->Read<VertexPC>();
            // clipping and such could be here
            count++;
        } else {
            RasterizeTri(v);
        }

        // only write one iters' worth of data: the assembled texture
        if (IsLastIteration()) 
            io->Write<Texture*>(&out);
    }

    Texture * texture;

    void RasterizeTri(VertexPC * v) {
        // fancy pants logic that i dont know YET here
    }
    
};




class VertexShader : public ShaderProcedure {

    uint32_t InputBytesToRead() {
        return sizeof(Vertex3) + sizeof(Vertex4);
    }

    uint32_t OutputBytesToWrite() {
        return sizeof(Vertex3) + sizeof(Vertex4);
    }

    void operator()(RuntimeIO * runtime) {
        Vertex3 * v     = runtime->Read<Vertex3>();
        Vertex4 * color = runtime->Read<Vertex4>();

        v = transform * (*v);


        
        runtime->Commit<Vertex3>(v);
        runtime->Commit<Vertex4>(color);
    
    }

    TransformMatrix transform;
}:




