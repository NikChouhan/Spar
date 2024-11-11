#pragma once

#include "WinUtil.h"

HRESULT CompileShader(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blob);
