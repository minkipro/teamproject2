#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class StringHelper
{
public:
	static wstring		StringToWide(std::string str);
	static string		WideToString(std::wstring wstr);
	static string		GetDirectoryFromPath(const std::string& filepath);
	static wstring		GetDirectoryFromPath(const std::wstring& filepath);
	static string		GetFileExtension(const std::string& filename);
	static wstring		GetFileNameFromPath(const std::wstring& filepath);
	static void			SearchAllDirectoryFromDirectory(const std::wstring& dirPath, std::vector<std::wstring>& out);
	static void			SearchAllFileFromDirectory(const std::wstring& dirPath, bool searchSubDir, std::vector<std::wstring>& out);
	static void			SearchAllFileFromDirectory(const std::wstring& dirPath, std::unordered_map<std::wstring, std::vector<std::wstring>>& out, const std::wstring& stacked=L"");
};