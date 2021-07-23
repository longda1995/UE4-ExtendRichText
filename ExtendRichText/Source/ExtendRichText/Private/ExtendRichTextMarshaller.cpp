// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendRichTextMarshaller.h"
#include "ExtendRichTextBlockDecorator.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/SlateTextUnderlineLineHighlighter.h"

#if WITH_FANCY_TEXT

FExtendRichTextMarshaller::FExtendRichTextMarshaller(TArray< TSharedRef< ITextDecorator > > InDecorators, const ISlateStyle* const InDecoratorStyleSet)
	:FRichTextLayoutMarshaller(MoveTemp(InDecorators),InDecoratorStyleSet)
{
}

FExtendRichTextMarshaller::~FExtendRichTextMarshaller()
{
}

TSharedRef<FExtendRichTextMarshaller> FExtendRichTextMarshaller::Create(TArray<TSharedRef<ITextDecorator>> InDecorators,
	const ISlateStyle* const InDecoratorStyleSet)
{
	return MakeShareable(new FExtendRichTextMarshaller(MoveTemp(InDecorators), InDecoratorStyleSet));
}

void FExtendRichTextMarshaller::AppendRunsForText(const int32 LineIndex, const FTextRunParseResults& TextRun,
                                                  const FString& ProcessedString, const FTextBlockStyle& DefaultTextStyle, const TSharedRef<FString>& InOutModelText,
                                                  FTextLayout& TargetTextLayout, TArray<TSharedRef<IRun>>& Runs, TArray<FTextLineHighlight>& LineHighlights,
                                                  TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextUnderlineLineHighlighter>>& CachedUnderlineHighlighters,
                                                  TMap<const FTextBlockStyle*, TSharedPtr<FSlateTextStrikeLineHighlighter>>& CachedStrikeLineHighlighters)
{
	TSharedPtr< ISlateRun > Run;
	TSharedPtr< ITextDecorator > Decorator = TryGetDecorator(ProcessedString, TextRun);
	
	const FTextBlockStyle* TextBlockStyle = nullptr;
	FTextRange ModelRange;
	if (Decorator.IsValid())
	{

		FString temp = InOutModelText.Get();
		ModelRange.BeginIndex = temp.Len();
		temp += ProcessedString.Mid(TextRun.ContentRange.BeginIndex, TextRun.ContentRange.EndIndex - TextRun.ContentRange.BeginIndex);
		ModelRange.EndIndex = temp.Len();
		
		FRunInfo RunInfo;
		RunInfo.Name = TextRun.Name;
		for (const TPair<FString, FTextRange>& Pair : TextRun.MetaData)
		{
			int32 Length = FMath::Max(0, Pair.Value.EndIndex - Pair.Value.BeginIndex);
			RunInfo.MetaData.Add(Pair.Key, ProcessedString.Mid(Pair.Value.BeginIndex, Length));
		}

		// Create run and update model string.
		if(RunInfo.Name.Equals(GetDefault<UExtendRichTextSettings>()->ExtendTagName))
		{
			FName indexName = FName(RunInfo.MetaData[TEXT("Style")]);
			if(!FExtendRichTextEditBoxStyles::Get().HasWidgetStyle<FTextBlockStyle>(indexName))
			{
				uint32 num = 0;
				LexFromString(num, *RunInfo.MetaData[TEXT("Style")]);

				FExtendRichTextEditBoxStyles::Get().Set(indexName, FExtendRichTextStyleMaker::MakeTextStyleByState(FRichTextStyleState::DeserializeFromUint32(num)));
			}
			
			TextBlockStyle = &FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FTextBlockStyle>(indexName);
		}
		
		Run = Decorator->Create(TargetTextLayout.AsShared(), TextRun, ProcessedString, InOutModelText, DecoratorStyleSet);
	}
	else
	{
		FRunInfo RunInfo(TextRun.Name);
		for (const TPair<FString, FTextRange>& Pair : TextRun.MetaData)
		{
			int32 Length = FMath::Max(0, Pair.Value.EndIndex - Pair.Value.BeginIndex);
			RunInfo.MetaData.Add(Pair.Key, ProcessedString.Mid(Pair.Value.BeginIndex, Length));
		}

		ModelRange.BeginIndex = InOutModelText->Len();
		if (!(TextRun.Name.IsEmpty()) && DecoratorStyleSet->HasWidgetStyle< FTextBlockStyle >(FName(*TextRun.Name)))
		{
			*InOutModelText += ProcessedString.Mid(TextRun.ContentRange.BeginIndex, TextRun.ContentRange.EndIndex - TextRun.ContentRange.BeginIndex);
			TextBlockStyle = &(DecoratorStyleSet->GetWidgetStyle< FTextBlockStyle >(FName(*TextRun.Name)));
		}
		else
		{
			*InOutModelText += ProcessedString.Mid(TextRun.OriginalRange.BeginIndex, TextRun.OriginalRange.EndIndex - TextRun.OriginalRange.BeginIndex);
			TextBlockStyle = &DefaultTextStyle;
		}
		ModelRange.EndIndex = InOutModelText->Len();
		
		// Create run.
		TSharedPtr< FSlateTextRun > SlateTextRun = FSlateTextRun::Create(RunInfo, InOutModelText, *TextBlockStyle, ModelRange);

		if (SlateTextRun)
		{
			// Apply the FontSizeMultiplier at the style use by the IRun
			SlateTextRun->ApplyFontSizeMultiplierOnTextStyle(FontSizeMultiplier);
		}
		Run = SlateTextRun;
	}

	if(TextBlockStyle != nullptr)
	{
		if (!TextBlockStyle->UnderlineBrush.GetResourceName().IsNone())
		{
			TSharedPtr<FSlateTextUnderlineLineHighlighter> UnderlineLineHighlighter = CachedUnderlineHighlighters.FindRef(TextBlockStyle);
			if (!UnderlineLineHighlighter.IsValid())
			{
				UnderlineLineHighlighter = FSlateTextUnderlineLineHighlighter::Create(TextBlockStyle->UnderlineBrush, TextBlockStyle->Font, TextBlockStyle->ColorAndOpacity, TextBlockStyle->ShadowOffset, TextBlockStyle->ShadowColorAndOpacity);
				CachedUnderlineHighlighters.Add(TextBlockStyle, UnderlineLineHighlighter);
			}

			LineHighlights.Add(FTextLineHighlight(LineIndex, ModelRange, FSlateTextUnderlineLineHighlighter::DefaultZIndex, UnderlineLineHighlighter.ToSharedRef()));
		}

		if (!TextBlockStyle->StrikeBrush.GetResourceName().IsNone())
		{
			TSharedPtr<FSlateTextStrikeLineHighlighter> StrikeLineHighlighter = CachedStrikeLineHighlighters.FindRef(TextBlockStyle);
			if (!StrikeLineHighlighter.IsValid())
			{
				StrikeLineHighlighter = FSlateTextStrikeLineHighlighter::Create(TextBlockStyle->StrikeBrush, TextBlockStyle->Font, TextBlockStyle->ColorAndOpacity, TextBlockStyle->ShadowOffset, TextBlockStyle->ShadowColorAndOpacity);
				CachedStrikeLineHighlighters.Add(TextBlockStyle, StrikeLineHighlighter);
			}

			LineHighlights.Add(FTextLineHighlight(LineIndex, ModelRange, FSlateTextStrikeLineHighlighter::DefaultZIndex, StrikeLineHighlighter.ToSharedRef()));
		}
	}

	Runs.Add(Run.ToSharedRef());
	
}
#endif
