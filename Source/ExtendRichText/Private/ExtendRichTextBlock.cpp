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

	const TSharedRef<FExtendRichTextMarshaller> Marshaller = FExtendRichTextMarshaller::Create(Decorators, StyleInstance.Get());
	const TSharedRef<FExtendRichTextDecorator> TextDeco = FExtendRichTextDecorator::CreateIns(this);

	Marshaller->AppendInlineDecorator(TextDeco);
	Marshaller->AppendInlineDecorator(FExtendImageDecorator::Create());
	
	MyRichTextBlock = SNew(SRichTextBlock)
		.TextStyle(FExtendRichTextEditBoxStyles::Get(), "DefaultTextStyle")
		.Marshaller(Marshaller);

	return MyRichTextBlock.ToSharedRef();
}

void UExtendRichTextBlock::OnHyperClick_Implementation(const FHyperMeta& Meta)const
{
	OnHyperLinkClickFunc.Broadcast(Meta);
}
 