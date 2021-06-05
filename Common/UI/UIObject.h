#pragma once
#include "Graphics\HCGraphic.h"
#include "Device\HCPhysics.h"
#include "Graphics\HCRenderInfos.h"

namespace HC
{
	enum class DATA_TYPE
	{
		TYPE_BOOL,
		TYPE_FLOAT,
		TYPE_INT,
		TYPE_UINT,
	};

	struct UnionData
	{
		DATA_TYPE type;
		union
		{
			bool	_b;
			float	_f;
			int		_i;
			unsigned int _u;
		};
	};
}

class UIObject
{
protected:
	enum UITYPE
	{
		UIPANEL,
		UIBUTTON,
		UIPARAM,
	};

public:
	UIObject()
		: m_benchUV(0.0f,0.0f)
		, m_parent(nullptr)
		, m_isOn(true)
	{
		m_pos = {};
	}
	virtual ~UIObject() = default;

	static	void	CreateUIRenderPipeLine(HCGraphic* device);

	virtual void	Init();
	virtual void	Update();
	virtual void	Render();
	virtual void	UIOn(bool value) { m_isOn = value; }

	void			SetTexture(const std::wstring& path, const DirectX::XMFLOAT2& size);
	void			SetTexture(const HCTextureData& textureData, const DirectX::XMFLOAT2& size);
	void			SetPos(const DirectX::XMFLOAT3& pos);
	void			SetSize(const DirectX::XMFLOAT2& size);
	void			SetBenchUV(const DirectX::XMFLOAT2& benchUV) { m_benchUV = benchUV; }

	void			AddChild(UIObject* child);
	void			AddFunc(HCColFunc func);
	void			SetParent(UIObject* object);
	
	void			ClearFunc();
	UINT						GetUINum();
	const DirectX::XMFLOAT2&	GetSize() { return m_renderInfo.Size; }

private:
	bool			CheckDependency(UIObject* object);
	UIObject*		GetRootUIObject();

protected:
	static std::shared_ptr<HCGraphicPipeLine>	s_graphicPipeLine;
	static const HCMesh*						s_mesh;

	DirectX::XMFLOAT3				m_pos;
	std::shared_ptr<IHCResource>	m_renderInfoBuffer;
	HCPointRenderInfo				m_renderInfo;
	HCTextureData					m_textureData;
	IHCCollider*					m_colliderData;
	DirectX::XMFLOAT2				m_benchUV;

	UIObject*						m_parent;
	std::vector<UIObject*>			m_childs;
	bool							m_isOn;
};