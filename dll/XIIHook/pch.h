#pragma once

#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>
#include <psapi.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <algorithm>
#include <type_traits>
#include <dwmapi.h>
#include <inttypes.h>
#include <cmath>
#include <mutex>
#include <tchar.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <XInput.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "Quaternion.h"
#include "Vector3.h"
#include "Utility.h"

#endif



