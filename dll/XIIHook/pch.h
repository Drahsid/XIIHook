#ifndef PCH_H
#define PCH_H
#define DLL_BUILD

constexpr double Rad2Deg = 0.0174532925199L;
constexpr double OneOverSixty = 0.01666666666666667L;

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

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "detours.lib")

#include "framework.h"

#endif



