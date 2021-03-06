// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendRichTextBlock.h"

#include "ExtendRichTextBlockDecorator.h"
#include "ExtendRichTextMarshaller.h"
#include "ExtendRichTextStyle.h"
#include "Widgets/Text/SRichTextBlock.h"

TSharedRef<SWidget> UExtendRichTextBlock::RebuildWidget()
{
	UpdateStyleData();
	TArray<TSharedRef<class ITextDecorator>> Decorators;
	CreateDecorators(Decorators);

	TSharedRef<FExtendRichTextMarshaller> Marshaller = FExtendRichTextMarshaller::Create(Decorators, StyleInstance.Get());

	Marshaller->AppendInlineDecorator(FExtendRichTextDecorator::CreateIns());
	Marshaller->AppendInlineDecorator(FExtendImageDecorator::Create());
	
	MyRichTextBlock = SNew(SRichTextBlock)
		.TextStyle(FExtendRichTextEditBoxStyles::Get(), "DefaultTextStyle")
		.Marshaller(Marshaller);

	return MyRichTextBlock.ToSharedRef();
}
 