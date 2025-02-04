#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_dx11.h>

#include "Application.h"
#include "renderer.h"
#include "Camera.h"
#include "Log.h"

#include <SDL2\SDL.h>

static bool cursorVis = false;


static void HandleCameraMovement(std::shared_ptr<Spar::Camera> camera, float deltaTime, const Uint8* keyState);
static void HandleMouseMovement(std::shared_ptr<Spar::Camera> camera, float deltaX, float deltaY, float sensitivity);

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
    m_camera->InitAsPerspective(45.0f, m_renderer->m_width, m_renderer->m_height);
    //m_camera->InitAsOrthographic(m_renderer->m_width, m_renderer->m_height);
    m_camera->SetPosition({0.0f, 0.0f, -6.f});
    cursorVis = false;
    SDL_ShowCursor(SDL_DISABLE);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    // shader stuff
    Shader shader;
    const WCHAR *vsShaderPath = L"../../../../assets/shaders/Model/ModelVS.hlsl";
    const WCHAR *psshaderPath = L"../../../../assets/shaders/Model/ModelPS.hlsl";
    shader.ProcessShaders(m_renderer, vsShaderPath, psshaderPath);
    // Load model
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/Cube/cube.glTF");
    suzanne.LoadModel(m_renderer, m_camera,"../../../../assets/models/sponza/Sponza.glTF");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/scifi/SciFiHelmet.gltf");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/suzanne/Suzanne.gltf");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/balls/MetalRoughSpheres.gltf");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/flighthelmet/FlightHelmet.gltf");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/bistro/bistro.gltf");
    //suzanne.LoadModel(m_renderer, m_camera, "../../../../assets/models/bistrogodot/bistrogodot.gltf");


    models.push_back(suzanne);

    //raster desc
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthClipEnable = true;
    // Create the rasterizer state object
    m_renderer->m_device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf());
    m_renderer->m_context->RSSetState(m_rasterState.Get());

    // Imgui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    io = &ImGui::GetIO();

    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    // io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForD3D(m_renderer->window);
    ImGui_ImplDX11_Init(m_renderer->m_device.Get(), m_renderer->m_context.Get());
}

void Spar::Application::Run()
{
    bool quit = false;
    SDL_Event e;

    static float lastFrame = 0.0f;

    while (!quit)
    {
        // Handle events
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_C)
            {
                cursorVis = !cursorVis;
                SDL_ShowCursor(cursorVis ? SDL_ENABLE : SDL_DISABLE);
                SDL_SetRelativeMouseMode(cursorVis ? SDL_FALSE : SDL_TRUE);
            }

            ImGui_ImplSDL2_ProcessEvent(&e);

            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_MOUSEMOTION)
            {
                // Use relative motion for camera control
                int deltaX = e.motion.xrel;
                int deltaY = e.motion.yrel;
                HandleMouseMovement(m_camera, deltaX, deltaY, 0.5f);
            }
        }

        // Calculate delta time
        float currentFrame = SDL_GetTicks() / 1000.0f;
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update game state and render
        Render(deltaTime);
    }
}


void Spar::Application::Render(float dt)
{
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    HandleCameraMovement(m_camera, dt, keyState);

    m_renderer->Clear();
    for (auto& model : models)
    {
        m_renderer->Submit(model);
    }

    EditorMenu(dt);
    m_renderer->Present();


}

void Spar::Application::Resize()
{
}

void Spar::Application::ShutDown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Spar::Application::EditorMenu(float dt)
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    //fps counter
    ImGui::Begin("Stats");
    static float constTime = 0.0f;
    static float time = 0.0f;
    static int count = 0;
    time += dt;
    count++;

    if (time >= 2)
    {
        constTime = time/count;
        count = 0;
        time = 0.0f;
    }
    ImGui::Text("FrameTime: %0.4f", constTime);
    ImGui::Text("FPS: %0.2f", 1 / constTime);

    ImGui::End();

    //#TODO? might add option to load model later idk

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

static void HandleCameraMovement(std::shared_ptr<Spar::Camera> camera, float deltaTime, const Uint8* keyState)
{
    constexpr float moveSpeed = 1.0f;

    SM::Vector3 forward = camera->GetLookAtTarget();
    forward.Normalize();

    SM::Vector3 up = camera->GetUp();
    up.Normalize();

    //might fix this later but I guess this left-right movement is more intuitive. Still will change it if needs be
    SM::Vector3 right = forward.Cross(up);
    right.Normalize();



    // Calculate movement in local space
    SM::Vector3 movement(0.0f, 0.0f, 0.0f);
    if (keyState[SDL_SCANCODE_W]) movement += forward * moveSpeed * deltaTime;
    if (keyState[SDL_SCANCODE_S]) movement -= forward * moveSpeed * deltaTime;
    if (keyState[SDL_SCANCODE_A]) movement -= right * moveSpeed * deltaTime;
    if (keyState[SDL_SCANCODE_D]) movement += right * moveSpeed * deltaTime;
    if (keyState[SDL_SCANCODE_Q]) movement -= up * moveSpeed * deltaTime;
    if (keyState[SDL_SCANCODE_E]) movement += up * moveSpeed * deltaTime;
    //Spar::Log::InfoDebug("movement: ", movement);
    camera->Translate(movement);
}

void HandleMouseMovement(std::shared_ptr<Spar::Camera> camera, float deltaX, float deltaY, float sensitivity)
{
    deltaX *= sensitivity;
    deltaY *= -sensitivity;

    float yaw{};
    yaw += deltaX;
    float pitch{};
    pitch += deltaY;

    float constrainPitch = true;

    // makes sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (pitch > 80.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    SM::Vector3 up = camera->GetUp();
    up.Normalize();

    camera->Rotate(up, DirectX::XMConvertToRadians(yaw));

    SM::Vector3 forward = camera->GetLookAtTarget();
    forward.Normalize();

    SM::Vector3 right = forward.Cross(up);
    right.Normalize();

    camera->Rotate(right, DirectX::XMConvertToRadians(pitch));
}


