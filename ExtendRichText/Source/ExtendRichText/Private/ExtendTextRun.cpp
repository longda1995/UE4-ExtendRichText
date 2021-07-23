// Fill out your copyright notice in the Description page of Project Settings.

#include "ExtendTextRun.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Text/DefaultLayoutBlock.h"
#include "Framework/Text/RunUtils.h"
#include "Framework/Text/ShapedTextCache.h"
#include "Framework/Text/WidgetLayoutBlock.h"
#include "Widgets/Input/SHyperlink.h"

TSharedRef<FExtendTextRun> FExtendTextRun::Create(const FRunInfo& RunInfo, const TSharedRef<const FString>& Text,const FTextRange& InRange,const FTextBlockStyle& Style)
{
	return MakeShareable(new FExtendTextRun(RunInfo, Text, InRange, Style));
}

TSharedRef<FExtendTextRun> FExtendTextRun::Create(const FRunInfo& RunInfo, const TSharedRef<const FString>& Text,
	const FHyperlinkStyle& Style,const FOnClick& NavigateDelegate,
	const FOnGenerateTooltip& TooltipDelegate, const FOnGetTooltipText& TooltipTextDelegate)
{
	return MakeShareable(new FExtendTextRun(RunInfo,Text,Style,NavigateDelegate,TooltipDelegate,TooltipTextDelegate));
}

TSharedRef<FExtendTextRun> FExtendTextRun::Create(const FRunInfo& RunInfo, const TSharedRef<const FString>& Text,
	const FTextBlockStyle& Style)
{
	return MakeShareable(new FExtendTextRun(RunInfo, Text, Style));
}

FExtendTextRun::~FExtendTextRun()
{
}

int16 FExtendTextRun::GetBaseLine(float Scale) const
{
	return IsText ? TextRun->GetBaseLine(Scale)
		: HyperRun->GetBaseLine(Scale);
}

int16 FExtendTextRun::GetMaxHeight(float Scale) const
{
	return IsText ? TextRun->GetMaxHeight(Scale)
		: HyperRun->GetMaxHeight(Scale);
}

FVector2D FExtendTextRun::Measure(int32 BeginIndex, int32 EndIndex, float Scale,
	const FRunTextContext& TextContext) const
{
	return IsText ? TextRun->Measure(BeginIndex,EndIndex,Scale,TextContext)
		: HyperRun->Measure(BeginIndex, EndIndex, Scale, TextContext);
}

int8 FExtendTextRun::GetKerning(int32 CurrentIndex, float Scale, const FRunTextContext& TextContext) const
{
	return IsText ? TextRun->GetKerning(CurrentIndex, Scale, TextContext)
		: HyperRun->GetKerning(CurrentIndex, Scale, TextContext);
}

TSharedRef<ILayoutBlock> FExtendTextRun::CreateBlock(int32 StartIndex, int32 EndIndex, FVector2D Size,
	const FLayoutBlockTextContext& TextContext, const TSharedPtr<IRunRenderer>& Renderer)
{
	return IsText ? TextRun->CreateBlock(StartIndex, EndIndex, Size, TextContext, Renderer)
		: HyperRun->CreateBlock(StartIndex, EndIndex, Size, TextContext, Renderer);
}

int32 FExtendTextRun::GetTextIndexAt(const TSharedRef<ILayoutBlock>& Block, const FVector2D& Location, float Scale,ETextHitPoint* const OutHitPoint) const
{
	return IsText ? TextRun->GetTextIndexAt(Block, Location, Scale, OutHitPoint)
		: HyperRun->GetTextIndexAt(Block, Location, Scale, OutHitPoint);
}

FVector2D FExtendTextRun::GetLocationAt(const TSharedRef<ILayoutBlock>& Block, int32 Offset, float Scale) const
{
	return IsText ? TextRun->GetLocationAt(Block, Offset, Scale)
		: HyperRun->GetLocationAt(Block, Offset, Scale);
}

void FExtendTextRun::BeginLayout()
{
	return IsText ? TextRun->BeginLayout()
		: HyperRun->BeginLayout();
}

void FExtendTextRun::EndLayout()
{
	return IsText ? TextRun->EndLayout()
		: HyperRun->EndLayout();
}

void FExtendTextRun::Move(const TSharedRef<FString>& NewText, const FTextRange& NewRange)
{
	return IsText ? TextRun->Move(NewText, NewRange)
		: HyperRun->Move(NewText, NewRange);
}

TSharedRef<IRun> FExtendTextRun::Clone() const
{
	return IsText ? TextRun->Clone()
		: HyperRun->Clone();
}

void FExtendTextRun::AppendTextTo(FString& AppendToText) const
{
	return IsText ? TextRun->AppendTextTo(AppendToText)
		: HyperRun->AppendTextTo(AppendToText);
}

void FExtendTextRun::AppendTextTo(FString& AppendToText, const FTextRange& PartialRange) const
{
	return IsText ? TextRun->AppendTextTo(AppendToText, PartialRange)
		: HyperRun->AppendTextTo(AppendToText, PartialRange);
}

ERunAttributes FExtendTextRun::GetRunAttributes() const
{
	return IsText ? TextRun->GetRunAttributes()
		: HyperRun->GetRunAttributes(); 
}

int32 FExtendTextRun::OnPaint(const FPaintArgs& Args, const FTextLayout::FLineView& Line,
	const TSharedRef<ILayoutBlock>& Block, const FTextBlockStyle& DefaultStyle, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	return IsText ? TextRun->OnPaint(Args, Line, Block, DefaultStyle, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled)
		: HyperRun->OnPaint(Args, Line, Block, DefaultStyle, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

const TArray<TSharedRef<SWidget>>& FExtendTextRun::GetChildren()
{
	return IsText ? TextRun->GetChildren()
		: HyperRun->GetChildren();
}

void FExtendTextRun::ArrangeChildren(const TSharedRef<ILayoutBlock>& Block, const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	return IsText ? TextRun->ArrangeChildren(Block, AllottedGeometry, ArrangedChildren)
		: HyperRun->ArrangeChildren(Block, AllottedGeometry, ArrangedChildren);
}

void FExtendTextRun::ApplyFontSizeMultiplierOnTextStyle(float FontSizeMultiplier)
{
	TextRun->ApplyFontSizeMultiplierOnTextStyle(FontSizeMultiplier);
}

