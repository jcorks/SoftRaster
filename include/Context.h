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
/// The template is the class representing the vertices containing
/// the information to perform the render.
/// 
template<typename UserVertexT>
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

    /// \brief Renders the given set of vertices according to the 
    /// Currently set pipeline program to the set framebuffer
    ///
    void RenderVertices(UserVertexT * VertexArray, uint32_t num);

    /// \brief Renders the given set of indices. The indices
    /// refer to the i'th vertex in the initial vertexArray
    ///
    void RenderVerticesIndexed(UserVertexT * VertexArray, uint32_t * indexList, uint32_t numIndices);



  private:

    Texture * framebuffer;
    Pipeline::Program * program;    

};
#include <SoftRaster/ContextImpl.h>
}


#endif
