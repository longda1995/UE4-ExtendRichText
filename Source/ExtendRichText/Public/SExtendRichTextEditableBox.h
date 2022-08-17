#pragma once

#include "CoreMinimal.h"
#include "ExtendRichTextStyle.h"
#include "Components/RichTextBlock.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Text/SlateEditableTextLayout.h"

class SExtendRichTextEditableBox;
class SMultiLineEditableTextBox;
class SColorBlock;
class SEditableTextBox;
class EXTENDRICHTEXT_API SButtonWithOccupyState : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SButtonWithOccupyState){}
	SLATE_ARGUMENT(FName, StyleName)
	SLATE_ARGUMENT(FOnClicked,OnClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& args)
	{
		ChildSlot
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(FExtendRichTextEditBoxStyles::Get().GetBrush("EmptyBrush"))
					.Visibility(EVisibility::HitTestInvisible)
					.BorderBackgroundColor_Lambda([&]
					{
						if(IsOccupied())
						{
							return FLinearColor(0,0,0,0.5f);
						}
						else
						{
							return FLinearColor(0, 0, 0, 0);
						}
						
					})
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SButton)
					.ButtonStyle(FExtendRichTextEditBoxStyles::Get(),args._StyleName)
					.OnClicked(args._OnClicked)
					.OnPressed(this,&SButtonWithOccupyState::OnPressed)
				]
			];
	}
	
	void OnPressed()
	{
		SetOccupied(!IsOccupied());
	}
	
	inline bool IsOccupied()
	{
		return bIsOccupied;
	}
	
	inline void SetOccupied(bool occupied)
	{
		bIsOccupied = occupied;
	}
protected:
	bool bIsOccupied = false;
};


class SHyperLinkBar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHyperLinkBar) { }

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	TSharedPtr<SBorder> CommonColorBtn;
	TSharedPtr<SBorder> HoverColorBtn;
	TSharedPtr<SBorder> ClickedColorBtn;
	TSharedPtr<SButton> ClickEventBtn;
	TSharedPtr<SButton> HoverEventBtn;
	TSharedPtr<SMultiLineEditableTextBox> HyperMetaInputBox;
};

struct FRichTextStyleState
{
	FRichTextStyleState()
		: FontSize(GetDefault<UExtendRichTextSettings>()->TextDefaultSize),
		 Bold(false),
		 Italic(false),
		 UnderLine(false),
		 IsText(true),
		 ColorIndex(0)
	{
	}
	
	FRichTextStyleState(uint8 FontSize, uint8 Bold, uint8 Italic, uint8 UnderLine, uint8 isText,uint8 ColorIndex)
		: FontSize(FontSize),
		  Bold(Bold),
		  Italic(Italic),
		  UnderLine(UnderLine),
		  IsText(isText),	
		  ColorIndex(ColorIndex)
	{
	}

	bool operator ==(const FRichTextStyleState& other)const
	{
		return FontSize == other.FontSize
			&& Bold == other.Bold
			&& Italic == other.Italic
			&& UnderLine == other.UnderLine
			&& ColorIndex == other.ColorIndex;
	}
	
	friend uint32 SerializeToUint32(const FRichTextStyleState& Other)
	{
		return ((uint32)Other.FontSize) << 16
		| ((uint32)Other.ColorIndex) << 8
		| ((uint32)Other.Bold) << 3
		| ((uint32)Other.Italic) << 2
		| ((uint32)Other.UnderLine) << 1
		| Other.IsText;
	}
	
	static FRichTextStyleState DeserializeFromUint32(uint32 num)
	{
		return FRichTextStyleState(num >> 16,num >> 3,num >> 2,num >> 1,num,num >> 8);
	}
	
	friend uint32 GetTypeHash(const FRichTextStyleState& Other)
	{
		return SerializeToUint32(Other);
	}
	
	uint8 FontSize;
	uint8 Bold : 1;
	uint8 Italic : 1;
	uint8 UnderLine : 1;
	uint8 IsText : 1;
	uint8 ColorIndex;
};

