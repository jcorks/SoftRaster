#ifndef H_SOFTRASTER_TEXTURE_INCLUDED
#define H_SOFTRASTER_TEXTURE_INCLUDED

#include <cstring>
#include <cstdint>

namespace SoftRaster {
class Texture {
  public:
    Texture();
    Texture(int w_, int h_, uint8_t * data_);
    ~Texture();


    inline int Width() {return w;} const
    inline int Height(){return h;} const

    uint8_t * GetData() {return data;}

    // Resize putting old texData in topleft
    void Resize(int newWidth, int newHeight);
    
    // Resize without copying old tex data
    void ResizeFast(int newWidth, int newHeight);

    // Replace old texture data with this data.
    void Redefine(uint8_t * data, int newWidth, int newHeight);

    enum class ColorAddRule {
        None,    // source overwrites destination
        Alpha,   // default
        Additive 
    };


    enum class ColorOverflowRule {
        None, // default: implementation dependent
        Wrap,
        Clamp
    };

    enum class SampleRule {
        Basic, // default
        LinearInterpolation
    };

    void SetColorRules(ColorAddRule, ColorOverflowRule);
    void SetSampleRule(SampleRule);



    
    // follows color rules
    void PutPixel  (int x, int y, uint8_t * pixel);
    void GetPixel  (int x, int y, uint8_t * pixel);
    void SamplePixel(float x, float y, uint8_t * pixel);


    // follows color Rules
    // texels outside the destination texel array are thrown out
    void PutTexture(int x, int y, int maxW, int maxH, Texture * t);
    

  private:

    int w, h;
    uint8_t * data;
};
}


#endif
