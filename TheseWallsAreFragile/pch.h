#pragma once

#define APPLICATION_NAME L"TheseWallsAreFragile"

#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#define SAFE_RELEASE(comptr)if(comptr){comptr->Release();comptr=nullptr;}
#define SAFE_DELETE(ptr)if(ptr){delete ptr;ptr=nullptr;}
#define SAFE_DELETE_ARRAY(ptr)if(ptr){delete[] ptr;ptr=nullptr;}