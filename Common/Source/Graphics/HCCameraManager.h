#pragma once
#include <vector>
namespace HC
{
	struct Camera
	{
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

	public:
		CameraManager();
		Camera* CreateCamera();
		void Update();
		const DirectX::XMMATRIX& GetMatrix() const { return m_viewMatrix; };
	private:
		DirectX::XMMATRIX m_viewMatrix;
		std::vector<std::unique_ptr<Camera>> m_cameras;
		Camera* m_mainCamera;
	};
}
