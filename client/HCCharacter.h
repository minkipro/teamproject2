#pragma once
#include "Scene/HCSceneObject.h"
#include "HC2DAnimation.h"
#include "Graphics/HCInputDataSamples.h"

class HC2DCharacter : public HCSceneObject
{
public:
	HC2DCharacter() = default;
	virtual ~HC2DCharacter() override;

	virtual void Init(const wchar_t* textureName) override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;

	void AddPos(const DirectX::XMFLOAT2& vec);
	void SetZ(float z);
	void SetPos(const DirectX::XMFLOAT2& pos);
	void SetSize(const DirectX::XMFLOAT2& size);
	void SetAnimation(int index);

private:
	HCPointRenderInfo				m_renderInfo;
	std::shared_ptr<IHCResource>	m_renderInfoBuffer;
	const HCMesh*					m_mesh = nullptr;
	std::vector<HC2DAnimation>	m_animations;
	int							m_currAniamtion;
};

