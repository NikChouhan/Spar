#include "Texture.h"
#include "stb_image.h"

Spar::Texture::Texture()
{
}

Spar::Texture::~Texture()
{
}

void Spar::Texture::LoadTexture(const char *path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

    if (!data)
    {
        Log::Error("Failed to load texture");
    }

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
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = data;
    subresourceData.SysMemPitch = width * 4;
    subresourceData.SysMemSlicePitch = 0;

    wrl::ComPtr<ID3D11Texture2D> texture = nullptr;
    
}