#include "WinUtil.h"
#include "Log.h"

namespace Spar
{
    class Texture
    {
    public:
        Texture();
        ~Texture();
        void LoadTexture(const char* path);
        //wrl::ComPtr<ID3D11ShaderResourceView> GetTextureView();

    private:
        wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;
    };
}