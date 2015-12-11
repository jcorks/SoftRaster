#include <SoftRaster/Texture.h>
#include <algorithm>

using namespace SoftRaster;

static uint8_t * ColorAdd_None    (const uint8_t *, const uint8_t *);
static uint8_t * ColorAdd_Alpha   (const uint8_t *, const uint8_t *);
static uint8_t * ColorAdd_Additive(const uint8_t *, const uint8_t *);



static uint8_t * SampleRule_Basic(uint16_t, uint16_t, Texture *);
static uint8_t * SampleRule_LI(uint16_t, uint16_t, Texture *);


struct Color32 {uint8_t r; uint8_t g; uint8_t b; uint8_t a;};


Texture::Texture(uint16_t w_, uint16_t h_, uint8_t * data_) {
    data = new uint8_t[w_*h_*4];
    if (data_) {
        memcpy(data, data_, w_*h_*4);
    }
    w = w_;
    h = h_;
    carule = ColorAdd_Alpha;
    sarule = SampleRule_Basic;
}

Texture::Texture(const Texture & t) {
    *this = t;
}

Texture::~Texture() {
    delete[] data;
}




void Texture::Resize(uint16_t newWidth, uint16_t newHeight) {
    uint8_t * newData = new uint8_t[newWidth * newHeight * 4];

    // careful not to exceed any limits;
    uint16_t limitHeight = std::min(newHeight, h);
    uint16_t limitWidth  = std::min(newWidth, w);
    for(uint16_t y = 0; y < limitHeight; ++y) {
        memcpy(newData+newWidth*y, data, limitWidth);
    }


    delete[] data;
    data = newData;
    w = newWidth;
    h = newHeight;
}



void Texture::ResizeFast(uint16_t newWidth, uint16_t newHeight) {
    delete[]data;
    data = new uint8_t[newWidth*newHeight*4];
    w = newWidth;
    h = newHeight;
}


Texture & Texture::operator=(const Texture & t) {
    delete[] data;
    w = t.w;
    h = t.h;
    carule = t.carule;
    sarule = t.sarule;
    data = new uint8_t[w*h*4];
    memcpy(data, t.data, w*h*4);
    return *this;
}   



void Texture::SetColorRules(ColorAddRule ca) {
    switch(ca) {
      case ColorAddRule::None:      carule = ColorAdd_None;    break;
      case ColorAddRule::Alpha:     carule = ColorAdd_Alpha;   break;
      case ColorAddRule::Additive:  carule = ColorAdd_Additive;break;
    }
}

void Texture::SetSampleRule(SampleRule s) {
    switch(s) {
      case SampleRule::Basic:               sarule = SampleRule_Basic; break;
      case SampleRule::LinearInterpolation: sarule = SampleRule_LI;    break;
    }
}




void Texture::PutPixel(uint16_t x, uint16_t y, const uint8_t * src) {
    memcpy(data+4*(x+y*w), carule(src, data+4*(x+y*w)), 4);   
}

void Texture::PutPixel(uint16_t x, uint16_t y, const Color * srcC) {
    static Color32 src;
    src.r = srcC->r*UINT8_MAX;
    src.g = srcC->g*UINT8_MAX;
    src.b = srcC->b*UINT8_MAX;
    src.a = srcC->a*UINT8_MAX;
    memcpy(data+4*(x+y*w), carule((uint8_t*)&src, data+4*(x+y*w)), 4);
}

void Texture::GetPixel(uint16_t x, uint16_t y, uint8_t * src) {
    memcpy(src, data+4*(x+y*w), 4);
}

void Texture::GetPixel(uint16_t x, uint16_t y, Color * src) {
    src->r = (*(data+4*(x+y*w+0)))/(float)UINT8_MAX;
    src->g = (*(data+4*(x+y*w+1)))/(float)UINT8_MAX;
    src->b = (*(data+4*(x+y*w+2)))/(float)UINT8_MAX;
    src->a = (*(data+4*(x+y*w+3)))/(float)UINT8_MAX;
}



void Texture::SamplePixel(float x, float y, uint8_t * src) {
    GetPixel(
        std::max(std::min(x, 1.f), 0.f)*w,
        std::max(std::min(x, 1.f), 0.f)*h,
        src
    );
}

