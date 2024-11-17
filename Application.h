#pragma once

#include "WinUtil.h"
#include "Camera.h"
#include <string>
#include <sstream>
#include "Shader.h"
#include "Buffer.h"
#include "Model.h"

namespace Spar
{
	class Camera;
	class Renderer;
}

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
	public:
		//public member variables

		std::shared_ptr<Renderer> m_renderer;
		std::shared_ptr<Camera> m_camera;

		wrl::ComPtr<ID3D11RasterizerState> m_rasterState = nullptr;


		Model suzanne = {};

		std::vector<Model> models{};

	private:
		wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;
		wrl::ComPtr<ID3D11SamplerState> m_samplerLinear = nullptr;

		DirectX::XMMATRIX m_world;
		DirectX::XMMATRIX m_view;
		DirectX::XMMATRIX m_projection;

		wrl::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;

		DirectX::XMFLOAT4 m_meshColor = DirectX::XMFLOAT4(0.7f, .7f, .7f, 1.f);

	};
}