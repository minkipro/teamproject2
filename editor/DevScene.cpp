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
	
	IHCShader* vs = nullptr;
	IHCShader* ps = nullptr;
	graphic->CreateShader("VS", HC::SHADERTYPE::VS, L"./../Common/Shader/SkeletonShader.hlsl", "VS", &vs);
	graphic->CreateShader("PS", HC::SHADERTYPE::PS, L"./../Common/Shader/SkeletonShader.hlsl", "PS", &ps);
	testPipeLine->SetShader(HC::SHADERTYPE::VS, vs);
	testPipeLine->SetShader(HC::SHADERTYPE::PS, ps);

	graphic->NumberingGraphicPipeLineSlot(0, testPipeLine);
}

void DevScene::Update()
{
	auto timer = HCDEVICE(HCTimer);
}