#pragma region PLUGIN_ENTRY - Do not change!
#include "stdafx.h"
#include <IBehaviourPlugin.h>

//Plugins
#include "ExamPlugin.h"


extern "C"
{
	int RunFramework(HMODULE module, std::string levelPath = {});
	__declspec(dllexport) int RunFrameworkDLL(HMODULE module)
	{
		return RunFramework(module, "data/LevelOne.gppl");
	}

	__declspec(dllexport) IBehaviourPlugin* Create()
	{
		return new ExamPlugin();
	}
}
#pragma endregion 
