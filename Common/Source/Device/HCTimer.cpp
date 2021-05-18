#include "stdafx.h"
#include "HCTimer.h"
#include <chrono>
float HCTimer::m_deltaTime = 0.0f;
std::chrono::time_point<std::chrono::steady_clock> hcPrevTime;//전역변수
HCTimer::HCTimer()
{
	
}

void HCTimer::Init()
{
	hcPrevTime = std::chrono::high_resolution_clock::now();
}

void HCTimer::Update()
{
	m_deltaTime =std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now()- hcPrevTime).count();
	hcPrevTime = std::chrono::high_resolution_clock::now();
}

float HCTimer::GetDeltatime()
{
	return m_deltaTime*0.001f;
}