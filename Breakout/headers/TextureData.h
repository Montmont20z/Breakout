#pragma once
#include <d3dx9.h>
#include <string>
struct TextureData {
	LPDIRECT3DTEXTURE9 texture = nullptr;
	D3DXIMAGE_INFO info = {};
	std::string filename;
	int logicalWidth = 0;
    int logicalHeight = 0;
};