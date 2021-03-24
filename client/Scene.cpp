#include "Scene.h"
#include "Graphics/Graphics.h"
void Scene::Init(ID3D11Device* device, ID3D11DeviceContext* dc, KeyboardClass* keyboard, Graphics* pgfx)
{
	gfx = pgfx;
	pDevice = device;
	pDc = dc;
	pKeyboard = keyboard;
	camera.Init();
	gameObject.Initialize(pDevice, pDc, &textureManager,L"Data\\Textures\\character.gif");
	panel.Initialize(pDevice, pDc, 0.0f, 0.0f, 100.0f, 100.0f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
}

void Scene::Update()
{ 
	camera.Update(pKeyboard);
	DirectX::XMMATRIX ortho = *camera.GetOrtho();
	DirectX::XMMATRIX worldOrtho = *camera.GetWorld() * ortho;
	gameObject.Update(worldOrtho);
	//panel.Update(ortho);
}

void Scene::Draw()
{
	gfx->SetTopologyAndShader(true, false);
	gameObject.Draw(pDc);

	gfx->SetTopologyAndShader(false, true);
	panel.Draw(pDc);
}
