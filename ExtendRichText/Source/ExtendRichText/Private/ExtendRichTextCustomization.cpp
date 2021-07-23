#include "ExtendRichTextCustomization.h"
#include "SExtendRichTextEditableBox.h"
#include "DetailWidgetRow.h"
#include "ExtendRichText.h"

TSharedRef<IPropertyTypeCustomization> FExtendRichTextCustomization::MakeInstance()
{
	return MakeShareable(new FExtendRichTextCustomization);
}

void FExtendRichTextCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> TextProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FExtendRichText, Text));

	struct SlateLocal
	{
		static FText GetValueAsText(TSharedPtr<IPropertyHandle> InPropertyHandle)
		{
			FText Text;

			if (InPropertyHandle->GetValueAsFormattedText(Text) == FPropertyAccess::MultipleValues)
			{
				Text = NSLOCTEXT("PropertyEditor", "MultipleValues", "Multiple Values");
			}

			return Text;
		}
		
		static void OnTextCommitted(const FText& NewText, ETextCommit::Type /*CommitInfo*/, TSharedPtr<IPropertyHandle> InPropertyHandle)
		{
			InPropertyHandle->SetValueFromFormattedString(NewText.ToString());
		}

		static void OnTextChanged(const FText& NewText, TSharedPtr<IPropertyHandle> InPropertyHandle)
		{
			InPropertyHandle->SetValueFromFormattedString(NewText.ToString());
		}
	};
	
	HeaderRow
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
		//ContentProperty->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(300)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 2.0f)
		[
			SNew(SHorizontalBox)
			//.Visibility_Static(&Local::GetTextVisibility, TypeProperty)
			+ SHorizontalBox::Slot()
			[
				TextProperty->CreatePropertyValueWidget()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 2.0f)
		[
			SNew(SHorizontalBox)
			//.Visibility_Static(&Local::GetRichTextVisibility, TypeProperty)
			+ SHorizontalBox::Slot()
			[
				SNew(SExtendRichTextEditableBox)
				.Text_Static(&SlateLocal::GetValueAsText, TextProperty)
				.OnTextChanged(FOnTextChanged::CreateStatic(&SlateLocal::OnTextChanged,TextProperty))
				.OnTextCommitted(FOnTextCommitted::CreateStatic(&SlateLocal::OnTextCommitted,TextProperty))
			]
		]
	];
}

void FExtendRichTextCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{

	
}