void Texture::SamplePixel(float x, float y, Color * src) {
    GetPixel(
        std::max(std::min(x, 1.f), 0.f)*w,
        std::max(std::min(x, 1.f), 0.f)*h,
        src
    );
}










void Texture::PutTexture(uint16_t x_, uint16_t y_, uint16_t maxX, uint16_t maxY, Texture * t) {
    uint16_t srcX = 0;
    uint16_t srcY = 0;
    for(uint16_t y = y_; y < maxY && srcY < t->h; ++y, ++srcY) {
        for(uint16_t x = x_; x < maxX && srcX < t->w; ++x, ++srcX) {
            if (x < 0 || x >= w ||
                y < 0 || y >= h
                ) continue;

            PutPixel(x, y, t->GetData()+4*(srcX + srcY*t->w));
        }
    }
}

void Texture::GetAsFormat(Format fmt, uint8_t * out) {
    if (fmt == Format::RGB) {
        for(uint16_t y = 0; y < h; ++y) {
            for(uint16_t x = 0; x < w; ++x) {
                *(out+(x+y*w)*3+0) = *(data+(x+y*w)*4+0);
                *(out+(x+y*w)*3+1) = *(data+(x+y*w)*4+1);
                *(out+(x+y*w)*3+2) = *(data+(x+y*w)*4+2);
            }
        }
    } else if (fmt == Format::BlendedRGB) {
        float alpha;
        for(uint16_t y = 0; y < h; ++y) {
            for(uint16_t x = 0; x < w; ++x) {
                alpha = *(data+(x+y*w)*4+3);
                *(out+(x+y*w)*3+0) = *(data+(x+y*w)*4+0) * alpha;
                *(out+(x+y*w)*3+1) = *(data+(x+y*w)*4+1) * alpha;
                *(out+(x+y*w)*3+2) = *(data+(x+y*w)*4+2) * alpha;
            }
        }
    } else {
        float val;
        for(uint16_t y = 0; y < h; ++y) {
            for(uint16_t x = 0; x < w; ++x) {
                val = *(data+(x+y*w)*4+0) + *(data+(x+y*w)*4+1) + 
                      *(data+(x+y*w)*4+2) + *(data+(x+y*w)*4+3);
                *(out+(x+y*w)) = UINT8_MAX*(val / 4.f);
            }
        }
    }
}


void Texture::Clear(uint8_t * src) {
    uint32_t clr = *((uint32_t*)src);
    memset(data, clr, w*h*4);
}






//////////// statics
uint8_t * ColorAdd_None(const uint8_t * src, const uint8_t * dest) {
    static Color32 out;
    out = *((Color32*)src);
    return (uint8_t *)&out;
}


uint8_t * ColorAdd_Alpha(const uint8_t * src, const uint8_t * dest) {
    static Color32 out;
    static float alpha;
    alpha = (src[3] / (float)UINT8_MAX);
    out.r = dest[0] + src[0]*alpha;
    out.g = dest[1] + src[1]*alpha;
    out.b = dest[2] + src[2]*alpha;
    out.a = dest[3] + src[3]*alpha;
    return (uint8_t *)&out;
}

uint8_t * ColorAdd_Additive(const uint8_t * src, const uint8_t * dest) {
    static Color32 out;
    out.r = src[0] + dest[0];
    out.g = src[1] + dest[1];
    out.b = src[2] + dest[2];
    out.a = src[3] + dest[3];    
    return (uint8_t *)&out;
}



uint8_t * SampleRule_Basic(uint16_t x, uint16_t y, Texture * t) {
    static Color32 out;
    out.r = *(t->GetData() + (x+(y*t->Width()))*4);
    out.g = *(t->GetData() + (x+(y*t->Width())+1)*4);
    out.b = *(t->GetData() + (x+(y*t->Width())+2)*4);
    out.a = *(t->GetData() + (x+(y*t->Width())+3)*4);
    return (uint8_t *)&out;
}


//TODO
uint8_t * SampleRule_LI(uint16_t x, uint16_t y, Texture * t) {
    static Color32 out;
    out.r = *(t->GetData() + (x+(y*t->Width()))*4);
    out.g = *(t->GetData() + (x+(y*t->Width())+1)*4);
    out.b = *(t->GetData() + (x+(y*t->Width())+2)*4);
    out.a = *(t->GetData() + (x+(y*t->Width())+3)*4);
    return (uint8_t *)&out;
}









