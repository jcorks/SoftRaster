#ifndef H_SOFTRASTER_CONTEXT_INCLUDED
#define H_SOFTRASTER_CONTEXT_INCLUDED


/* 
    SoftRaster: A small software graphics rasterizer framework.
    Johnathan Corkery, 2015
 */


#include <SoftRaster/Texture.h>
#include <SoftRaster/Pipeline.h>


namespace SoftRaster {

/// \brief The driver of the rendering process.
///
/// The Context is a high-level interface that manages the rendering state.
/// It routes input vertex information to the desired Pipeline::Program then commits changes to an attached Texture.
/// As such, the results of a render are always to a Texture object.
/// The context does not own the Texture, but needs an alive reference
/// to one to perform the draw. 
///
/// Information to be rendered is passed into the Context
/// as a user type. The only restrict is that the user type must inherit from 
/// Vector3. User vertices are supplied in array form and are copied 
/// into the propgram before running, so the pointer to the vertices
/// does not need to be kept valid after you're done drawing.
///
class Context {
  public:
    Context(Texture * defaultFramebuffer);

    /// \brief Sets the framebuffer of the context.
    /// 
    /// The framebuffer is the Texture accessible to the Pipeline::Program
    /// when rendering vertices.
    ///
    void SetFramebuffer   (Texture *);

    /// \brief Sets the program to render with.
    /// See Pipeline and Pipeline::Program
    ///
    void UseProgram(Pipeline::Program * program);

    /// \brief Renders the given set of vertices according to the 
    /// currently set parameters. 
    ///
    /// UserVertexT must inherit from Vector3.
    ///
    template<typename UserVertexT>
    void RenderVertices(UserVertexT * VertexArray, uint32_t num);

    /// \brief Renders the given set of indices. The indices
    /// refer to the i'th vertex in the initial vertexArray
    ///
    /// UserVertexT must inherit from Vector3.
    ///
    template<typename UserVertexT>
    void RenderVerticesIndexed(UserVertexT * VertexArray, uint32_t * indexList, uint32_t numIndices);



  private:

    Texture * framebuffer;
    Pipeline::Program * program;    

};
#include <SoftRaster/ContextImpl.hpp>
}


#endif
