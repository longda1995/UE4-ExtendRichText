// Fill out your copyright notice in the Description page of Project Settings.

#include "SExtendRichTextEditableBox.h"
#include "ExtendRichTextStyle.h"
#include "ExtendRichTextBlockDecorator.h"
#include "ExtendRichTextMarshaller.h"
#include "ExtendRichTextSettings.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/RichTextBlockImageDecorator.h"
#include "Framework/Text/ITextDecorator.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Framework/Text/SlateImageRun.h"
#include "Styling/SlateStyle.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include <Developer/DesktopPlatform/Public/DesktopPlatformModule.h>
#include <Editor/UnrealEd/Public/EditorDirectories.h>

#include "ExtendHyperEventHandler.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "ExtendRichText"

DECLARE_DELEGATE_OneParam(FOnGetColor, uint8)
class SColorBtn : public SBorder
{
public:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		OnGetColor.ExecuteIfBound(ColorIndex);
		return SBorder::OnMouseButtonDown(MyGeometry, MouseEvent);
	}
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		auto Color = GetDefault<UExtendRichTextSettings>()->TextColors[ColorIndex];
		SetBorderBackgroundColor(Color*0.5f);
	}
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
	{
		auto Color = GetDefault<UExtendRichTextSettings>()->TextColors[ColorIndex];
		SetBorderBackgroundColor(Color);
	}
	uint8 ColorIndex;
	FOnGetColor OnGetColor;
};

DECLARE_DELEGATE_OneParam(FOnGetFontSize, uint8)
class SFontSizeBtn : public SButton
{
public:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		OnGetFontSize.ExecuteIfBound(FontSize);
		return SBorder::OnMouseButtonDown(MyGeometry, MouseEvent);
	}
	uint8 FontSize;
	FOnGetFontSize OnGetFontSize;
};

class SFontSizeChooser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFontSizeChooser){}
		SLATE_ARGUMENT(SEditableTextBox*,FontSizeBox)
		SLATE_ARGUMENT(TArray<uint8>, SizeArr)
		SLATE_ARGUMENT(SExtendRichTextEditorBar*,EditorBar)
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs)
	{
		FontSizeInputBox = InArgs._FontSizeBox;
		EditorBar = InArgs._EditorBar;
		TSharedPtr<SScrollBox> scrollBox;
		ChildSlot
		[
			SNew(SBox)
			.HeightOverride(100)
			.WidthOverride(40)
			[
				SAssignNew(scrollBox, SScrollBox)
				.ScrollBarVisibility(EVisibility::Collapsed)
			]
		];
		
		for(auto size : InArgs._SizeArr)
		{
			auto sizeBtn = SNew(SFontSizeBtn)
				.TextStyle(FExtendRichTextEditBoxStyles::Get(), "FontSizeBtn")
				.Text(FText::AsNumber(size)) 
				.ButtonStyle(FExtendRichTextEditBoxStyles::Get(), "BlackBtnStyle")
				.HAlign(HAlign_Center);
			
			sizeBtn->FontSize = size;
			sizeBtn->OnGetFontSize.BindLambda([&](uint8 fontSize)
			{
				FontSizeInputBox->SetText(FText::AsNumber(fontSize));
				EditorBar->SetFontSizeForSelectedText(fontSize);
			});
			
			scrollBox->AddSlot().HAlign(HAlign_Fill).VAlign(VAlign_Fill)[sizeBtn];
		}
	};
private:
	SEditableTextBox* FontSizeInputBox;
	SExtendRichTextEditorBar* EditorBar;
};

class SColorPickPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SColorPickPanel) {}
		SLATE_ARGUMENT(TArray<FLinearColor>,Colors)
		SLATE_ARGUMENT(TSharedPtr<SMenuAnchor>,ColorMenu)
		SLATE_ARGUMENT(SExtendRichTextEditorBar*,EditorBar)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		Colors = InArgs._Colors;
		ColorMenu = InArgs._ColorMenu;
		EditorBar = InArgs._EditorBar;
		TSharedPtr<SUniformGridPanel> colorPanel;
		ChildSlot
		[
			SAssignNew(colorPanel, SUniformGridPanel)
			.SlotPadding(FMargin(1, 1, 1, 1))
		];

		for (int32 colorNum = 0; colorNum < Colors.Num(); colorNum++)
		{
			auto colorBtn = SNew(SColorBtn)
				.BorderImage(FExtendRichTextEditBoxStyles::Get().GetBrush("EmptyBrush"))
				.Padding(FMargin(10, 10))
				.BorderBackgroundColor(Colors[colorNum]);

			colorBtn->ColorIndex = colorNum;
			colorBtn->OnGetColor.BindLambda([&](uint8 colorIndex)
			{
				ColorMenu->SetIsOpen(false);
				EditorBar->SetCurrentColor(colorIndex);
			});
			colorPanel->AddSlot(colorNum % 4, colorNum / 4)[colorBtn];
		}
	}

