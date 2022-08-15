#pragma once
#include "ExtendRichTextSettings.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"


const FVector2D icon16x16(16.f, 16.f);
const FVector2D icon32x32(32.f, 32.f);
const FVector2D small10x5(10.f, 5.f);
const FVector2D icon8x8(8.f, 8.f);
const FVector2D icon12x12(12.f, 12.f);
class FExtendRichTextEditBoxStyles final :public FSlateStyleSet
{

public:
	explicit FExtendRichTextEditBoxStyles(const FName& InStyleSetName)
		: FSlateStyleSet(InStyleSetName)
	{
		const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("ExtendRichText");
		check(Plugin.IsValid());

		SetContentRoot(Plugin->GetContentDir());
		SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
		const auto setting = GetDefault<UExtendRichTextSettings>();
		
		Set("BoldBtnStyle", MakeBtnStyleByImg("Texture/Bold.png", icon16x16));
		Set("ItalicBtnStyle", MakeBtnStyleByImg("Texture/Italic.png", icon16x16));
		Set("UnderLineBtnStyle", MakeBtnStyleByImg("Texture/UnderLine.png", icon16x16));
		Set("ColorBtnStyle", MakeBtnStyleByImg("Texture/Font.png", icon16x16));
		Set("PainterBtnStyle", MakeBtnStyleByImg("Texture/Painter.png", icon16x16));
		Set("EraserBtnStyle", MakeBtnStyleByImg("Texture/Eraser.png", icon16x16));
		Set("FontSizeBrush", new FSlateImageBrush(RootToContentDir("Texture/FontSize.png"), icon16x16));
		Set("EmptyBrush", new FSlateImageBrush("", small10x5));
		Set("InsertImgBtnStyle", MakeBtnStyleByImg("Texture/Img.png", icon16x16));
		Set("InsertHyperLinkBtn", MakeBtnStyleByImg("Texture/HyperLink.png", icon16x16));
		const FTextBlockStyle defaultTextStyle = FTextBlockStyle()
			.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", setting->TextDefaultSize))
			.SetColorAndOpacity(FSlateColor::UseForeground())
			.SetShadowOffset(FVector2D::ZeroVector)
			.SetShadowColorAndOpacity(FLinearColor::Black)
			.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f));
		
		Set("BlackBtnStyle", FButtonStyle()
			.SetNormal(FSlateImageBrush(FName(),icon8x8,FLinearColor(0,0,0,0.5f),ESlateBrushTileType::NoTile,ESlateBrushImageType::FullColor))
			.SetHovered(FSlateImageBrush(FName(), icon8x8, FLinearColor(0.7f, 0.2f, 0.02f, 0.5f), ESlateBrushTileType::NoTile, ESlateBrushImageType::FullColor))
			.SetPressed(FSlateImageBrush(FName(), icon8x8, FLinearColor(0, 0, 0, 0.3f), ESlateBrushTileType::NoTile, ESlateBrushImageType::FullColor))
			.SetDisabled(FSlateImageBrush(FName(), icon8x8, FLinearColor(0, 0, 0, 0.2f), ESlateBrushTileType::NoTile, ESlateBrushImageType::FullColor))
			.SetNormalPadding(FMargin(0, 0, 0, 1))
			.SetPressedPadding(FMargin(0, 1, 0, 0)));

		Set("FontSizeBtn",FTextBlockStyle()
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular",8))
		.SetColorAndOpacity(FLinearColor::White));
		
		Set("FontSizeExpandBtn",MakeBtnStyleByImg("Texture/Expand.png",icon12x12));
		
		Set("DefaultTextStyle", defaultTextStyle);
		
		const FSlateFontInfo SizeTextFont = FCoreStyle::GetDefaultFontStyle("Regular", 6);
		Set("SizeFont", SizeTextFont);

		Set("UnderLineBrush", new FSlateImageBrush(RootToContentDir("Texture/HyperlinkUnderline.png"),icon8x8, FLinearColor::White, ESlateBrushTileType::Vertical));
		const FButtonStyle underLineStyle = FButtonStyle()
			.SetNormal(FSlateBorderBrush(RootToContentDir("Texture/HyperlinkDotted.png"), FMargin(0, 0, 0, 3 / 16.f)))
			.SetPressed(FSlateNoResource())
			.SetHovered(FSlateBorderBrush(RootToContentDir("Texture/HyperlinkUnderline.png"), FMargin(0, 0, 0, 3 / 16.f)));

		Set("HyperLinkUnderLine", underLineStyle);

		const FHyperlinkStyle defaultHyperLinkStyle = FHyperlinkStyle()
			.SetPadding(FMargin(1,1,1,1))
			.SetTextStyle(defaultTextStyle).SetUnderlineStyle(underLineStyle);
		Set("DefaultHyperStyle", defaultHyperLinkStyle);

		FSlateStyleRegistry::RegisterSlateStyle(*this);
	}

	virtual ~FExtendRichTextEditBoxStyles() override
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*this);
	}

	static FExtendRichTextEditBoxStyles& Get()
	{
		static FExtendRichTextEditBoxStyles ins("InteractiveRichText");
		return ins;
	}
private:

	const FButtonStyle MakeBtnStyleByImg(FString imgName,const FVector2D& size)
	{
		return FButtonStyle()
			.SetNormal(FSlateImageBrush(RootToContentDir(imgName), size, FLinearColor(1, 1, 1, 0.5f)))
			.SetHovered(FSlateImageBrush(RootToContentDir(imgName), size, FLinearColor(1, 1, 1, 1)))
			.SetPressed(FSlateImageBrush(RootToContentDir(imgName), size, FLinearColor(1, 1, 1, 0.4f)))
			.SetDisabled(FSlateImageBrush(RootToContentDir(imgName), size, FLinearColor(0, 0, 0, 0.3f)))
			.SetNormalPadding(FMargin(0, 0, 0, 1))
			.SetPressedPadding(FMargin(0, 1, 0, 0));
	}
};