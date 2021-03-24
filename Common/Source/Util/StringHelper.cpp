#include "StringHelper.h"
#include <algorithm>
#include <Windows.h>

std::wstring StringHelper::StringToWide(std::string str)
{
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

std::string StringHelper::GetDirectoryFromPath(const std::string& filepath)
{
	size_t off1 = filepath.find_last_of('\\');
	size_t off2 = filepath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) //If no slash or backslash in path?
	{
		return "";
	}
	if (off1 == std::string::npos)
	{
		return filepath.substr(0, off2);
	}
	if (off2 == std::string::npos)
	{
		return filepath.substr(0, off1);
	}
	//If both exists, need to use the greater offset
	return filepath.substr(0, (((off1) > (off2)) ? (off1) : (off2)));
}

std::string StringHelper::GetFileExtension(const std::string& filename)
{
	size_t off = filename.find_last_of('.');
	if (off == std::string::npos)
	{
		return {};
	}
	return std::string(filename.substr(off + 1));
}

void StringHelper::SearchAllDirectoryFromDirectory(const std::wstring& dirPath, std::vector<std::wstring>& out)
{
	WIN32_FIND_DATA fd;
	HANDLE handle = 0;
	int result = 1;
	handle = FindFirstFile((dirPath + L"\\*").c_str(), &fd);

	if (handle == INVALID_HANDLE_VALUE)
	{
		FindClose(handle);
		return;
	}

	std::wstring subFolderPath = dirPath + L"/";

	while (result)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (fd.cFileName[0] != '.')
			{
				std::wstring temp(fd.cFileName);
				out.push_back(temp);
			}
		}

		result = FindNextFile(handle, &fd);
	}

	FindClose(handle);
}

void StringHelper::SearchAllFileFromDirectory(const std::wstring& dirPath, bool searchSubDir, std::vector<std::wstring>& out)
{
	WIN32_FIND_DATA fd;
	HANDLE handle = 0;
	int result = 1;
	handle = FindFirstFile((dirPath + L"\\*").c_str(), &fd);

	if (handle == INVALID_HANDLE_VALUE)
	{
		FindClose(handle);
		return;
	}

	std::wstring subFolderPath = dirPath + L"/";
	std::vector<std::wstring> subFolders;

	while (result)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (searchSubDir)
			{
				if (fd.cFileName[0] != '.')
				{
					subFolders.push_back(fd.cFileName);
				}
			}
		}
		else
		{
			std::wstring filePath = subFolderPath + fd.cFileName;
			out.push_back(filePath);
		}

		result = FindNextFile(handle, &fd);
	}

	if (searchSubDir)
	{
		for (auto& it : subFolders)
		{
			SearchAllFileFromDirectory(subFolderPath + it, searchSubDir, out);
		}
	}

	FindClose(handle);
}
