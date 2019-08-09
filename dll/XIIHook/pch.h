#ifndef PCH_H
#define PCH_H
#define DLL_BUILD

constexpr double Rad2Deg = 0.0174532925199L;
constexpr double Rad90 = 1.570796L;
constexpr double OneOverSixty = 0.01666666666666667L;
constexpr double M_PI = 3.14159265358979323846L;


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

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "detours.lib")

#include "framework.h"
#include "Quaternion.h"
#include "Vector3.h"

#endif