private:
	TArray<FLinearColor> Colors;
	TSharedPtr<SMenuAnchor> ColorMenu;
	SExtendRichTextEditorBar* EditorBar;
};

void SHyperLinkBar::Construct(const FArguments& InArgs)
{
	const auto btnMargin = FMargin(2.f, 1.f, 2.f, 1.f);
	const auto textPadding = FMargin(10.f,4.f,10.f,4.f);

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(btnMargin)
			[
				SAssignNew(CommonColorBtn, SBorder)
				.Padding(textPadding)
				.BorderImage(FExtendRichTextEditBoxStyles::Get().GetBrush("EmptyBrush"))
				.BorderBackgroundColor_Lambda([]
				{
					return FSlateColor(FLinearColor(1,1,1,1));
				})
				.ToolTipText(LOCTEXT("HyperCommonColor", "Normal Color"))
				[
					SNew(STextBlock)
					.Text(FText::FromString("N"))
				]
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(btnMargin)
			[
				SAssignNew(HoverColorBtn, SBorder)
				.BorderImage(FExtendRichTextEditBoxStyles::Get().GetBrush("EmptyBrush"))
				.BorderBackgroundColor_Lambda([]
				{
					return FSlateColor(FLinearColor(1, 1, 1, 1));
				})
				.Padding(textPadding)
				.ToolTipText(LOCTEXT("HyperHoverColor", "Hover Color"))
				[
					SNew(STextBlock)
					.Text(FText::FromString("H"))
				]
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(btnMargin)
			[
				SAssignNew(ClickedColorBtn, SBorder)
				.BorderImage(FExtendRichTextEditBoxStyles::Get().GetBrush("EmptyBrush"))
				.BorderBackgroundColor_Lambda([]
				{
					return FSlateColor(FLinearColor(1, 1, 1, 1));
				})
				.Padding(textPadding)
				.ToolTipText(LOCTEXT("HyperClickedColor", "Clicked	Color"))
				[
					SNew(STextBlock)
					.Text(FText::FromString("C"))
				]
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(btnMargin)
			[
				SAssignNew(ClickEventBtn, SButton)
				.ToolTipText(LOCTEXT("ClickEveBtn", "On Click Event"))
				[
					SNew(STextBlock)
					.Text(FText::FromString("CE"))
				]
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(btnMargin)
			[
				SAssignNew(HoverEventBtn, SButton)
				.ToolTipText(LOCTEXT("HoverEveBtn", "On Hover Event"))
				[
					SNew(STextBlock)
					.Text(FText::FromString("HE"))
				]
			]
		]
	+SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(btnMargin)
		[
			SAssignNew(HyperMetaInputBox,SMultiLineEditableTextBox)
			.HintText(LOCTEXT("MetaInput","Input Meta here!"))
			.TextStyle(FExtendRichTextEditBoxStyles::Get(),"DefaultTextStyle")
			.AllowMultiLine(true)
		]
	];
}


class SColorPickPanel;
const FMargin iconPadding = FMargin(2,0);
void SExtendRichTextEditorBar::Construct(const FArguments& InArgs)
{
	EditableBox = InArgs._EditableBox;
	
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBorder)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(FMargin(0))
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					SNew(SImage)
					.Image(FExtendRichTextEditBoxStyles::Get().GetBrush("FontSizeBrush"))
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(0))
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(28)
					.HeightOverride(15)
					[
						SAssignNew(FontSizeInput,SEditableTextBox)
						.Text(FText::AsNumber(GetDefault<UExtendRichTextSettings>()->TextDefaultSize))
						.OnTextCommitted(this,&SExtendRichTextEditorBar::OnFontSizeCommitted)
						.Font(FExtendRichTextEditBoxStyles::Get().GetFontStyle("SizeFont"))
					]
				]
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				[
					SNew(SOverlay)
					+SOverlay::Slot()
					[
						SAssignNew(FontSizeMenu,SMenuAnchor)
						.OnGetMenuContent_Lambda([&] 
						{
							auto setting = GetDefault<UExtendRichTextSettings>();
							return SNew(SFontSizeChooser)
							.EditorBar(this)
							.FontSizeBox(FontSizeInput.Get())
							.SizeArr(setting->DefaultFontSizeList);
						})
					]
					+SOverlay::Slot()
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.ButtonStyle(FExtendRichTextEditBoxStyles::Get(),"FontSizeExpandBtn")
						.OnClicked(this,&SExtendRichTextEditorBar::OnExpandBtnClick)
					]
				]
			]
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(iconPadding)
		.AutoWidth()
		[
			SAssignNew(BoldBtn,SButtonWithOccupyState)
			.StyleName("BoldBtnStyle")
			.OnClicked(FOnClicked::CreateSP(this,&SExtendRichTextEditorBar::OnBoldClick))
			.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this,&SExtendRichTextEditorBar::IsBoldEnable)))
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(iconPadding)
		.AutoWidth()
		[
			SAssignNew(ItalicBtn, SButtonWithOccupyState)
			.StyleName("ItalicBtnStyle")
			.OnClicked(FOnClicked::CreateSP(this, &SExtendRichTextEditorBar::OnItalicClick))
			.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this,&SExtendRichTextEditorBar::IsItalicEnable)))
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(iconPadding)
		.AutoWidth()
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(ColorBtn,SButton)
				.ButtonStyle(FExtendRichTextEditBoxStyles::Get(),"ColorBtnStyle")
				.OnClicked(FOnClicked::CreateSP(this, &SExtendRichTextEditorBar::OnColorBtnClick))
				.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &SExtendRichTextEditorBar::IsColorEnable)))
			]
			+ SOverlay::Slot()
			.Padding(FMargin(0,0,0,3))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Bottom)
			[
				SAssignNew(ColorMenu,SMenuAnchor)
				.OnGetMenuContent_Lambda([&]
				{
					return SNew(SColorPickPanel)
					.Colors(GetDefault<UExtendRichTextSettings>()->TextColors)
					.EditorBar(this)
					.ColorMenu(ColorMenu);
				})
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(0,0,0,3))
			[
				SNew(SBorder)
				.Padding(FMargin(8,1.5f))
				.Visibility(EVisibility::HitTestInvisible)
				.BorderImage(FExtendRichTextEditBoxStyles::Get().GetBrush("EmptyBrush"))
				.BorderBackgroundColor_Lambda([&]
				{
					return GetDefault<UExtendRichTextSettings>()->TextColors[CurrentColorIndex];
				})
			]
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(iconPadding)
		.AutoWidth()
		[
			SAssignNew(UnderLineBtn,SButtonWithOccupyState)
			.StyleName("UnderLineBtnStyle")
			.OnClicked(FOnClicked::CreateSP(this,&SExtendRichTextEditorBar::OnUnderLineClick))
			.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this,&SExtendRichTextEditorBar::IsUnderLineEnable)))
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(iconPadding)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SAssignNew(PainterBtn,SButtonWithOccupyState)
			.StyleName("PainterBtnStyle")
			.OnClicked(FOnClicked::CreateSP(this,&SExtendRichTextEditorBar::OnPainterBtnClick))
			.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &SExtendRichTextEditorBar::IsPainterEnable)))
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(iconPadding)
		.AutoWidth()
		[
			SAssignNew(EraserBtn,SButton)
			.ButtonStyle(FExtendRichTextEditBoxStyles::Get(),"EraserBtnStyle")
			.OnClicked(this,&SExtendRichTextEditorBar::OnEraserBtnClick)
			.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &SExtendRichTextEditorBar::IsEraserBtnEnable)))
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(iconPadding)
		.AutoWidth()
		[
			SAssignNew(InsertImgBtn,SButton)
			.ButtonStyle(FExtendRichTextEditBoxStyles::Get(),"InsertImgBtnStyle")
			.OnClicked(this,&SExtendRichTextEditorBar::OnInsertImgBtnClick)
			.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &SExtendRichTextEditorBar::IsInsertImgBtnEnable)))
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(iconPadding)
		.AutoWidth()
		[
			SAssignNew(HyperBtn,SButton)
			.ButtonStyle(FExtendRichTextEditBoxStyles::Get(),"InsertHyperLinkBtn")
			.OnClicked(this,&SExtendRichTextEditorBar::OnHyperBtnClick)
			.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &SExtendRichTextEditorBar::IsHyperBtnEnable)))
		]
	];
}

