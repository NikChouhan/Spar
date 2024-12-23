#include <fastgltf\core.hpp>

#include "ModelFast.h"
#include "Log.h"

bool Spar::ModelFast::LoadGltf(std::string path)
{
	fastgltf::Parser parser;

	auto data = fastgltf::GltfDataBuffer::FromPath(path);

	if (data.error() != fastgltf::Error::None)
	{
		Log::Error("[FastGltf] Failed to load the gltf file");
		return false;
	}

	auto asset = parser.loadGltf(data.get(), path, fastgltf::Options::None);

	if (auto error = asset.error(); error != fastgltf::Error::None)
	{
		Log::Error("[FastGltf] Failed to parse the gltf file");
		return false;
	}



}