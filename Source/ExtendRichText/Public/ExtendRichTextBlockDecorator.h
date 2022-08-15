// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExtendRichTextStyle.h"
#include "ExtendRichTextSettings.h"
#include "SExtendRichTextEditableBox.h"
#include "Components/RichTextBlockDecorator.h"
#include "Framework/Text/SlateHyperlinkRun.h"
#include "Framework/Text/SlateTextRun.h"

#include "ExtendRichTextBlockDecorator.generated.h"


class FRunCreator
{
public:
	static TSharedRef<FSlateTextRun> CreateTextRun(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style, const FTextRange& InRange)
	{
		return FSlateTextRun::Create(InRunInfo, InText, Style, InRange);
	}

	static TSharedRef<FSlateTextRun> CreateTextRun(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style)
	{
		return FSlateTextRun::Create(InRunInfo, InText, Style);
	}
	
	static TSharedRef<FSlateHyperlinkRun> CreateHyperRun(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FHyperlinkStyle& InStyle, FSlateHyperlinkRun::FOnClick NavigateDelegate, FSlateHyperlinkRun::FOnGenerateTooltip InTooltipDelegate, FSlateHyperlinkRun::FOnGetTooltipText InTooltipTextDelegate)
	{
		return FSlateHyperlinkRun::Create(InRunInfo, InText, InStyle, NavigateDelegate, InTooltipDelegate, InTooltipTextDelegate);
	}

	static TSharedRef<FSlateHyperlinkRun> CreateHyperRun(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FHyperlinkStyle& InStyle, const FTextRange& Range,FSlateHyperlinkRun::FOnClick NavigateDelegate, FSlateHyperlinkRun::FOnGenerateTooltip InTooltipDelegate, FSlateHyperlinkRun::FOnGetTooltipText InTooltipTextDelegate)
	{
		return FSlateHyperlinkRun::Create(InRunInfo,InText,InStyle, NavigateDelegate, InTooltipDelegate, InTooltipTextDelegate,Range);
	}
};


class FExtendRichTextStyleMaker
{
public:
	static TSharedPtr<FHyperlinkStyle> MakeStyleByState(const FRichTextStyleState& State)
	{
		TSharedPtr<FHyperlinkStyle> style = MakeShareable(new FHyperlinkStyle());

		style->SetTextStyle(MakeTextStyleByState(State));
		style->SetUnderlineStyle(FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FButtonStyle>(TEXT("HyperLinkUnderLine")));
		style->SetPadding(FMargin(1.f));
		return style;
	}

	static FTextBlockStyle MakeTextStyleByState(const FRichTextStyleState& state)
	{
		//size
		uint8 size = state.FontSize;

		//font
		FString fontName = TEXT("Regular");
		if (state.Bold && !state.Italic)
		{
			fontName = TEXT("Bold");
		}
		else if (state.Bold && state.Italic)
		{
			fontName = TEXT("BoldItalic");
		}
		else if (!state.Bold && state.Italic)
		{
			fontName = TEXT("Italic");
		}

		auto setting = GetDefault<UExtendRichTextSettings>();
		//color
		FSlateColor color = setting->TextColors[state.ColorIndex];

		FTextBlockStyle textStyle = FTextBlockStyle()
			.SetFont(FCoreStyle::GetDefaultFontStyle(FName(fontName), size))
			.SetColorAndOpacity(color)
			.SetShadowOffset(FVector2D::ZeroVector)
			.SetHighlightColor(FLinearColor::Black);

		if (state.UnderLine)
		{
			textStyle.SetUnderlineBrush(*FExtendRichTextEditBoxStyles::Get().GetBrush("UnderLineBrush"));
		}

		return textStyle;
	}
};
class UExtendRichTextBlockDecorator;
class SExtendRichTextEditableBox;
class EXTENDRICHTEXT_API FEditableRichTextDecorator : public ITextDecorator
{
public:
	FEditableRichTextDecorator(const UObject* Hyper);
	virtual ~FEditableRichTextDecorator(){}
	virtual TSharedRef<ISlateRun> Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunInfo, const FString& OriginalText, const TSharedRef<FString>& ModelText, const ISlateStyle* Style) override;
	virtual bool Supports(const FTextRunParseResults& RunInfo, const FString& Text) const override;

protected:
	TWeakObjectPtr<const UObject> HyperEventHandler;
};

class FExtendRichTextDecorator : public FEditableRichTextDecorator
{
public:
	virtual ~FExtendRichTextDecorator() override;
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;
	static TSharedRef<FExtendRichTextDecorator> CreateIns(const UObject* ClickHandler);
protected:
	FExtendRichTextDecorator(const UObject* ClickHandler)
		:FEditableRichTextDecorator(ClickHandler)
	{}
};


class FExtendImageDecorator : public ITextDecorator
{
public:

	static TSharedRef< FExtendImageDecorator > Create();
	virtual ~FExtendImageDecorator() {}

public:

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;

	virtual TSharedRef< ISlateRun > Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override;

	static FString GetPathToImage(const FString& InSrcMetaData);

private:

	FExtendImageDecorator();
};
/**
 * 
 */
UCLASS(Abstract)
class EXTENDRICHTEXT_API UExtendRichTextBlockDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	UExtendRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	UFUNCTION(BlueprintImplementableEvent,Category = Navigate)
	void OnRichTextNavigate(const FString& metaString);
};