const FRichTextStyleState SExtendRichTextEditorBar::GetRichTextStyleState()
{
	return FRichTextStyleState(FCString::Atof(*FontSizeInput->GetText().ToString()),
		BoldBtn->IsOccupied(),
		ItalicBtn->IsOccupied(),
		UnderLineBtn->IsOccupied(),
		1,
		CurrentColorIndex);
}

void SExtendRichTextEditorBar::SetCurrentColor(uint8 color)
{
	CurrentColorIndex = color;
	EditableBox->ColorSelectedText(CurrentColorIndex);
}

void SExtendRichTextEditorBar::SetCurrentStyleState(const FRichTextStyleState& state)
{
	BoldBtn->SetOccupied(state.Bold);
	ItalicBtn->SetOccupied(state.Italic);
	UnderLineBtn->SetOccupied(state.UnderLine);
	CurrentColorIndex = state.ColorIndex;
	int size = FCString::Atoi(*FontSizeInput->GetText().ToString());
	if(size != state.FontSize)
	{
		FontSizeInput->SetText(FText::AsNumber(state.FontSize));
	}
}

void SExtendRichTextEditorBar::NotifyEditorBoxChange()
{
	EditableBox->HandleStyleChanged();
}

void SExtendRichTextEditorBar::SetFontSizeForSelectedText(uint8 newSize)
{
	OperationInfo info = MakeTuple(EStyleOperationCode::FontSize,newSize);
	EditableBox->StyleSelectedText(info);
}

