#pragma once

#include "WinUtil.h"
#include "Camera.h"
#include <string>
#include <sstream>
#include "renderer.h"

namespace Spar
{
	class Application
	{
	public:

		Application();
		~Application();

		void Init();
		void Update();
		void Render();
		void ShutDown();
	protected:
		inline Graphics::Renderer* GetRenderer() const { return m_renderer.get(); }

	public:
		//public member variables

		std::unique_ptr<Graphics::Renderer> m_renderer = std::make_unique<Graphics::Renderer>();

	};
}