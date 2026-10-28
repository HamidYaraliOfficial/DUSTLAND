#pragma once

#include "Modules/ModuleManager.h"

/**
 * Minimal module entry point for DustlandCore. This module has no runtime
 * behaviour of its own; it only carries shared UENUM / USTRUCT declarations
 * consumed by every gameplay module (Survival, Inventory, Economy, Save, ...).
 */
class FDustlandCoreModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};
