//#include "imgui.h"
//#include "imgui_impl_sdl2.h"
//#include "imgui_impl_dx11.h"

#include "Application.h"
#include "renderer.h"
#include "Camera.h"
#include "Log.h"

#include <SDL2\SDL.h>



Spar::Application::Application()
{
    m_renderer = std::make_shared<Renderer>();
    m_camera = std::make_shared<Camera>();
    io = nullptr;
}

Spar::Application::~Application()
{
    ShutDown();
}

void Spar::Application::Init()
{
    Log::Init();
    m_renderer->Init();
    // camera setup
    m_camera->InitAsPerspective(90.0f, m_renderer->m_width, m_renderer->m_height);
    m_camera->SetPosition({0.0f, 0.0f, -6.f});
    // shader stuff
    Shader shader;
    const WCHAR *vsShaderPath = L"../../../../assets/shaders/Model/ModelVS.hlsl";
    const WCHAR *psshaderPath = L"../../../../assets/shaders/Model/ModelPS.hlsl";
    shader.ProcessShaders(m_renderer, vsShaderPath, psshaderPath);
    // Load model
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/Cube/cube.glTF");
    //suzanne.LoadModel(m_renderer, m_camera,"../../../../assets/models/sponza/Sponza.glTF");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/scifi/SciFiHelmet.gltf");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/suzanne/Suzanne.gltf");

   suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/balls/MetalRoughSpheres.gltf");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/flighthelmet/FlightHelmet.gltf");


    models.push_back(suzanne);

    // Initialize the world matrix
    m_world = DirectX::XMMatrixIdentity();
    // Initialize the view matrix
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthClipEnable = true;
    // Create the rasterizer state object
    m_renderer->m_device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf());
    m_renderer->m_context->RSSetState(m_rasterState.Get());
    m_view = m_camera->GetViewMatrix().Transpose();

    //// Imgui setup
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();

    //io = &ImGui::GetIO();

    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    //io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    //// io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    //ImGui::StyleColorsDark();

    //ImGui_ImplSDL2_InitForD3D(m_renderer->window);
    //ImGui_ImplDX11_Init(m_renderer->m_device.Get(), m_renderer->m_context.Get());
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
            //ImGui_ImplSDL2_ProcessEvent(&e);
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
    static f64 angle = 0.0f;
    angle += dt;

    //suzanne.UpdateCB(m_renderer, m_camera, dt);
}

void Spar::Application::Render()
{
    m_renderer->Clear();

    // EditorMenu();

    for (auto &model : models)
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
    /*ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();*/
}

//void Spar::Application::EditorMenu()
//{
//    // Start the Dear ImGui frame
//    ImGui_ImplDX11_NewFrame();
//    ImGui_ImplSDL2_NewFrame();
//    ImGui::NewFrame();
//
//    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//    if (true)
//        ImGui::ShowDemoWindow();
//
//    ImGui::Render();
//
//    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
//
//    // Update and Render additional Platform Windows
//    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//    {
//        ImGui::UpdatePlatformWindows();
//        ImGui::RenderPlatformWindowsDefault();
//    }
//}
