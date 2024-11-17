#include "WinUtil.h"
#include "Log.h"

namespace Spar
{
    class Renderer;
}

namespace Spar
{
    class Texture
    {
    public:
        Texture();
        ~Texture();
        void LoadTexture(std::shared_ptr<Spar::Renderer> renderer ,const char* path);
        //wrl::ComPtr<ID3D11ShaderResourceView> GetTextureView();

    public:
        wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;
    };
}