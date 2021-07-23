// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ExtendHyperEventHandler.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDRICHTEXT_API UExtendHyperEventHandler : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = Handler)
		void HandleOnClick(const TMap<FString, FString>& meta);
	void HandleOnClick_Implementation(const TMap<FString, FString>& meta)const;

	
	UFUNCTION(BlueprintNativeEvent, Category = Handler)
	void HandleOnTooltip(const TMap<FString,FString>& meta);
	void HandleOnTooltip_Implementation(const TMap<FString, FString>& meta);

};