FReply SExtendRichTextEditorBar::OnBoldClick()
{
	//��ת״̬
	EditableBox->BoldSelectedText(BoldBtn->IsOccupied());
	return FReply::Handled();
}

FReply SExtendRichTextEditorBar::OnItalicClick()
{
	//��ת״̬
	EditableBox->ItalicSelectText(ItalicBtn->IsOccupied());
	return FReply::Handled();
}

FReply SExtendRichTextEditorBar::OnUnderLineClick()
{
	EditableBox->UnderLineSelectText(UnderLineBtn->IsOccupied());
	return FReply::Handled();
}

FReply SExtendRichTextEditorBar::OnColorBtnClick()
{
	if(ColorMenu.IsValid())
	{
		ColorMenu->SetIsOpen(true);
	}
	return FReply::Handled();
}

void SExtendRichTextEditorBar::OnFontSizeCommitted(const FText& text, ETextCommit::Type type)
{
	EditableBox->ResizeSelectText(FCString::Atoi(*text.ToString()));
}

FReply SExtendRichTextEditorBar::OnPainterBtnClick()
{
	auto selectedRuns = EditableBox->GetSelectedRuns();
	
	if(selectedRuns.Num() == 1)
	{
		FRunInfo runInfo = selectedRuns[0]->GetRunInfo();
		if(runInfo.Name.IsEmpty())
		{
			
		}
		else if(runInfo.Name.Equals(GetDefault<UExtendRichTextSettings>()->ExtendTagName))
		{
			
		}
	}
	return FReply::Handled();
}

FReply SExtendRichTextEditorBar::OnInsertImgBtnClick()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		TArray<FString> OutFiles;
		const FString Extension(TEXT("png"));
		const FString Filter = FString::Printf(TEXT("%s files (*.%s)|*.%s"), *Extension, *Extension, *Extension);
		const FString DefaultPath = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_IMPORT);

		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
		void* ParentWindowHandle = (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid()) ? ParentWindow->GetNativeWindow()->GetOSWindowHandle() : nullptr;

		if (DesktopPlatform->OpenFileDialog(ParentWindowHandle, FText::Format(LOCTEXT("ImagePickerDialogTitle", "Choose a {0} file"), FText::FromString(Extension)).ToString(), DefaultPath, TEXT(""), Filter, EFileDialogFlags::None, OutFiles))
		{
			check(OutFiles.Num() == 1);

			FRunInfo RunInfo(TEXT("img"));

			// the path to the image needs to be stored either as a 'long package name' version of itself
			// (minus png extension) or as a literal (base dir relative) path.
			FString ContentPath;
			if (FPackageName::TryConvertFilenameToLongPackageName(OutFiles[0], ContentPath))
			{
				RunInfo.MetaData.Add(TEXT("src"), ContentPath);
			}
			else
			{
				RunInfo.MetaData.Add(TEXT("src"), OutFiles[0]);
			}
			
			EditableBox->InsertImageAtCursor(RunInfo,FName(*FExtendImageDecorator::GetPathToImage(OutFiles[0])),FVector2D(8,8));

			FEditorDirectories::Get().SetLastDirectory(ELastDirectory::GENERIC_IMPORT, FPaths::GetPath(OutFiles[0]));
		}
	}
	return FReply::Handled();
}

