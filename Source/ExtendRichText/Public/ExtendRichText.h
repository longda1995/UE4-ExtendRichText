#pragma once
#include "ExtendRichText.generated.h"

USTRUCT(BlueprintType,Blueprintable)
struct FExtendRichText
{
	GENERATED_BODY()
public:
	FExtendRichText(){}
	FExtendRichText(FText text)
		:Text(text)
	{}
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = ExtendText)
		FText Text;
};
