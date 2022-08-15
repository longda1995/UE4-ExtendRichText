// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendRichTextSettings.h"

#define LOCTEXT_NAMESPACE "ExtendRichText"

#if WITH_EDITOR
UExtendRichTextSettings::UExtendRichTextSettings()
{
	TextColors.Append({FColor::White,FColor::Black, FColor::Blue});
	ExtendTagName = TEXT("Ext");
	DefaultFontSizeList = {6,7,8,9,10,11,12,13,14,15};
	ImgTagName = TEXT("Img");
}

FText UExtendRichTextSettings::GetSectionText() const
{
	return LOCTEXT("SectionText", "Extend Rich Text");
}

FText UExtendRichTextSettings::GetSectionDescription() const
{
	return LOCTEXT("SectionDescription", "Configure how the Dialogue Editor behaves + Runtime behaviour");
}


bool UExtendRichTextSettings::CanEditChange(const FProperty* InProperty) const
{
	return Super::CanEditChange(InProperty);
}

void UExtendRichTextSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE