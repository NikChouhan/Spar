#include "WinUtil.h"
#include "string"
#include "Buffer.h"

namespace Spar
{
    class Texture
    {
    public:
        Texture();
        ~Texture();
        void LoadTexture(std::string path);
        wrl::ComPtr<ID3D11ShaderResourceView> GetTextureView();

    private:
        wrl::ComPtr<ID3D11ShaderResourceView> m_textureView = nullptr;
        DrawableAssets assets{};
    };
}