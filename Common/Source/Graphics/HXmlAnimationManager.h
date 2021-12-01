#pragma once
#include <map>
#include <string>
struct HAction
{
	bool isRepeat;
	DWORD time;
	vector<UINT> indices;
};
class HXmlAnimationManager
{
public:
	static HXmlAnimationManager* Get()
	{
		return &m_instance;
	}
	map<string, map<string,HAction>> m_actionCharacterMap;
private:
	static HXmlAnimationManager m_instance;
};

