#pragma once

#include "CoreMinimal.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/SlateHyperlinkRun.h"
#include "Framework/Text/SlateTextRun.h"

/**
 * 
 */
class EXTENDRICHTEXT_API FExtendTextRun : public ISlateRun,public TSharedFromThis<FExtendTextRun>
{
public:

	class FWidgetViewModel
	{
	public:

		bool IsPressed() const { return bIsPressed; }
		bool IsHovered() const { return bIsHovered; }

		void SetIsPressed(bool Value) { bIsPressed = Value; }
		void SetIsHovered(bool Value) { bIsHovered = Value; }

	private:

		bool bIsPressed = false;
		bool bIsHovered = false;
	};
	
	typedef TMap< FString, FString > FMetadata;
	DECLARE_DELEGATE_OneParam(FOnClick, const FMetadata& /*Metadata*/);
	DECLARE_DELEGATE_RetVal_OneParam(FText, FOnGetTooltipText, const FMetadata& /*Metadata*/);
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<IToolTip>, FOnGenerateTooltip, const FMetadata& /*Metadata*/);

	static TSharedRef<FExtendTextRun> Create(const FRunInfo& RunInfo, const TSharedRef<const FString>& Text, const FTextRange& InRange, const FTextBlockStyle& Style);

	static TSharedRef<FExtendTextRun> Create(const FRunInfo& RunInfo, const TSharedRef<const FString>& Text,
		const FHyperlinkStyle& Style,const FOnClick& NavigateDelegate,
		const FOnGenerateTooltip& TooltipDelegate, const FOnGetTooltipText& TooltipTextDelegate);
	static TSharedRef<FExtendTextRun> Create(const FRunInfo& RunInfo, const TSharedRef<const FString>& Text, const FTextBlockStyle& Style);

	virtual ~FExtendTextRun();
	virtual FTextRange GetTextRange() const override { return IsText ? TextRun->GetTextRange():HyperRun->GetTextRange(); }
	virtual void SetTextRange(const FTextRange& Value) override { IsText ? TextRun->SetTextRange(Value):HyperRun->SetTextRange(Value); }
	virtual int16 GetBaseLine(float Scale) const override;
	virtual int16 GetMaxHeight(float Scale) const override;
	virtual FVector2D Measure(int32 StartIndex, int32 EndIndex, float Scale,
		const FRunTextContext& TextContext) const override;
	virtual int8 GetKerning(int32 CurrentIndex, float Scale, const FRunTextContext& TextContext) const override;
	virtual TSharedRef<ILayoutBlock> CreateBlock(int32 StartIndex, int32 EndIndex, FVector2D Size,
		const FLayoutBlockTextContext& TextContext, const TSharedPtr<IRunRenderer>& Renderer) override;
	virtual int32 GetTextIndexAt(const TSharedRef<ILayoutBlock>& Block, const FVector2D& Location, float Scale,
		ETextHitPoint* const OutHitPoint) const override;
	virtual FVector2D GetLocationAt(const TSharedRef<ILayoutBlock>& Block, int32 Offset, float Scale) const override;
	virtual void BeginLayout() override;
	virtual void EndLayout() override;
	virtual void Move(const TSharedRef<FString>& NewText, const FTextRange& NewRange) override;
	virtual TSharedRef<IRun> Clone() const override;
	virtual void AppendTextTo(FString& Text) const override;
	virtual void AppendTextTo(FString& Text, const FTextRange& Range) const override;
	virtual const FRunInfo& GetRunInfo() const override { return IsText ? TextRun->GetRunInfo():HyperRun->GetRunInfo(); }
	virtual ERunAttributes GetRunAttributes() const override;
	virtual int32 OnPaint(const FPaintArgs& PaintArgs, const FTextArgs& TextArgs, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual const TArray<TSharedRef<SWidget>>& GetChildren() override;
	virtual void ArrangeChildren(const TSharedRef<ILayoutBlock>& Block, const FGeometry& AllottedGeometry,
		FArrangedChildren& ArrangedChildren) const override;

	void ApplyFontSizeMultiplierOnTextStyle(float FontSizeMultiplier);

protected:

	FExtendTextRun(const FRunInfo& RunInfo, const TSharedRef<const FString>& Text,
		const FHyperlinkStyle& Style,const FOnClick& NavigateDelegate, const FOnGenerateTooltip& TooltipDelegate,
		const FOnGetTooltipText& TooltipTextDelegate)
		:IsText(false),
		HyperRun(FSlateHyperlinkRun::Create(RunInfo,Text,Style,NavigateDelegate,TooltipDelegate,TooltipTextDelegate))
	{
	}

	FExtendTextRun(const FRunInfo& RunInfo, const TSharedRef<const FString> Text, const FTextRange& InRange,const FTextBlockStyle& Style)
		:IsText(true),
		TextRun(FSlateTextRun::Create(RunInfo, Text, Style, InRange))
	{
	}

	FExtendTextRun(const FRunInfo& RunInfo, const TSharedRef<const FString> Text, const FTextBlockStyle& Style)
		:IsText(true),
		TextRun(FSlateTextRun::Create(RunInfo, Text, Style))
	{
	}


	bool IsText = false;
	TSharedPtr<FSlateTextRun> TextRun;
	TSharedPtr<FSlateHyperlinkRun> HyperRun;
};
