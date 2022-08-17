// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "ExtendRichTextBlock.generated.h"

USTRUCT(BlueprintType)
struct FHyperMeta
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TMap<FString,FString> Metas;
};

UINTERFACE(Blueprintable)
class EXTENDRICHTEXT_API UHyperClickInterface : public UInterface
{
	GENERATED_BODY()
};
class EXTENDRICHTEXT_API IHyperClickInterface
{ 
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ClickHandle)
	void OnHyperClick(const FHyperMeta& Meta)const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHyperLinkClick, const FHyperMeta&, MetaData);
/**
 * 
 */
UCLASS()
class EXTENDRICHTEXT_API UExtendRichTextBlock : public URichTextBlock, public IHyperClickInterface
{
	GENERATED_BODY()
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	UPROPERTY(BlueprintAssignable,Category = ExtendRichText)
		FOnHyperLinkClick OnHyperLinkClickFunc;
	void OnHyperClick_Implementation(const FHyperMeta& Meta)const override;
};
