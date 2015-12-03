#ifndef H_SOFTRASTER_TEXTURE_INCLUDED
#define H_SOFTRASTER_TEXTURE_INCLUDED

#include <cstring>
#include <cstdint>

namespace SoftRaster {
class Texture {
  public:
    Texture();
    Texture(uint16_t w_, uint16_t h_, uint8_t * data_);
    ~Texture();


    inline uint16_t Width() {return w;} const
    inline uint16_t Height(){return h;} const

    uint8_t * GetData() {return data;}

    // Resize putting old texData in topleft
    void Resize(uint16_t newWidth, uint16_t newHeight);
    
    // Resize without copying old tex data
    void ResizeFast(uint16_t newWidth, uint16_t newHeight);

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
    void PutPixel  (uint16_t x, uint16_t y, uint8_t * pixel);


    void GetPixel  (uint16_t x, uint16_t y, uint8_t * pixel);
    void SamplePixel(float x, float y, uint8_t * pixel);


    // follows color Rules
    // texels outside the destination texel array are thrown out
    void PutTexture(uint16_t x, uint16_t y, uint16_t maxW, uint16_t maxH, Texture * t);
    

  private:

    uint16_t w, h;
    uint8_t * data;
};
}


#endif
