#pragma once
#include "Modules/ModuleManager.h"

class FWeatherModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};
