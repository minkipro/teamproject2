#include "stdafx.h"
#include "HBasicSceneObject.h"
#include "Graphics\HCMeshManager.h"
#include "Graphics/HXmlAnimationManager.h"


void HBasicSceneObject::Init(const wchar_t* textureName)
{
	auto graphic = HCDEVICE(HCGraphic);
	//디버깅용
	m_textRender = nullptr;
	graphic->CreateTextData(m_textRender);

	std::vector<std::wstring> fontNames;
	m_textRender->GetFontNames(fontNames);
	m_textRender->SetFont(fontNames.front());
	//


	m_mesh = HCMeshManager::Get()->GetMesh(typeid(HCOnePointExtToRect).name());
	m_textureData = graphic->GetTextureIndex(textureName);

	HC::GRAPHIC_RESOURCE_DESC renderInfoBufferDesc;

	renderInfoBufferDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_BUFFER;
	renderInfoBufferDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC;
	renderInfoBufferDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_ALL;
	renderInfoBufferDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_STRUCTURED_BUFFER;
	renderInfoBufferDesc.Stride = sizeof(HCPointRenderInfo);
	renderInfoBufferDesc.Buffer.StrideNum = 1;

	m_pos = { HC::GO.WIN.WindowsizeX * 0.5f, HC::GO.WIN.WindowsizeY * 0.5f };

	float width = (float)m_textureData.sizeX;
	float height = (float)m_textureData.sizeY;
	if (!m_textureData.spriteUV.empty()) //스프라이트면
	{
		UINT curSpriteIndex = 0;
		XMFLOAT4 uv = m_textureData.spriteUV[curSpriteIndex];
		width *= (uv.z - uv.x);
		height *= (uv.w - uv.y);
	}
	m_renderInfo.Size = { width, height };
	m_renderInfo.Position = { m_pos.x, m_pos.y , 0.6f};
	m_renderInfo.SpriteInfoIndex = m_textureData.spriteStartIndex;


	renderInfoBufferDesc.DefaultData = &m_renderInfo;
	graphic->CreateResource(renderInfoBufferDesc, m_renderInfoBuffer);

	//애니메이션 등록
	wstring onlyFileName = StringHelper::GetFileNameFromPath(textureName);
	auto& curActionMap = HXmlAnimationManager::Get()->m_actionCharacterMap[StringHelper::WideToString(onlyFileName)];

	int offset = 0;
	for (auto& it : curActionMap)
	{
		m_animations.push_back(HC2DAnimation());
		
		m_animations.back().SetTexture(m_textureData);
		m_animations.back().SetSpriteIndices(it.second.indices, it.first.c_str(), ((float)it.second.time / 1000.0f) * (float)it.second.indices.size(), it.second.isRepeat);
	}
	SetAnimation(3);
}

void HBasicSceneObject::Update(float deltaTime)
{
	float width = (float)m_textureData.sizeX;
	float height = (float)m_textureData.sizeY;
	m_renderInfo.SpriteInfoIndex = m_animations[m_curAnimIndex].Update(deltaTime);


	if (!m_textureData.spriteUV.empty()) //스프라이트면
	{
		XMFLOAT4 uv = m_textureData.spriteUV[m_renderInfo.SpriteInfoIndex - m_textureData.spriteStartIndex];
		width *= (uv.z - uv.x);
		height *= (uv.w - uv.y);
	}

	bool isRight = false;
	auto keyboard = HCDEVICE(HCKeyboard);
	if (keyboard->IsKeyCurrState(HCKEYBOARD_KEY_TYPE::A, HCKEYBOARD_KEY_STATE::PRESS_HELD) && !keyboard->IsKeyCurrState(HCKEYBOARD_KEY_TYPE::D, HCKEYBOARD_KEY_STATE::PRESS_HELD))
	{
		m_pos.x -= 200.0f * deltaTime;
		SetAnimation(5);
	}
	else if (!keyboard->IsKeyCurrState(HCKEYBOARD_KEY_TYPE::A, HCKEYBOARD_KEY_STATE::PRESS_HELD) && keyboard->IsKeyCurrState(HCKEYBOARD_KEY_TYPE::D, HCKEYBOARD_KEY_STATE::PRESS_HELD))
	{
		m_pos.x += 200.0f * deltaTime;
		SetAnimation(5);
		isRight = true;
	}
	else
	{
		SetAnimation(2);
	}

	

	/*static float curTime = 0.0f;
	curTime += deltaTime;
	int offset = 0;
	for (auto it : m_animations)
	{
		int currentIndex = ((int)curTime % (m_animations.size()));
		char buffer[1024] = {};
		if (offset == currentIndex)
		{
			_itoa_s(currentIndex, buffer, 10);
			m_textRender->m_text = StringHelper::StringToWide(buffer);
			SetAnimation(currentIndex);
			break;
		}
		offset++;
	}*/
	



	
	if (isRight)
	{
		m_renderInfo.Size = { -width, height };
		m_renderInfo.Position = { m_pos.x+width, m_pos.y - height , 0.6f };
	}
	else
	{
		m_renderInfo.Size = { width, height };
		m_renderInfo.Position = { m_pos.x, m_pos.y - height , 0.6f };
	}
	
	

	RECT cr = { m_renderInfo.Position.x, m_renderInfo.Position.y, m_renderInfo.Position.x + m_renderInfo.Size.x, m_renderInfo.Position.y + m_renderInfo.Size.y };
	auto mouse = HCDEVICE(HCMouse);
	XMFLOAT2 mousePos = mouse->GetMousePos();
	
	bool isCollision = false;
	if (mousePos.x > cr.left && mousePos.x < cr.right)
	{
		if (mousePos.y > cr.top && mousePos.y < cr.bottom)
		{
			isCollision = true;
		}
	}
	if (isCollision)
	{
		SetAnimation(0);
	}
	else
	{

	}
	
}

void HBasicSceneObject::Render()
{
	m_renderInfoBuffer->Map();
	m_renderInfoBuffer->CopyCpuDataToGpu(&m_renderInfo);
	m_renderInfoBuffer->UnMap();

	auto graphic = HCDEVICE(HCGraphic);

	graphic->SetTexture(m_textureData.textureIndex, 0);
	graphic->SetShaderResource(m_renderInfoBuffer, 1);

	graphic->DrawInsatance(m_mesh, 1);
}

void HBasicSceneObject::SetAnimation(int index)
{
	if (m_curAnimIndex != index)
	{
		m_curAnimIndex = index;
		m_animations[m_curAnimIndex].Start();
	}
}
