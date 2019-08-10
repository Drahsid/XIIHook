#ifndef PCH_H
#define PCH_H
#define DLL_BUILD

typedef bool(*bPa1a2)(int a1, int a2);
typedef void(*voidPn)();
typedef void(*voidPa1)(int a1);

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

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "detours.lib")

#include "framework.h"
#include "Quaternion.h"
#include "Vector3.h"

#endif



