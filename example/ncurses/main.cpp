#include "../base/basics.h"
#include <iostream>
#include <ncurses.h>
#include <cmath>
using namespace SoftRaster;




Vertex Triangle[] = {
    { -.5f,  0.f, .3f,    .3f, .3f, .3f, 1.f },
    {  .5f,  0.f, .3f,    .3f, .3f, .3f, 1.f },
    {  0.f, 1.f,  .3f,    .3f, .3f, .3f, 1.f },

    { -.7f,  0.f, 0.f,    .7f, .7f, .7f, 1.f },
    {  .7f,  0.f, 0.f,    .7f, .7f, .7f, 1.f },
    {  0.f, 1.f,  0.f,    .7f, .7f, .7f, 1.f },

    { -.3f,  0.f, -.3f,    1.f, 1.f, 1.f, 1.f },
    {  .3f,  0.f, -.3f,    1.f, 1.f, 1.f, 1.f },
    {  0.f, 1.f,  -.3f,    1.f, 1.f, 1.f, 1.f },
};




uint8_t ColorBlack[] { 0, 0, 0, 0 };




int main() {
    Texture * framebuffer = new Texture(80, 60);
    
    VertexShader   vShader;
    FragmentShader fShader;
    
    Context context(framebuffer);
    Pipeline pipeline;
    pipeline.PushExecutionStage(&vShader);
    pipeline.PushExecutionStage(
        CreateRasterizer(Polygon::Triangles)
    );
    pipeline.PushExecutionStage(&fShader);
    context.UseProgram(pipeline.Compile());



    





    
    // ncurses behavior
    initscr();
    for(;;) {
        static float counter = 0.f;

        framebuffer->Clear(ColorBlack);

        vShader.modelview.SetToIdentity();
        vShader.modelview.RotateByAngles(counter, counter/10, counter/20);



        context.RenderVertices<Vertex>(Triangle, 9);
        //context.RenderVerticesIndexed<Vertex>(Tetra, TetraIndices, 12);
      

        

        

        uint8_t pixel[4];
        for(int y = 0; y < framebuffer->Height(); ++y) {
            for(int x = 0; x < framebuffer->Width(); ++x) {
                framebuffer->GetPixel(x, y, pixel);
                mvaddch(y, x, pixelToChar(pixel));
            }
        }

        counter += .05;
        mvprintw(0, 0, "%.2f",counter);
        refresh();
    }

}