FReply SExtendRichTextEditorBar::OnHyperBtnClick()
{
	OperationInfo info = MakeTuple(EStyleOperationCode::IsText,0);
	EditableBox->StyleSelectedText(info);
	return FReply::Handled();
}

FReply SExtendRichTextEditorBar::OnEraserBtnClick()
{
	EditableBox->EraseSelectedStyles();
	return FReply::Handled();
}

FReply SExtendRichTextEditorBar::OnExpandBtnClick()
{
	FontSizeMenu->SetIsOpen(true);
	return FReply::Handled();
}

bool SExtendRichTextEditorBar::IsBoldEnable()
{
	return EditableBox->HasAnyTextSelected();
}

bool SExtendRichTextEditorBar::IsItalicEnable()
{
	return EditableBox->HasAnyTextSelected();
}

bool SExtendRichTextEditorBar::IsUnderLineEnable()
{
	return EditableBox->HasAnyTextSelected();
}

bool SExtendRichTextEditorBar::IsColorEnable()
{
	return EditableBox->HasAnyTextSelected();
}

bool SExtendRichTextEditorBar::IsFontSizeEnable()
{
	return EditableBox->HasAnyTextSelected();
}

bool SExtendRichTextEditorBar::IsPainterEnable()
{
	return EditableBox->GetSelectedRuns().Num() == 1;
}

bool SExtendRichTextEditorBar::IsInsertImgBtnEnable()
{
	return true;
}

bool SExtendRichTextEditorBar::IsHyperBtnEnable()
{
	return EditableBox->HasAnyTextSelected();
}

bool SExtendRichTextEditorBar::IsEraserBtnEnable()
{
	return EditableBox->GetSelectedRuns().Num() > 0;
}

