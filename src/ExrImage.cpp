#include "ExrImage.h"
#include <sstream>
#include <fstream>
#include <string>

#define IMATH_HALF_NO_LOOKUP_TABLE

#include <half.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>

#undef min
#undef max
#define INVALID_PIXEL_TYPE Imf::NUM_PIXELTYPES

inline float dot (
    const Bifrost::Math::float2& p1,
    const Bifrost::Math::float2& p2
    )
{
    return (p1.x * p2.x) + (p1.y * p2.y);
}

void utility::pixel_sampler(
    const Float4Array& pixels,
    const Amino::uint_t& in_width,
    const Amino::uint_t& in_height,
    const Bifrost::Math::float2& a,
    const Bifrost::Math::float2& b,
    const Bifrost::Math::float2& c,
    const Bifrost::Math::float3& pa,
    const Bifrost::Math::float3& pb,
    const Bifrost::Math::float3& pc,
    Amino::Ptr<Float3Array>& positions,
    Amino::Ptr<Float4Array>& colors
    )
{
    float xnorm = 1.0f / in_width;
    float ynorm = 1.0f / in_height;
    float xbias = (xnorm / 2.0f);
    float ybias = (ynorm / 2.0f);
    float left = std::min({a.x, b.x, c.x}) - xnorm;
    float right = std::max({a.x, b.x, c.x}) + xnorm;
    float top = std::max({a.y, b.y, c.y}) + ynorm;
    float bot = std::min({a.y, b.y, c.y}) - ynorm;
    int min_x = static_cast<int>(left * in_width);
    int max_x = static_cast<int>(right * in_width);
    int min_y = static_cast<int>(bot * in_height);
    int max_y = static_cast<int>(top * in_height);
    int size = (max_x - min_x) * (max_y - min_y);
    if (max_x >= in_width){max_x = in_width - 1;}
    if (max_y >= in_height){max_y = in_height - 1;}
    if (min_y < 0){min_y = 0;}
    if (min_x < 0){min_x = 0;}
    Float3Array point_array;
    Float4Array color_array;
    for (int y = min_y; y < max_y; y++){
        for (int x = min_x; x < max_x; x++){
            Bifrost::Math::float2 p;
            Bifrost::Math::float2 v0;
            Bifrost::Math::float2 v1;
            Bifrost::Math::float2 v2;
            p.x = (x*xnorm) + xbias;
            p.y = (y*ynorm) + ybias;
            v0.x = c.x - a.x;
            v0.y = c.y - a.y;
            v1.x = b.x - a.x;
            v1.y = b.y - a.y;
            v2.x = p.x - a.x;
            v2.y = p.y - a.y;
            float dot00 = dot(v0, v0);
            float dot01 = dot(v0, v1);
            float dot02 = dot(v0, v2);
            float dot11 = dot(v1, v1);
            float dot12 = dot(v1, v2);
            float inverse_denominator = 1.0f / ((dot00 * dot11) - (dot01 * dot01));
            const float u = ((dot11 * dot02) - (dot01 * dot12)) * inverse_denominator;
            if (u < 0.0f) {
                continue;
            }
            const float v = ((dot00 * dot12) - (dot01 * dot02)) * inverse_denominator;
            if (v < 0.0f){
                continue;
            }
            const float uv = u + v;
            if (uv > 1.0f){
                continue;
            }
            const float w = 1.0f - uv;
            Bifrost::Math::float3 point;
            //point.x = p.x;
            //point.y = p.y;
            point.x = (pa.x * w) + (pb.x * v) + (pc.x * u);
            point.y = (pa.y * w) + (pb.y * v) + (pc.y * u);
            point.z = (pa.z * w) + (pb.z * v) + (pc.z * u);
            point_array.push_back(point);
            Bifrost::Math::float4 color;
            const int pix = (y*in_width) + x;
            color.x = pixels[pix].x;
            color.y = pixels[pix].y;
            color.z = pixels[pix].z;
            color_array.push_back(color);
        }
    }
    positions = Amino::newClassPtr<Float3Array>(std::move(point_array)); 
    colors = Amino::newClassPtr<Float4Array>(std::move(color_array)); 
}

void  io::read_exr(
    Bifrost::Object& object,
    const Amino::String& filename
)
{
    Imf::RgbaInputFile* img_input;
    const char* f = filename.c_str();
    std::ifstream file(f);
    if (!file) {
        return;
    }
    img_input = new Imf::RgbaInputFile(f);

    // Get our region of interest (roi) from the header.
    const Imf::Header& header = img_input->header();
    const Imath::Box2i roi = header.dataWindow();
    int dx = roi.min.x;
    int dy = roi.min.y;
    int width = roi.max.x - dx + 1;
    int height = roi.max.y - dy + 1;
    object.setProperty("width", width);
    object.setProperty("height", height);

    // Populate an Imath image array with pixel data.
    Imf::Array<Imf::Rgba> pixels;
    pixels.resizeErase(width * height);
    img_input->setFrameBuffer(pixels - dx - dy * width, 1, width);
    img_input->readPixels(roi.min.y, roi.max.y);

    const int size = width * height;

    Float4Array new_array(size);
    int yr = height - 1;
    for (int y = 0; y < height; y++, yr--) {
        for (int x = 0; x < width; x++) {
            const int i = (y * width) + x;
            const int ir = (yr * width) + x;
            new_array[ir].x = pixels[i].r;
            new_array[ir].y = pixels[i].g;
            new_array[ir].z = pixels[i].b;
            new_array[ir].w = pixels[i].a;
        }
    }

    auto out_rgb = Amino::newClassPtr<Float4Array>(std::move(new_array));
    object.setProperty("rgba", out_rgb);
    delete img_input;// delete pixels;
}

void io::write_exr(
    const Amino::Ptr<Bifrost::Object>& image,
    Amino::String& filename
)
{
    bool hw = image->hasProperty("width");
    if (!hw)
        return;
    Amino::Any anyRgba = image->getProperty("rgba");
    Amino::Any anyWidth = image->getProperty("width");
    Amino::Any anyHeight = image->getProperty("height");

    auto width = Amino::any_cast<Amino::int_t>(anyWidth);
    auto height = Amino::any_cast<Amino::int_t>(anyHeight);
    auto rgbaPtr = Amino::any_cast<Amino::Ptr<Float4Array>>(anyRgba);
    Float4Array aminoArray = *rgbaPtr;

    const char* f = filename.c_str();
    Imf::RgbaOutputFile img_output(f, width, height, Imf::WRITE_RGBA);
    const int size = width * height;
    Imf::Rgba* pixels = new Imf::Rgba[size];

    int yr = height - 1;
    for (int y = 0; y < height; y++, yr--) {
        for (int x = 0; x < width; x++) {
            const int i = (y * width) + x;
            const int ir = (yr * width) + x;
            pixels[ir].r = aminoArray[i].x;
            pixels[ir].g = aminoArray[i].y;
            pixels[ir].b = aminoArray[i].z;
            pixels[ir].a = aminoArray[i].w;
        }
    }
    img_output.setFrameBuffer(pixels, 1, width);
    img_output.writePixels(height);
    delete pixels;// delete pixels;
}
