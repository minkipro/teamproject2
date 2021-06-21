#pragma once
#include <vector>
namespace HC
{
	struct Camera
	{
		Camera() : position({ 0.0f, 0.0f }), scale({ 1.0f, 1.0f }), rot(0.0f) {}
		DirectX::XMFLOAT2 position;
		DirectX::XMFLOAT2 scale;
		float rot;
	};
	class CameraManager
	{
	public:
		static CameraManager* Get() { return &instance; }
	private:
		static CameraManager instance;
		CameraManager();

	public:
		Camera* CreateCamera();
		void Update();
		DirectX::XMMATRIX XM_CALLCONV GetMatrix();
	private:
		DirectX::XMFLOAT4X4						m_viewMatrix;
		std::vector<std::unique_ptr<Camera>>	m_cameras;
		Camera* m_mainCamera;
	};
}
