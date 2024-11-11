#pragma once

#include "WinUtil.h"
#include "Camera.h"
#include <string>
#include <sstream>
#include "renderer.h"
#include "DDSTextureLoader.h"
#include "Shader.h"
#include "Buffer.h"

namespace Spar
{
	class Application
	{
	public:

		Application();
		~Application();

		void Init();
		void Run();
		void Update(float dt);
		void Resize();
		void Render();
		void ShutDown();
	protected:
		Graphics::Renderer* GetRenderer() const { return m_renderer.get(); }

	public:
		//public member variables

		std::unique_ptr<Graphics::Renderer> m_renderer = std::make_unique<Graphics::Renderer>();

	private:
		wrl::ComPtr<ID3D11Buffer> m_CBNeverChanges = nullptr;
		wrl::ComPtr<ID3D11Buffer> m_CBChangesOnResize = nullptr;
		wrl::ComPtr<ID3D11Buffer> m_CBChangesEveryFrame = nullptr;
		wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;
		wrl::ComPtr<ID3D11SamplerState> m_samplerLinear = nullptr;

		DirectX::XMMATRIX m_world;
		DirectX::XMMATRIX m_view;
		DirectX::XMMATRIX m_projection;

		wrl::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;

		DirectX::XMFLOAT4 m_meshColor = DirectX::XMFLOAT4(0.7f, .7f, .7f, 1.f);

	};
}