void SExtendRichTextEditableBox::Construct(const FArguments& InArgs)
{
	OnTextCommitted = InArgs._OnTextCommitted;
	OnTextChanged = InArgs._OnTextChanged;
	const UExtendRichTextSettings* Setting = GetDefault<UExtendRichTextSettings>();
	CurSize = Setting->TextDefaultSize;

	//FExtendRichTextMarshaller
	const TSharedRef<FExtendRichTextMarshaller> RichTextMarshaller = FExtendRichTextMarshaller::Create(
		TArray<TSharedRef<ITextDecorator>>(),
		&FExtendRichTextEditBoxStyles::Get()
	);
	const UExtendHyperEventHandler* Handler = GetDefault<UExtendHyperEventHandler>();
	//
	RichTextMarshaller->AppendInlineDecorator(FExtendRichTextDecorator::CreateIns(Handler));
	RichTextMarshaller->AppendInlineDecorator(FExtendImageDecorator::Create());
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
		[
			SAssignNew(RichTextEditorBar, SExtendRichTextEditorBar)
			.EditableBox(this)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign( HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
		[
			SAssignNew(RichTextEditBox, SMultiLineEditableTextBox)
			.OnTextChanged(this, &SExtendRichTextEditableBox::HandleRichEditableTextChanged)
			.OnTextCommitted(this, &SExtendRichTextEditableBox::HandleRichEditableTextCommitted)
			.OnCursorMoved(this, &SExtendRichTextEditableBox::HandleRichEditableTextCursorMoved)
			.Marshaller(RichTextMarshaller)
			.Font(FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FTextBlockStyle>("DefaultTextStyle").Font)
			.ClearTextSelectionOnFocusLoss(false)
			.AutoWrapText(true)
			.Margin(4)
			.LineHeightPercentage(1.1f)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(0.f, 0.f, 0.f, 0.f))
		[
			SAssignNew(HyperLinkBar, SHyperLinkBar)
			.Visibility_Lambda([&]
			{
				auto runs = RichTextEditBox->GetSelectedRuns();
				if(runs.Num() == 1 && runs[0]->GetRunInfo().Name.Equals(GetDefault<UExtendRichTextSettings>()->ExtendTagName))
				{
					FString stateStr = runs[0]->GetRunInfo().MetaData[TEXT("Style")];
					FRichTextStyleState state = FRichTextStyleState::DeserializeFromUint32(FCString::Atoi(*stateStr));
					if(!state.IsText)
					{
						return EVisibility::Visible;
					}
				}
				return EVisibility::Collapsed;
			})
		]
	];
	RichTextEditBox->SetText(InArgs._Text);
}

void SExtendRichTextEditableBox::HandleStyleChanged()
{
	FString SelectedText = RichTextEditBox->GetSelectedText().ToString();
	if(SelectedText.Len() > 0)
	{
		for (int32 SelectedTextIndex = 0; SelectedTextIndex < SelectedText.Len(); ++SelectedTextIndex)
		{
			if (FChar::IsLinebreak(SelectedText[SelectedTextIndex]))
			{
				SelectedText.LeftInline(SelectedTextIndex, false);
				break;
			}
		}
		const FRichTextStyleState state = GetCurrentTextStyleState();
		const FRunInfo runInfo = BuildRunInfoFromStyleState(state);

		FString styleIndexStr = FString::FromInt(SerializeToUint32(state));
		
		if(state.IsText)
		{
			if(!FExtendRichTextEditBoxStyles::Get().HasWidgetStyle<FTextBlockStyle>(FName(styleIndexStr)))
			{
				FExtendRichTextEditBoxStyles::Get().Set(FName(styleIndexStr), FExtendRichTextStyleMaker::MakeTextStyleByState(state));
			}
			RichTextEditBox->InsertRunAtCursor(FRunCreator::CreateTextRun(runInfo, MakeShareable(new FString(SelectedText)), 
				FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FTextBlockStyle>(FName(styleIndexStr))));
		}
		else
		{
			if(!FExtendRichTextEditBoxStyles::Get().HasWidgetStyle<FHyperlinkStyle>(FName(styleIndexStr)))
			{
				FExtendRichTextEditBoxStyles::Get().Set(FName(styleIndexStr), *FExtendRichTextStyleMaker::MakeStyleByState(state));
			}
			RichTextEditBox->InsertRunAtCursor(FRunCreator::CreateHyperRun(runInfo, MakeShareable(new FString(SelectedText)), FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FHyperlinkStyle>(FName(styleIndexStr)), 
				NULL, NULL, NULL));
		}
	}
}

void SExtendRichTextEditableBox::BoldSelectedText(bool isBold)
{
	if(!RichTextEditBox->GetSelectedText().IsEmpty())
	{
		OperationInfo info = MakeTuple(EStyleOperationCode::Bold,isBold ? 1 : 0);
		StyleSelectedText(info);
	}
}

void SExtendRichTextEditableBox::ItalicSelectText(bool isItalic)
{
	if (!RichTextEditBox->GetSelectedText().IsEmpty())
	{
		OperationInfo info = MakeTuple(EStyleOperationCode::Italic, isItalic ? 1 : 0);
		StyleSelectedText(info);
	}
}

void SExtendRichTextEditableBox::UnderLineSelectText(bool isUnderLine)
{
	if (!RichTextEditBox->GetSelectedText().IsEmpty())
	{
		OperationInfo info = MakeTuple(EStyleOperationCode::UnderLine, isUnderLine ? 1 : 0);
		StyleSelectedText(info);
	}
}

void SExtendRichTextEditableBox::ColorSelectedText(uint8 colorIndex)
{
	if (!RichTextEditBox->GetSelectedText().IsEmpty())
	{
		OperationInfo info = MakeTuple(EStyleOperationCode::Color, colorIndex);
		StyleSelectedText(info);
	}
}

void SExtendRichTextEditableBox::ResizeSelectText(uint8 size)
{
	if (!RichTextEditBox->GetSelectedText().IsEmpty())
	{
		OperationInfo info = MakeTuple(EStyleOperationCode::FontSize, size);
		StyleSelectedText(info);
	}
}

void SExtendRichTextEditableBox::StyleSelectedText(const OperationInfo& info)
{
	using RunPairs = TArray<TPair<FRichTextStyleState, FString>>;
	if(FText::TrimPrecedingAndTrailing(RichTextEditBox->GetSelectedText()).IsEmpty())
	{
		return;
	}
	
	auto makeNewStyleStateByOperation = [info](const FRichTextStyleState& state)
	{
		FRichTextStyleState newState = state;
		switch (info.Key) {
		case EStyleOperationCode::Bold: 
			newState.Bold = (info.Value != 0);
			break;
		case EStyleOperationCode::Italic: 
			newState.Italic = (info.Value != 0);
			break;
		case EStyleOperationCode::UnderLine: 
			newState.UnderLine = (info.Value != 0);
			break;
		case EStyleOperationCode::FontSize: 
			newState.FontSize = info.Value;
			break;
		case EStyleOperationCode::Color:
			newState.ColorIndex = info.Value;
			break;
		case EStyleOperationCode::IsText:
			newState.IsText = (info.Value != 0);
			break;
		default: 
			break;
		}
		return newState;
	};
	
	auto selectedRuns = GetSelectedRuns();
	auto selectedText = RichTextEditBox->GetSelectedText();

	RunPairs runInfosToInsert;
	//�����м��Run
	for (int i = 0; i < selectedRuns.Num(); i++)
	{
		auto run = selectedRuns[i];
		FString RunText;
		run->AppendTextTo(RunText);

		FString newRunContent;
		if (i == 0 || i == selectedRuns.Num() - 1)//��һ���������һ��
		{
			auto selectTextStr = selectedText.ToString();
			//��ȫѡ�и�Run
			if (i == 0 ? selectTextStr.StartsWith(RunText) : selectTextStr.EndsWith(RunText))
			{
				newRunContent = RunText;
			}
			else//ֻѡ�в���
			{
				//��������һ��Run�����䷴ת����Ӧ��һ���㷨
				if (i == selectedRuns.Num() - 1)
				{
					selectTextStr = selectTextStr.Reverse();
					RunText = RunText.Reverse();
				}
				
				int num = 1;
				while (true)
				{
					if(num <= selectTextStr.Len())
					{
						FString tempStr = selectTextStr.Mid(0, num);
						if(RunText.Find(tempStr, ESearchCase::CaseSensitive, ESearchDir::FromEnd) >= 0)
						{
							newRunContent = tempStr;
							num++;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}

				if (i == selectedRuns.Num() - 1)
					newRunContent = newRunContent.Reverse();
			}
		}
		else
		{
			newRunContent = RunText;
		}
		
		auto setting = GetDefault<UExtendRichTextSettings>();
		auto runInfo = run->GetRunInfo();
		if (runInfo.Name.IsEmpty())
		{
			FRichTextStyleState state;
			runInfosToInsert.Add(MakeTuple(makeNewStyleStateByOperation(state), newRunContent));
		}
		else if (runInfo.Name.Equals(setting->ExtendTagName))
		{
			FString styleStr = runInfo.MetaData[TEXT("Style")];
			FRichTextStyleState state = FRichTextStyleState::DeserializeFromUint32(FCString::Atoi(*styleStr));
			runInfosToInsert.Add(MakeTuple(makeNewStyleStateByOperation(state), newRunContent));
		}
	}

	//�������ڵ�run
	TArray<TArray<int>> SameRunStateArr;
	for(int i = 0;i < runInfosToInsert.Num();i++)
	{
		if(i == 0)
		{
			SameRunStateArr.Add({0});
		}
		else
		{
			if(runInfosToInsert[i].Key == runInfosToInsert[i-1].Key)
			{
				for(auto& indexSet : SameRunStateArr)
				{
					if(indexSet.Contains(i-1))
					{
						indexSet.Add(i);
					}
				}
			}
			else
			{
				SameRunStateArr.Add({i});
			}
		}
	}

	//�ϲ����ڵ�Run
	RunPairs finalRuns;
	for(auto runSet : SameRunStateArr)
	{
		FString newStr;
		for(auto index : runSet)
		{
			newStr += runInfosToInsert[index].Value;
		}
		finalRuns.Add(MakeTuple(runInfosToInsert[runSet[0]].Key, newStr));
	}

	//���µ�run���뵽�������
	for (auto run : finalRuns)
	{
		FRichTextStyleState state = run.Key;
		const FRunInfo runInfo = BuildRunInfoFromStyleState(state);

		FString styleIndexStr = FString::FromInt(SerializeToUint32(state));

		if (state.IsText)
		{
			if (!FExtendRichTextEditBoxStyles::Get().HasWidgetStyle<FTextBlockStyle>(FName(styleIndexStr)))
			{
				FExtendRichTextEditBoxStyles::Get().Set(FName(styleIndexStr), FExtendRichTextStyleMaker::MakeTextStyleByState(state));
			}
			RichTextEditBox->InsertRunAtCursor(FRunCreator::CreateTextRun(runInfo, MakeShareable(new FString(run.Value)),
				FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FTextBlockStyle>(FName(styleIndexStr))));
		}
		else
		{
			if (!FExtendRichTextEditBoxStyles::Get().HasWidgetStyle<FHyperlinkStyle>(FName(styleIndexStr)))
			{
				FExtendRichTextEditBoxStyles::Get().Set(FName(styleIndexStr), *FExtendRichTextStyleMaker::MakeStyleByState(state));
			}
			RichTextEditBox->InsertRunAtCursor(FRunCreator::CreateHyperRun(runInfo, MakeShareable(new FString(run.Value)), FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FHyperlinkStyle>(FName(styleIndexStr)),
				NULL, NULL, NULL));
		}
	}
}

const FRunInfo SExtendRichTextEditableBox::BuildRunInfoFromStyleState(const FRichTextStyleState styleState)const
{
	FRunInfo runInfo(GetDefault<UExtendRichTextSettings>()->ExtendTagName);
	
	runInfo.MetaData.Add(TEXT("Style"),FString::FromInt(SerializeToUint32(styleState)));
	return runInfo;
}


TArray<TSharedRef<const IRun>> SExtendRichTextEditableBox::GetSelectedRuns()
{
	return RichTextEditBox->GetSelectedRuns();
}

void SExtendRichTextEditableBox::EraseSelectedStyles()
{
	auto selectedText = RichTextEditBox->GetSelectedText();
	if(!selectedText.IsEmpty())
	{
		FRunInfo emptyInfo;
		RichTextEditBox->ApplyToSelection(emptyInfo,FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FTextBlockStyle>("DefaultTextStyle"));
	}
}

void SExtendRichTextEditableBox::InsertImageAtCursor(const FRunInfo& info, const FName resourceName, const FVector2D& size)
{
	auto imageRun = FSlateImageRun::Create(info,
		MakeShareable(new FString(TEXT("\x200B"))), // Zero-Width Breaking Space
		resourceName,
		0);
	RichTextEditBox->InsertRunAtCursor(imageRun);
}

bool SExtendRichTextEditableBox::IsEditBoxFocused()
{
	return RichTextEditBox->HasKeyboardFocus();
}

void SExtendRichTextEditableBox::HandleRichEditableTextChanged(const FText& Text)
{
	OnTextChanged.ExecuteIfBound(Text);
}

void SExtendRichTextEditableBox::HandleRichEditableTextCommitted(const FText& Text, ETextCommit::Type Type)
{
	OnTextCommitted.ExecuteIfBound(Text, Type);
}

void SExtendRichTextEditableBox::HandleRichEditableTextCursorMoved(const FTextLocation& NewCursorPosition)
{
	auto setting = GetDefault<UExtendRichTextSettings>();
	if(GetSelectedRuns().Num() > 0)
	{
		auto selectedRun = GetSelectedRuns()[0];
		//todo:����
	}
	else
	{
		auto run = RichTextEditBox->GetRunUnderCursor();
		if(run->GetRunInfo().Name.Equals(setting->ExtendTagName))
		{
			const TMap<FString, FString>& meta = run->GetRunInfo().MetaData;
			uint32 serializedNum;
			LexFromString(serializedNum, *meta[TEXT("Style")]);
			FRichTextStyleState state = FRichTextStyleState::DeserializeFromUint32(serializedNum);
			RichTextEditorBar->SetCurrentStyleState(state);
		}
		else
		{
			FRichTextStyleState state((uint8)setting -> TextDefaultSize,false,false,false,true,0);

			RichTextEditorBar->SetCurrentStyleState(state);
		}
	}
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