class SExtendRichTextEditorBar final: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SExtendRichTextEditorBar)
		:_EditableBox(nullptr)
	{}
		SLATE_ARGUMENT(SExtendRichTextEditableBox* ,EditableBox)
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);
	virtual bool SupportsKeyboardFocus() const override{return true;}
	
	const FRichTextStyleState GetRichTextStyleState();
	TSharedPtr<SMenuAnchor> GetMenuAnchor() { return ColorMenu; }
	void SetCurrentColor(uint8 color);
	void SetCurrentStyleState(const FRichTextStyleState& state);
	void NotifyEditorBoxChange();
	void SetFontSizeForSelectedText(uint8 newSize);
protected:
	FReply OnBoldClick();
	FReply OnItalicClick();
	FReply OnUnderLineClick();
	FReply OnColorBtnClick();
	void OnFontSizeCommitted(const FText&, ETextCommit::Type);
	FReply OnPainterBtnClick();
	FReply OnInsertImgBtnClick();
	FReply OnHyperBtnClick();
	FReply OnEraserBtnClick();
	FReply OnExpandBtnClick();

	bool IsBoldEnable();
	bool IsItalicEnable();
	bool IsUnderLineEnable();
	bool IsColorEnable();
	bool IsFontSizeEnable();
	bool IsPainterEnable();
	bool IsInsertImgBtnEnable();
	bool IsHyperBtnEnable();
	bool IsEraserBtnEnable();

	TSharedPtr<SButtonWithOccupyState> BoldBtn;
	TSharedPtr<SButtonWithOccupyState> ItalicBtn;
	TSharedPtr<SButtonWithOccupyState> UnderLineBtn;
	TSharedPtr<SButton> ColorBtn;
	TSharedPtr<SEditableTextBox> FontSizeInput;
	TSharedPtr<SButtonWithOccupyState> PainterBtn;
	TSharedPtr<SButton> InsertImgBtn;
	TSharedPtr<SButtonWithOccupyState> HyperBtn;
	TSharedPtr<SButton> EraserBtn;
	TSharedPtr<SMenuAnchor> ColorMenu;
	TSharedPtr<SMenuAnchor> FontSizeMenu;
	
	uint8 CurrentColorIndex = 0;
	SExtendRichTextEditableBox* EditableBox;
	FRichTextStyleState SelectedState;
};

enum class EStyleOperationCode
{
	IsText,
	Bold,
	Italic,
	UnderLine,
	FontSize,
	Color
};

using OperationInfo = ::TPair<EStyleOperationCode, uint8>;
/**
 * 
 */
class EXTENDRICHTEXT_API SExtendRichTextEditableBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SExtendRichTextEditableBox){}
	SLATE_ATTRIBUTE(FText,Text)
	SLATE_ARGUMENT(FOnTextCommitted,OnTextCommitted)
	SLATE_ARGUMENT(FOnTextChanged,OnTextChanged)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	inline FRichTextStyleState GetCurrentTextStyleState()const { return RichTextEditorBar->GetRichTextStyleState(); }
	void HandleStyleChanged();
	void BoldSelectedText(bool isBold);
	void ItalicSelectText(bool isItalic);
	void UnderLineSelectText(bool isUnderLine);
	void ColorSelectedText(uint8 colorIndex);
	void ResizeSelectText(uint8 size);
	void StyleSelectedText(const OperationInfo& info);
	const FRunInfo BuildRunInfoFromStyleState(const FRichTextStyleState style)const;
	TArray<TSharedRef<const IRun>> GetSelectedRuns();
	bool HasAnyTextSelected() { return RichTextEditBox->AnyTextSelected(); }
	void EraseSelectedStyles();
	void InsertImageAtCursor(const FRunInfo& info,const FName resourceName,const FVector2D& size);
	bool IsEditBoxFocused();
protected:

	void HandleRichEditableTextChanged(const FText& Text);

	void HandleRichEditableTextCommitted(const FText& Text, ETextCommit::Type Type);

	void HandleRichEditableTextCursorMoved(const FTextLocation& NewCursorPosition);

	FOnTextChanged OnTextChanged;
	FOnTextCommitted OnTextCommitted;
	//
	TSharedPtr<SExtendRichTextEditorBar> RichTextEditorBar;
	TSharedPtr<SMultiLineEditableTextBox> RichTextEditBox;
	TSharedPtr<SHyperLinkBar> HyperLinkBar;
	FTextBlockStyle CurrentTextStyle;
	uint8 CurSize;
};
