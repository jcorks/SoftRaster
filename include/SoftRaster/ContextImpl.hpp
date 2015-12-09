// should never be included in anything except Context.h

// Context is more of a manager class;
// its not necessary to use it, but makes life a tad simpler.
// as a result, it doesn't actually own errr any references..
// (might be a sign for needed refactoring)
#include <SoftRaster/Primitives.h>
#include <cassert>




template<typename T>
void Context::RenderVertices(
        T * vertexData, 
        uint32_t num) {
    if (!program) return;
    // kind of a crude test. Would prefer some sort of preprocessor based check somehow
    T * testInst = new T;
    if (!dynamic_cast<Vector3 *>(testInst)) {
        assert(!"The SoftRaster::Context template vertex type must inherit from the primitive SoftRaster::Vertex3!");
    }
    delete testInst;


    program->Run(
        framebuffer, 
        (uint8_t*)vertexData,
        sizeof(T),
        num
    );         
}

template<typename T>
void Context::RenderVerticesIndexed(
        T * vertexArray, 
        uint32_t * indexList, 
        uint32_t numIndices) {

    if (!program) return;    

    // Only step here is to expand the vertex list
    // NOTE: this is functional, but not ideal...
    // Pre-allocated index array?
    T * coreList = new T[numIndices];
    for(uint32_t i = 0; i < numIndices; ++i) 
        memcpy(coreList+i, vertexArray+indexList[i], sizeof(T));

    RenderVertices<T>(coreList, numIndices);

    delete[] coreList;
}




