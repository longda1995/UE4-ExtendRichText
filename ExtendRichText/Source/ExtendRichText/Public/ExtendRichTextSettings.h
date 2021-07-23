// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "ExtendRichTextSettings.generated.h"

class UExtendHyperEventHandler;
/**
 * 
 */
UCLASS(Config = Engine, DefaultConfig, meta = (DisplayName = "Extend Rich Text Setting"))
class EXTENDRICHTEXT_API UExtendRichTextSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UExtendRichTextSettings(){}
	// UDeveloperSettings interface
// Gets the settings container name for the settings, either Project or Editor
	FName GetContainerName() const override { return TEXT("Project"); }
	// Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc.
	FName GetCategoryName() const override { return TEXT("Editor"); };
	// The unique name for your section of settings, uses the class's FName.
	FName GetSectionName() const override { return Super::GetSectionName(); };

#if WITH_EDITOR
	// Gets the section text, uses the classes DisplayName by default.
	FText GetSectionText() const override;
	// Gets the description for the section, uses the classes ToolTip by default.
	FText GetSectionDescription() const override;

	// Whether or not this class supports auto registration or if the settings have a custom setup
	bool SupportsAutoRegistration() const override { return true; }

	// UObject interface
#if ENGINE_MINOR_VERSION >= 25
	bool CanEditChange(const FProperty* InProperty) const override;
#else
	bool CanEditChange(const UProperty* InProperty) const override;
#endif

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	
	UPROPERTY(EditAnywhere, Config, Category = ExtendRichTextStyle)
		FString ExtendTagName;

	UPROPERTY(EditAnywhere, Config, Category = ExtendRichTextStyle)
		uint16 TextDefaultSize;

	UPROPERTY(EditAnywhere, Config, Category = ExtendRichTextStyle)
		TArray<FLinearColor> TextColors;

	UPROPERTY(EditAnywhere, Config, Category = ExtendRichTextStyle)
		FString ImgTagName;
	
	UPROPERTY(EditAnywhere, Config, Category = ExtendRichTextStyle)
		TArray<uint8>  DefaultFontSizeList;

	//超链事件处理器
	UPROPERTY(EditAnywhere, Config, Category = Handler)
		TSubclassOf<UExtendHyperEventHandler> HandlerClass;
};
