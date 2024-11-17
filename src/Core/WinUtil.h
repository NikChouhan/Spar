#pragma once

#include <Windows.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include "StandardTypes.h"
#include "SimpleMath.h"
#include <wrl.h>
#include <assert.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <memory>

namespace SM = DirectX::SimpleMath;
namespace wrl = Microsoft::WRL;

#define ReleaseCOM(x) { if(x){ x->Release(); x = nullptr; } }
