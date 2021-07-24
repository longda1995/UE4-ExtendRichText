// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "ExtendRichTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class EXTENDRICHTEXT_API UExtendRichTextBlock : public URichTextBlock
{
	GENERATED_BODY()
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
