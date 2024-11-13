#include "Shader.h"

namespace Spar
{

	HRESULT Shader::CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob)
	{
		if (!fileName || !entryPoint || !shaderModel)
		{
			return E_INVALIDARG;
		}

		*blob = nullptr;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
			flags |= D3DCOMPILE_DEBUG;
	#endif

		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			nullptr, nullptr
		};

		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(fileName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint, shaderModel,
			flags, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}

		*blob = shaderBlob;

		return hr;
	}

	HRESULT Shader::ProcessShaders(std::shared_ptr<Graphics::Renderer> renderer, const WCHAR* vsShaderPath, const WCHAR* psShaderPath)
	{
		ID3DBlob* pVSBlob = nullptr;
		CompileShader(vsShaderPath, "VSMain", "vs_4_0", &pVSBlob);

		HRESULT hr1 = renderer->m_device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, renderer->m_vertexShader.GetAddressOf());

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		UINT numElements = ARRAYSIZE(layout);

		renderer->m_device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), renderer->m_vertexLayout.GetAddressOf());

		renderer->m_context->IASetInputLayout(renderer->m_vertexLayout.Get());

		ID3DBlob* pPSBlob = nullptr;

		CompileShader(psShaderPath, "PSMain", "ps_4_0", &pPSBlob);

		HRESULT hr2 = renderer->m_device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, renderer->m_pixelShader.GetAddressOf());

		if (!hr1 && !hr2)
			return E_FAIL;

		return S_OK;
	}
};