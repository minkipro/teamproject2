#pragma once
#include <string>
#include <vector>

class StringHelper
{
public:
	static std::wstring	StringToWide(std::string str);
	static std::string	GetDirectoryFromPath(const std::string& filepath);
	static std::string	GetFileExtension(const std::string& filename);
	static void			SearchAllDirectoryFromDirectory(const std::wstring& dirPath, std::vector<std::wstring>& out);
	static void			SearchAllFileFromDirectory(const std::wstring& dirPath, bool searchSubDir, std::vector<std::wstring>& out);
};