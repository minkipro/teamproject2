#pragma once
#include "Graphics\HCGraphic.h"
#include "Device\HCPhysics.h"

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

	virtual void	Init() = 0;
	virtual void	Update();

	virtual void	UIOn();
	virtual void	UIOff();
	virtual void	SetTexture(const std::wstring& path, const DirectX::XMFLOAT2& size);

	void			AddChild(UIObject* child);
	void			SetParent(UIObject* object);
	void			SetPos(const DirectX::XMFLOAT3& pos);
	void			SetSize(const DirectX::XMFLOAT2& size);
	void			SetBenchUV(const DirectX::XMFLOAT2& benchUV) { m_benchUV = benchUV; }

	void			AddFunc(HCColFunc func);

	const DirectX::XMFLOAT2&	GetSize() { return m_renderPoint.Size; }

private:
	bool			CheckDependency(UIObject* object);
	UIObject*		GetRootUIObject();

protected:
	static HCGraphicPipeLine*	s_graphicPipeLine;
	UIObject*					m_parent;
	std::vector<UIObject*>		m_childs;
	DirectX::XMFLOAT3			m_pos;
	RenderPoint					m_renderPoint;
	HCColliderData				m_ColliderData;
	DirectX::XMFLOAT2			m_benchUV;
	bool						m_isOn;
};