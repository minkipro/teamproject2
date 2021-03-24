#pragma once
#include "GameObject.h"
#include "Keyboard/KeyboardClass.h"
#include <d3d11.h>
#include "Camera.h"
#include "TextureManager.h"
#include "panel.h"
class Graphics;
class Scene
{
private:
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDc = nullptr;

	KeyboardClass* pKeyboard;
	Camera camera;
	TextureManager textureManager;
	GameObject gameObject;
	Panel panel;
	Graphics* gfx;
public:
	void Init(ID3D11Device* device, ID3D11DeviceContext* dc, KeyboardClass* keyboard, Graphics* pgfx);
	void Update();
	void Draw();
};

