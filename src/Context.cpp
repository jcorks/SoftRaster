#include <SoftRaster/Context.h>

using namespace SoftRaster;

Context::Context(Texture * dfb) :
          program      (nullptr){
    SetFramebuffer(dfb);
}


void Context::SetFramebuffer(Texture * t) {
    framebuffer = t;
}

void Context::UseProgram(Pipeline::Program * p) {
    program = p;
}
