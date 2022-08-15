#pragma once
#include "CoreMinimal.h"

class FExtendRichTextModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};


