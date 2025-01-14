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
struct ImGuiIO;

namespace Spar
{
	class Application
	{
	public:

		Application();
		~Application();

		void Init();
		void Run();
		void Render(float dt);
		void Resize();
		void ShutDown();
		void EditorMenu(float dt);
	public:
		//public member variables

		std::shared_ptr<Renderer> m_renderer;
		std::shared_ptr<Camera> m_camera;

		wrl::ComPtr<ID3D11RasterizerState> m_rasterState = nullptr;


		Model suzanne = {};

		std::vector<Model> models{};

	private:

		DirectX::XMFLOAT4 m_meshColor = DirectX::XMFLOAT4(0.7f, .7f, .7f, 1.f);
		ImGuiIO* io;
	};
}