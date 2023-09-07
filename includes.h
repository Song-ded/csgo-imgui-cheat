#include "offsets.h"
#include <iostream>
#include <Windows.h>
#include <d3dx9.h>	
#include <d3d9.h>
#include "kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "drawing.h"
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include "vector.h"
#include "xorstr.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace offsets::netvars;
using namespace offsets::signatures;

extern LPDIRECT3DDEVICE9 pDevice;
