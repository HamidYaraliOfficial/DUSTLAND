#pragma once
#include "Modules/ModuleManager.h"

class FSurvivalModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};
