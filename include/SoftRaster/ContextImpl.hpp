// should never be included in anything except Context.h

// Context is more of a manager class;
// its not necessary to use it, but makes life a tad simpler.
// as a result, it doesn't actually own errr any references..
// (might be a sign for needed refactoring)
template<typename T>
Context<T>::Context(Texture * dfb) 
        : sizeofVertex (0),
          program      (nullptr){
    // kind of a crude test. Would prefer some sort of preprocessor based check somehow
    T * testInst = new T;
    if (!dynamic_cast<Vertex3 *>(testInst)) {
        assert(!"The SoftRaster::Context template vertex type must inherit from the primitive SoftRaster::Vertex3!");
    }
    delete testIst;
    SetFramebuffer(t);
}


template<typename T>
void Context<T>::SetFramebuffer(Texture * t) {
    texture = t;
}

template<typename T>
void Context<T>::UseProgram(Pipeline::Program * p) {
    program = p;
}


template<typename T>
void Context<T>::RenderVertices(
        T * vertexData, 
        uint32_t num) {
    if (!sizeofVertex || !program) return;

    program->Run(
        texture, 
        vertexData,
        sizeof(T)
    );         
}

template<typename T>
void Context<T>::RenderVerticesIndexed(
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

    RenderVertices(coreList, numIndices);

    delete[] coreList;
}




