#include <SoftRaster/SoftRaster.h>


using namespace SoftRaster;

// Context is more of a manager class;
// its not necessary to use it, but makes life a tad simpler.
// as a result, it doesn't actually own errr any references..
// (might be a sign for needed refactoring)

Context::Context(Texture * dfb) 
        : sizeofVertex (0),
          program      (nullptr){
    SetFramebuffer(t);
}




void Context::SetFramebuffer(Texture * t) {
    texture = t;
}


void Context::UseProgram(Pipeline::Program * p) {
    program = p;
}


void Context::DefineVertexSize(uint32_t n) {
    sizeofVertex = n;
}


void Context::RenderVertices(
        uint8_t * vertexData, 
        uint32_t num) {
    if (!sizeofVertex || !program) return;

    program->Run(
        texture, 
        vertexData,
        sizeofVertex
    );         
}


void Context::RenderVerticesIndexed(
        uint8_t * vertexArray, 
        uint32_t * indexList, 
        uint32_t numIndices) {

    if (!sizeofVertex || !program) return;    

    // Only step here is to expand the vertex list
    // NOTE: this is functional, but not ideal...
    // Pre-allocated index array?
    uint8_t * coreList = new uint8_t[numIndices*sizeofVertex];
    for(uint32_t i = 0; i < numIndices; ++i) 
        memcpy(coreList+i, vertexArray+indexList[i], sizeofVertex);

    RenderVertices(coreList, numIndices);

    delete[] coreList;
}




