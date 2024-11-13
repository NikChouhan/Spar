#pragma once

#include "WinUtil.h"
#include "renderer.h"
namespace Spar
{
	class Shader
	{
	public:
		Shader()
		{

		}
		~Shader()
		{

		}

		HRESULT ProcessShaders(std::shared_ptr<Graphics::Renderer> renderer ,const WCHAR* vsShaderPath, const WCHAR* psShaderPath);

	private:
		HRESULT CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob);

	public:


	private:
		const WCHAR* path;


	};


}