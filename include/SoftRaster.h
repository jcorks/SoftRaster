#ifndef H_SOFTRASTER_CONTEXT_INCLUDED
#define H_SOFTRASTER_CONTEXT_INCLUDED


/* 
    SoftRaster: A small software graphics rasterizer framework.
    Johnathan Corkery, 2015
 */


#include <SoftRaster/Texture.h>
#include <SoftRaster/Pipeline.h>


namespace SoftRaster {

/// \brief The driver of the rasterizer.
/// Results of a draw are always to a texture.
/// The context does not own the Texture, but needs a reference
/// to one to perform the draw.
/// 
class Context {
  public:
    Context(Texture * defaultFramebuffer);

    /// \brief Sets the texture to post rendered results to.
    ///
    void SetFramebuffer   (Texture *);

    /// \brief Sets the program to render with.
    /// See Pipeline
    ///
    void UseProgram(Pipeline::Program * program);

    /// \brief Sets the size of each vertex.
    /// the default is sizeof(float)*3
    ///
    void DefineVertexSize(uint32_t n);

    /// \brief Renders the given set of vertices according to the 
    /// Currently set pipeline program to the set framebuffer
    ///
    void RenderVertices(uint8_t * VertexArray, uint32_t num);

    /// \brief Renders the given set of indices. The indices
    /// refer to the i'th vertex in the initial vertexArray
    ///
    void RenderVerticesIndexed(uint8_t * VertexArray, uint32_t * indexList, uint32_t numIndices);



  private:

    uint32_t sizeofVertex;
    Texture * framebuffer;
    Pipeline::Program * program;    







};
}


#endif
