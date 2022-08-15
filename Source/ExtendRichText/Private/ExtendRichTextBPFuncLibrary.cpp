// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendRichTextBPFuncLibrary.h"
#include "ExtendRichTextStyle.h"

const FTextBlockStyle& UExtendRichTextBPFuncLibrary::GetDefaultTextStyle()
{
	return FExtendRichTextEditBoxStyles::Get().GetWidgetStyle<FTextBlockStyle>("DefaultTextStyle");
}

void UExtendRichTextBPFuncLibrary::SetText(FExtendRichText& richText, FText newText)
{
	richText = FExtendRichText(newText);
}
