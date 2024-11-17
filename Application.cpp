#include "Application.h"

Spar::Application::Application()
{
    m_renderer = std::make_shared<Graphics::Renderer>();
    m_camera = std::make_shared<Graphics::Camera>();
}

Spar::Application::~Application()
{
    ShutDown();
}

void Spar::Application::Init()
{
    m_renderer->Init();

    m_camera->InitAsPerspective(90, m_renderer->m_width, m_renderer->m_height);
    m_camera->SetPosition({ 0.0f, 0.0f, -2.8f });

    //shader stuff
    Shader shader;
    const WCHAR* vsShaderPath = L"../../../../assets/shaders/VS_shader.hlsl";
    const WCHAR* psshaderPath = L"../../../../assets/shaders/PS_shader.hlsl";

    shader.ProcessShaders(m_renderer, vsShaderPath, psshaderPath);

    m_world = DirectX::XMMatrixIdentity();

    // Initialize the view matrix

    D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthClipEnable = true;

	// Create the rasterizer state object
	m_renderer->m_device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf());
    m_renderer->m_context->RSSetState(m_rasterState.Get());

    m_view = m_camera->GetViewMatrix().Transpose();
}

void Spar::Application::Run()
{
    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        // Handle events
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;
        }

        // Calculate delta time
        static float lastFrame = 0.0f;
        float currentFrame = SDL_GetTicks() / 1000.0f;
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update game state
        Update(deltaTime);

        // Render frame
        Render();
    }
}

void Spar::Application::Update(float dt)
{
    static float angle = 0.0f;
    angle += dt;
}

void Spar::Application::Render()
{
    m_renderer->Clear();

    models.push_back(textureCube);

    for (auto& model : models)
    {
        m_renderer->Submit(model);
    }

    m_renderer->Present();

}

void Spar::Application::Resize()
{

}

void Spar::Application::ShutDown()
{

}
