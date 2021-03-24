#pragma once
#include <DirectXMath.h>
class KeyboardClass;
class Camera
{
private:
	DirectX::XMFLOAT2 pos;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX orthoMatrix;

public:
	void Init();
	void Update(KeyboardClass* keyboard);
	DirectX::XMMATRIX* GetWorld();
	DirectX::XMMATRIX* GetOrtho();
};

