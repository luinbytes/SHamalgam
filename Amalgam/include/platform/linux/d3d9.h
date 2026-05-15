#pragma once

#include "Windows.h"

struct IDirect3D9;
struct IDirect3DTexture9;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
struct IDirect3DStateBlock9;
struct IDirect3DDevice9;
struct D3DPRESENT_PARAMETERS;

using LPDIRECT3D9 = IDirect3D9*;
using LPDIRECT3DDEVICE9 = IDirect3DDevice9*;
using LPDIRECT3DTEXTURE9 = IDirect3DTexture9*;
using LPDIRECT3DVERTEXBUFFER9 = IDirect3DVertexBuffer9*;
using LPDIRECT3DINDEXBUFFER9 = IDirect3DIndexBuffer9*;

#define D3D_OK 0
#define D3DRS_SRGBWRITEENABLE 194

