#ifndef IMAGE_OPS_H
#define IMAGE_OPS_H

#include "ExrImageExport.h"

#include <Amino/Cpp/Annotate.h> // for AMINO_ANNOTATE macro

#include <Amino/Core/Array.h>
#include <Amino/Core/Any.h>
#include <Amino/Core/Ptr.h>
#include <Amino/Core/String.h>
#include <Bifrost/Math/Types.h>
#include <Bifrost/Object/Object.h>


using Float4Array = Amino::Array<Bifrost::Math::float4>;
using Float3Array = Amino::Array<Bifrost::Math::float3>;
using Float2Array = Amino::Array<Bifrost::Math::float2>;

namespace utility {
	IMAGE_DECL
	void pixel_sampler(
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
    AMINO_ANNOTATE("Amino::Node, pack=ImagePack, name=::Image::Sample_Pixels, metadata={internal, string, true}");
}
namespace io {
IMAGE_DECL
void read_exr(
	Bifrost::Object& object AMINO_ANNOTATE("Amino::InOut outName=image"),
	const Amino::String& filename
)
AMINO_ANNOTATE("Amino::Node, pack=ImagePack, name=::Image::Read_Exr, metadata=[{icon, string, ../resources/icons/read_exr.png}, {documentation, string, ../resources/docs/read_exr.md}]");

IMAGE_DECL
void write_exr(
	const Amino::Ptr<Bifrost::Object>& image,
	Amino::String& filename AMINO_ANNOTATE("Amino::InOut outName=file_path")
)
AMINO_ANNOTATE("Amino::Node, pack=ImagePack, name=::Image::Write_Exr, metadata=[{icon, string, ../resources/icons/write_exr.png}, {documentation, string, ../resources/docs/write_exr.md}]");

}
#endif // IMAGE_OPS_H
