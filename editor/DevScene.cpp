#include "stdafx.h"
#include "DevScene.h"
#include "HCDevice.h"
#include "GlobalOption.h"
#include "Util/StringHelper.h"

DevScene::DevScene()
{

}

void DevScene::Init()
{
	auto graphic = HCDEVICE(HCGraphic);

	HCGraphicPipeLine* testPipeLine = nullptr;
	graphic->CreateGraphicPipeLine("testPipe", &testPipeLine);

	std::unordered_map<std::wstring, std::vector<std::wstring>> fileNames;
	StringHelper::SearchAllFileFromDirectory(HC::GO.Editor.ModelFolderPath, fileNames);
	for (auto it : fileNames)
	{
		for (int i = 0; i < it.second.size(); i++)
		{
			m_model.Initialize(it.second[i]);
		}
	}
	IHCShader* vs = nullptr;
	IHCShader* ps = nullptr;
	graphic->CreateShader("VS", HC::SHADERTYPE::VS, L"./../Common/Shader/SkeletonShader.hlsl", "VS", &vs);
	graphic->CreateShader("PS", HC::SHADERTYPE::PS, L"./../Common/Shader/SkeletonShader.hlsl", "PS", &ps);
	testPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::TRIANGLELIST;
	testPipeLine->SelectInputSample<RenderVertexSkeleton>();
	testPipeLine->SetShader(HC::SHADERTYPE::VS, vs);
	testPipeLine->SetShader(HC::SHADERTYPE::PS, ps);

	graphic->NumberingGraphicPipeLineSlot(0, testPipeLine);

	testPipeLine->RenderReserveObject(&m_test);
}

void DevScene::Update()
{
	auto timer = HCDEVICE(HCTimer);
	m_test.Position.x += 0.1f * timer->GetDeltatime();
}