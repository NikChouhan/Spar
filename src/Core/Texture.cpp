#include "Texture.h"
#include "renderer.h"
#include "Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>


Spar::Texture::Texture()
{
}


Spar::Texture::~Texture()
{
}

void Spar::Texture::LoadTexture(std::shared_ptr<Spar::Renderer> renderer, const char* filename)
{
    // Load the image using stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);  // Flip the image vertically for DirectX
    unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    
    if (!data) {
        Log::Error("Failed to load texture");
    }
    // Create texture descriptor
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // Create subresource data
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = width * 4;    // 4 bytes per pixel (RGBA)
    initData.SysMemSlicePitch = 0;       // Only used for 3D textures

    // Create the texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    assert(renderer->m_device);
    HRESULT hr = renderer->m_device->CreateTexture2D(&textureDesc, &initData, texture.GetAddressOf());
    stbi_image_free(data);  // Free the image data
    
    if (FAILED(hr)) {
        Log::Error("Failed to create texture");
    }

    // Create shader resource view
    hr = renderer->m_device->CreateShaderResourceView(texture.Get(), nullptr, m_textureView.GetAddressOf());
    
    if (FAILED(hr)) 
    {
        Log::Error("Failed to create texture view");
    }
}