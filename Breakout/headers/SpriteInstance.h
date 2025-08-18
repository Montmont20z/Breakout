#pragma once
#include <string>
#include <d3dx9.h>


struct SpriteInstance {
	int id = -1; // unique ID to allow sprite to easily used by other class

	std::string texturePath;
	
	// Variable for transformation
	D3DXVECTOR3 position = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 scale = D3DXVECTOR3(1, 1, 1);
	float rotation = 0.0f;
	D3DCOLOR color = D3DCOLOR_XRGB(255,255,255);
	int renderOrder = 0; // Lower values render first (background), higher value render last (foreground)
	bool visible = true;

	// animation definition
	int animationRows = 1;
	int animationCols = 1;
	int frameCount = animationRows * animationCols;
	int currentFrame = 0;
	float frameDuration = 0.1f; // seconds per frame
	float elapsedSinceFrame = 0.0f; // accumulated seconds
	bool looping = true;
	bool playing = false;

	 RECT GetSourceRect(int textureWidth, int textureHeight) const {
        RECT rect = {0,0,0,0};
        if (frameCount <= 1 || animationCols <= 0 || animationRows <= 0) {
            rect.left = 0; rect.top = 0; rect.right = textureWidth; rect.bottom = textureHeight;
            return rect;
        }

        int frameW = textureWidth / animationCols;
        int frameH = textureHeight / animationRows;

        int frame = currentFrame % frameCount; // safety
        int col = frame % animationCols;
        int row = frame / animationCols;

        rect.left = col * frameW;
        rect.top = row * frameH;
        rect.right = rect.left + frameW;
        rect.bottom = rect.top + frameH;
        return rect;
    }

	SpriteInstance() = default;
	
	SpriteInstance(const std::string& path, const D3DXVECTOR3& pos, int order = 0, int animationRows = 1, int animationCols = 1, int frameCount = 0, float frameDuration = 0.1f, bool looping = true, bool playing = false)
		: texturePath(path)
		, position(pos)
		, renderOrder(order) 
		, animationRows(animationRows)
		, animationCols(animationCols)
		, frameCount(frameCount)
		, frameDuration(frameDuration)
		, looping(looping)
		, playing(playing) 
	{
		if (this->animationRows < 1) this->animationRows = 1;
		if (this->animationCols < 1) this->animationCols = 1;

		if (this->frameCount <= 0) {
			this->frameCount = this->animationRows * this->animationCols;
		}
	}
};