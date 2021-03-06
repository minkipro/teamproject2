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
			unsigned int		NumSwapchainBuffer = 2;
			const std::wstring	TextureFolderPath = L"./../Common/Texture";
			const std::wstring	FontFolderPath = L"./../Common/Fonts";
			const std::wstring	SpriteTextureSuffix = L"sp_";
		}GRAPHIC;
	}GO;
}
