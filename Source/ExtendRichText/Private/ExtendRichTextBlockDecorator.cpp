// Fill out your copyright notice in the Description page of Project Settings.
#include "ExtendRichTextBlockDecorator.h"

#include "ExtendHyperEventHandler.h"
#include "ExtendRichTextStyle.h"
#include "ExtendRichTextSettings.h"
#include "ExtendTextRun.h"
#include "SExtendRichTextEditableBox.h"
#include "Components/RichTextBlock.h"
#include "Framework/Text/SlateImageRun.h"
#include "Framework/Text/SlateTextRun.h"
#include "Widgets/Text/SRichTextBlock.h"

FExtendRichTextDecorator::~FExtendRichTextDecorator()
{
}

bool FExtendRichTextDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	auto setting = GetDefault<UExtendRichTextSettings>();
	if(!RunParseResult.Name.IsEmpty() && RunParseResult.Name.Equals(setting->ExtendTagName))
	{
		return true;
	}
	return false;
}

TSharedRef<FExtendRichTextDecorator> FExtendRichTextDecorator::CreateIns(SExtendRichTextEditableBox* EditableBox,UExtendRichTextBlockDecorator* decorator,URichTextBlock* richTextBlock)
{
	return MakeShareable(new FExtendRichTextDecorator());
}

FEditableRichTextDecorator::FEditableRichTextDecorator()
{
	auto handlerClass = GetDefault<UExtendRichTextSettings>()->HandlerClass;
	if(IsValid(handlerClass))
	{
		HyperEventHandler = GetDefault<UExtendHyperEventHandler>(handlerClass);
	}
	else
	{
		HyperEventHandler = GetDefault<UExtendHyperEventHandler>();
	}
	
}

TSharedRef<ISlateRun> FEditableRichTextDecorator::Create(const TSharedRef<FTextLayout>& TextLayout,
                                                         const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText,
                                                         const ISlateStyle* Style)
{
	FRunInfo RunInfo;
	RunInfo.Name = RunParseResult.Name;
	for (const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
	{
		RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
	}

	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	*InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
	ModelRange.EndIndex = InOutModelText->Len();

	//�����л���state
	if (RunInfo.MetaData.Contains(TEXT("Style")))
	{
		FString styleStr = RunInfo.MetaData[TEXT("Style")];

		uint32 num;
		LexFromString(num, *styleStr);

		const FRichTextStyleState state = FRichTextStyleState::DeserializeFromUint32(num);

		if (state.IsText)
		{
			if(!FExtendRichTextEditBoxStyles::Get().HasWidgetStyle<FTextBlockStyle>(FName(styleStr)))
			{
				FExtendRichTextEditBoxStyles::Get().Set(FName(styleStr), FExtendRichTextStyleMaker::MakeTextStyleByState(state));
			}
			auto run = FRunCreator::CreateTextRun(RunInfo, InOutModelText,
				FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FTextBlockStyle>(FName(styleStr)),ModelRange);
			run->ApplyFontSizeMultiplierOnTextStyle(1);
			return run;
		}
		else
		{
			if(!FExtendRichTextEditBoxStyles::Get().HasWidgetStyle<FHyperlinkStyle>(FName(styleStr)))
			{
				FExtendRichTextEditBoxStyles::Get().Set(FName(styleStr), *FExtendRichTextStyleMaker::MakeStyleByState(state));
			}
			return FRunCreator::CreateHyperRun(RunInfo,InOutModelText,
				FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FHyperlinkStyle>(FName(styleStr)), ModelRange,
	FSlateHyperlinkRun::FOnClick::CreateUObject(HyperEventHandler,&UExtendHyperEventHandler::HandleOnClick_Implementation),NULL,NULL);
		}
	}
	return FSlateTextRun::Create(RunInfo, InOutModelText, Style->GetWidgetStyle<FTextBlockStyle>("DefaultTextStyle"),ModelRange);
}

bool FEditableRichTextDecorator::Supports(const FTextRunParseResults& RunInfo, const FString& Text) const
{
	return false;
}

TSharedRef<FExtendImageDecorator> FExtendImageDecorator::Create()
{
	return MakeShareable(new FExtendImageDecorator());
}

bool FExtendImageDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	return (RunParseResult.Name == TEXT("img"));
}

TSharedRef<ISlateRun> FExtendImageDecorator::Create(const TSharedRef<FTextLayout>& TextLayout,
	const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText,
	const ISlateStyle* Style)
{
	const FTextRange* const BrushNameRange = RunParseResult.MetaData.Find(TEXT("src"));

	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	*InOutModelText += TEXT('\x200B'); // Zero-Width Breaking Space
	ModelRange.EndIndex = InOutModelText->Len();

	FRunInfo RunInfo(RunParseResult.Name);
	for (const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
	{
		RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
	}

	if (BrushNameRange != NULL)
	{
		const FString BrushNameString = OriginalText.Mid(BrushNameRange->BeginIndex, BrushNameRange->EndIndex - BrushNameRange->BeginIndex);

		return FSlateImageRun::Create(RunInfo, InOutModelText, *GetPathToImage(BrushNameString), 0, ModelRange);
	}

	const FInlineTextImageStyle& ImageStyle = FInlineTextImageStyle::GetDefault();
	return FSlateImageRun::Create(RunInfo, InOutModelText, &ImageStyle.Image, ImageStyle.Baseline, ModelRange);
}

FString FExtendImageDecorator::GetPathToImage(const FString& InSrcMetaData)
{
	// images are content-relative if they are created from a project
	FString FullPath;
	if (FPackageName::IsValidLongPackageName(InSrcMetaData))
	{
		FullPath = FPackageName::LongPackageNameToFilename(InSrcMetaData);
		if (FPaths::GetExtension(FullPath).Len() == 0)
		{
			FullPath += TEXT(".png");
		}
	}
	else
	{
		// otherwise they are raw paths relative to the engine
		FullPath = InSrcMetaData;
		FPaths::MakePathRelativeTo(FullPath, FPlatformProcess::BaseDir());
	}

	return FullPath;
}

FExtendImageDecorator::FExtendImageDecorator()
{
}

UExtendRichTextBlockDecorator::UExtendRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> UExtendRichTextBlockDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return FExtendRichTextDecorator::CreateIns(nullptr,this,InOwner);
}
