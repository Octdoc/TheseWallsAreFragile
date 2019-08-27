#include "pch.h"
#include "application.h"

int Main(HINSTANCE hInstance)
{
	std::vector<std::wstring> cmdLine;
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
	for (int i = 1; i < argc; i++)
	{
		std::wstring str;
		for (int j = 0; argv[i][j]; j++)
			if (argv[i][j] != '"')
				str += argv[i][j];
		cmdLine.push_back(str);
	}
	try
	{
		twaf::Application application;
		application.Init(hInstance, 900, 500);
		application.Run(cmdLine);
	}
	catch (std::exception e)
	{
		MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	return Main(hInstance);
}
int wmain(int argc, wchar_t* argv[])
{
	return Main(GetModuleHandle(NULL));
}