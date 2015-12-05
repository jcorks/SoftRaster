#ifndef H_SOFTRASTER_TEXTURE_INCLUDED
#define H_SOFTRASTER_TEXTURE_INCLUDED

#include <cstring>
#include <cstdint>

namespace SoftRaster {

/// \brief A 32-bit image.
///
/// All pixel positions are relative to the top left of
/// the image and begin at 0. Any raw pixel access will
/// layout the data in one contiguous array in a fow major 
/// fashion
class Texture {
  public:
    


    /// \brief Denotes a rule to be followed the interaction between source and destination pixels
    ///
    enum class ColorAddRule {
        None,    ///< Source overwrites destination.
        Alpha,   ///< Source blends with destination accoring to alpha values. This is the default 
        Additive ///< Source adds to destination for all channels uniformly.
    };


    /// \brief Denotes the rule to be followed when retrieving pixel data
    ///
    enum class SampleRule {
        Basic,               ///< The pixel nearest to the sample is returned.
        LinearInterpolation  ///< The pixel returned is blended with bordering pixel colors uniformly.
    };

    /// \brief Denotes a data format.
    ///
    enum class Format {
        RGB,       ///< Only the Red, Green, and Blue channels are written respectively. GetAsFormat() expects the input buffer to be Width()*Height()*3 bytes.
        BlendedRGB,///< Same as RGB, but the alpha saturation of each pixel is multiplied to the other color channels. GetAsFormat() expects the input buffer to be Width()*Height()*3
        Grayscale, ///< Each pixel is the average of all the cahnnels saturation. Only one byte of information is written for each pixel. GetAsFormat() expect the input buffer to be Width()*Height() bytes.
    };



    /// \brief Allocates space for the Texture and initializes with the data given, if any.
    ///
    ///@param w_ Width of the image
    ///@param h_ Height of the image
    ///@data_  Source data. If null, the space for the image is allocated, but not set.
    Texture(uint16_t w_, uint16_t h_, uint8_t * data_=nullptr);
    Texture(const Texture & t);
    ~Texture();

    /// \brief Returns the width of the image.
    ///
    inline uint16_t Width() const {return w;}

    /// \brief Returns the height of the image.
    ///
    inline uint16_t Height() const {return h;}

    /// \brief Returns a raw pointer to the data.
    ///
    /// The data is layed out in a linear format and is Width()*Height()*4 bytes in size.
    /// The first byte is the top left pixel and the last byte is the bottom right pixel of the image.
    /// The data is also editable.
    inline uint8_t * GetData() {return data;}

    /// \brief Resize putting old texData in topleft.
    ///
    void Resize(uint16_t newWidth, uint16_t newHeight);
    
    /// \brief Resize without copying old data.
    ///
    /// Once called, the Texture's pixel data is undefined.
    void ResizeFast(uint16_t newWidth, uint16_t newHeight);

    /// \brief Replace old texture data with this data.
    ///
    Texture & operator=(const Texture & t);

    

    /// \brief Sets how pixels should interact when editing pixel data.
    ///
    void SetColorRules(ColorAddRule);

    /// \brief Sets how pixels should be read.
    ///
    void SetSampleRule(SampleRule);



    /// \brief Edits the pixel at the given position following the set rules set by SetColorRules().
    ///
    /// Bounds checking should be handled by the caller.
    ///\{
    void PutPixel  (uint16_t x, uint16_t y, const uint8_t * pixel);
    void PutPixel  (uint16_t x, uint16_t y, const Color * pixel);
    ///\}

    /// \brief Returns the pixel at the given position according to the sampling rule set by SetSamleRule().
    ///
    /// Bounds checking should be handled by the caller.
    ///\{
    void GetPixel  (uint16_t x, uint16_t y, const uint8_t * pixel);
    void GetPixel  (uint16_t x, uint16_t y, const Color * pixel);
    ///\}
    
    /// \brief Equivalent to GetPixel() but the position is a normalized amount from 0.f to 1.f. 
    ///
    /// The sampling positions are clamped.
    ///\{
    void SamplePixel(float x, float y, const uint8_t * pixel);
    void SamplePixel(float x, float y, const Color * pixel);
    ///\}

    /// \brief Writes a texture to this texture.
    ///
    /// (x, y) mark the top left of where the source image 
    /// will be placed on the destination image. Pixel blending 
    /// follows set color rules and pixels outside the destination 
    /// Texture are thrown out
    void PutTexture(uint16_t x, uint16_t y, uint16_t maxW, uint16_t maxH, Texture * src);
    
    /// \brief Returns the current stored image in different formats. See Format for details.
    ///
    void GetAsFormat(Format, uint8_t *);
  private:

    uint16_t w, h;
    uint8_t * data;

    (uint8_t *)(*ColorTransform)(uint8_t*src,uint8_t*dest);
    (uint8_t *)(*SampleTransform)(uint16_t, uint16_t, Texture*);

    ColorTransform       carule;
    SampleTransform      sarule;
};
}


#endif
