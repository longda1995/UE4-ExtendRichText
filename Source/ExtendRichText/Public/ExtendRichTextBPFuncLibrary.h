// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExtendRichText.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExtendRichTextBPFuncLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDRICHTEXT_API UExtendRichTextBPFuncLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
		UFUNCTION(BlueprintCallable,BlueprintPure, Category = ExtendRichText)
		static const FTextBlockStyle& GetDefaultTextStyle();

		UFUNCTION(BlueprintCallable, Category = ExtendRichText)
			static void SetText(UPARAM(ref)FExtendRichText& richText,FText newText);
};
