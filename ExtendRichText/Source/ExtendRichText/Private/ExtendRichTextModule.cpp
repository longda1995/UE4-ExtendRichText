// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendRichTextCustomization.h"
#include "ExtendRichTextModule.h"

#define LOCTEXT_NAMESPACE "FExtendRichTextModule"

void FExtendRichTextModule::StartupModule()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout("ExtendRichText", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FExtendRichTextCustomization::MakeInstance));
	
}

void FExtendRichTextModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout("ExtendRichText");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExtendRichTextModule, ExtendRichText)