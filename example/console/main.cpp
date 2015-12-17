/// Example usage of SoftRaster
///
/// Renders a triangle and dumps the output to standard out.


// basics.h holds our StageProcedures and some helper functions.
#include "../base/basics.h"
#include <iostream>
using namespace SoftRaster;

// This is the geometry for the render. Just a basic triangle.
// Since our vertices don't undergo projection of any kind, these
// are in homogenous coordinates (each coordinate spans from -1 to 1
// and is a clipping bound. For x and y, -1 marks the left and the top, 1
// makrs the right and bottom of the framebuffer.
Vertex Triangle[] = {
    { -1.f, -1.f, 0.f,    1.f, 1.f, 1.f, 1.f },
    { 1.f, -1.f, 0.f,    1.f, 1.f, 1.f, 1.f },
    { 0.f, 1.f, 0.f,    1.f, 1.f, 1.f, 1.f }
};

// RGBA pixel
uint8_t ColorBlack[] { 0, 0, 0, 0 };




int main() {

    // First, the context should be setup
    // framebuffer will be the texture written to when rendering
    Texture * framebuffer = new Texture(80, 64);

    // Defined in base/basics.cpp
    VertexShader   vShader;
    FragmentShader fShader;
    
    // Attach the framebuffer to the context
    Context context(framebuffer);

    // Form the pipeline.
    Pipeline pipeline;
    pipeline.PushExecutionStage(&vShader);
    pipeline.PushExecutionStage(
        CreateRasterizer(Polygon::Triangles)
    );
    pipeline.PushExecutionStage(&fShader);

    // Attach pipeline to the context to set up running.
    context.UseProgram(pipeline.Compile());





    // Rotate the transform. The transform is appllied to the input vertices in 
    // the vertex shader.
    vShader.modelview.RotateByAngles(45, 45, 45);

    
    // Set the framebuffer to a clear state.
    framebuffer->Clear(ColorBlack);

    // Draw.
    context.RenderVertices<Vertex>(Triangle, 3);


    // Dump the pixels to standard out.    
    uint8_t pixel[4];
    for(int y = 0; y < framebuffer->Height(); ++y) {
        for(int x = 0; x < framebuffer->Width(); ++x) {
            framebuffer->GetPixel(x, y, pixel);
            std::cout << pixelToChar(pixel);
        }
        std::cout << std::endl;
    }

    return 0;
}

