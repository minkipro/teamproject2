#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class StringHelper
{
public:
	static std::wstring	StringToWide(std::string str);
	static std::string  WideToString(std::wstring wstr);
	static std::string	GetDirectoryFromPath(const std::string& filepath);
	static std::wstring	GetDirectoryFromPath(const std::wstring& filepath);
	static std::string	GetFileExtension(const std::string& filename);
	static std::wstring GetFileNameFromPath(const std::wstring& filepath);
	static void			SearchAllDirectoryFromDirectory(const std::wstring& dirPath, std::vector<std::wstring>& out);
	static void			SearchAllFileFromDirectory(const std::wstring& dirPath, bool searchSubDir, std::vector<std::wstring>& out);
	static void			SearchAllFileFromDirectory(const std::wstring& dirPath, std::unordered_map<std::wstring, std::vector<std::wstring>>& out, const std::wstring& stacked=L"");
};