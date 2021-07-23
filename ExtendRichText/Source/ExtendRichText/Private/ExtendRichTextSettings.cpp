// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendRichTextSettings.h"

#define LOCTEXT_NAMESPACE "ExtendRichText"

#if WITH_EDITOR
FText UExtendRichTextSettings::GetSectionText() const
{
	return LOCTEXT("SectionText", "Extend Rich Text");
}

FText UExtendRichTextSettings::GetSectionDescription() const
{
	return LOCTEXT("SectionDescription", "Configure how the Dialogue Editor behaves + Runtime behaviour");
}


#if ENGINE_MINOR_VERSION >= 25
bool UExtendRichTextSettings::CanEditChange(const FProperty* InProperty) const
#else
bool UDlgSystemSettings::CanEditChange(const UProperty* InProperty) const
#endif
{
	return Super::CanEditChange(InProperty);
}

void UExtendRichTextSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE