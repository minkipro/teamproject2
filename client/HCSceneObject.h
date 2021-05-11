#pragma once
namespace HC
{
	class SceneObject
	{
	public:
		virtual ~SceneObject() = 0;
		virtual void Update() = 0;
	};
}
