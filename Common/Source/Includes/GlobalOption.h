#pragma once

namespace HC
{
	static struct GlobalOptions
	{
		struct WindowOption
		{
			int WindowsizeX = 900;
			int WindowsizeY = 900;
		}WIN;

		struct GraphicOption
		{
			unsigned int NumSwapchainBuffer = 2;

		}GRAPHIC;

	}GO;
}
