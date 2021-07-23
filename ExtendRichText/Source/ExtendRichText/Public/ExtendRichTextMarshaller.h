// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Framework/Text/RichTextLayoutMarshaller.h"
#if WITH_FANCY_TEXT

/**
 * 
 */
class EXTENDRICHTEXT_API FExtendRichTextMarshaller : public FRichTextLayoutMarshaller
{
public:
	static TSharedRef< FExtendRichTextMarshaller > Create(TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet);
	
	virtual void AppendRunsForText(const int32 LineIndex, const FTextRunParseResults& TextRun, const FString& ProcessedString, const FTextBlockStyle& DefaultTextStyle, const TSharedRef<FString>& InOutModelText, FTextLayout& TargetTextLayout, TArray<TSharedRef<IRun>>& Runs, TArray<FTextLineHighlight>& LineHighlights, TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextUnderlineLineHighlighter>>& CachedUnderlineHighlighters, TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextStrikeLineHighlighter>>& CachedStrikeLineHighlighters) override;
	virtual~FExtendRichTextMarshaller();
protected:
	FExtendRichTextMarshaller(TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet);
	TArray<TSharedRef<FTextBlockStyle>> DecoratorTempStyle;
};
#endif
