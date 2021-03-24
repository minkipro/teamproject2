#pragma once
#include <comdef.h>

#define COM_HRESULT_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw COMException( hr, msg, __FILE__, __FUNCTION__, __LINE__ )
#define COM_THROW_IF_FAILED( result, msg ) if( result ) throw COMException( msg, __FILE__, __FUNCTION__, __LINE__ )

class COMException
{
public:
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hr);
		whatmsg = L"Msg: " + std::wstring(msg.begin(), msg.end()) + L"\n";
		whatmsg += error.ErrorMessage();
		whatmsg += L"\nFile: " + std::wstring(file.begin(), file.end());
		whatmsg += L"\nFunction: " + std::wstring(function.begin(), function.end());
		whatmsg += L"\nLine: " + std::to_wstring(line);
	}

	COMException(HRESULT hr, const std::wstring& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hr);
		whatmsg = L"Msg: " + msg + L"\n";
		whatmsg += error.ErrorMessage();
		whatmsg += L"\nFile: " + std::wstring(file.begin(), file.end());
		whatmsg += L"\nFunction: " + std::wstring(function.begin(), function.end());
		whatmsg += L"\nLine: " + std::to_wstring(line);
	}

	COMException(const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		whatmsg = L"Msg: " + std::wstring(msg.begin(), msg.end()) + L"\n";
		whatmsg += L"\nFile: " + std::wstring(file.begin(), file.end());
		whatmsg += L"\nFunction: " + std::wstring(function.begin(), function.end());
		whatmsg += L"\nLine: " + std::to_wstring(line);
	}

	COMException(const std::wstring& msg, const std::string& file, const std::string& function, int line)
	{
		whatmsg = L"Msg: " + msg + L"\n";
		whatmsg += L"\nFile: " + std::wstring(file.begin(), file.end());
		whatmsg += L"\nFunction: " + std::wstring(function.begin(), function.end());
		whatmsg += L"\nLine: " + std::to_wstring(line);
	}

	const wchar_t* what() const
	{
		return whatmsg.c_str();
	}
private:
	std::wstring whatmsg;
};