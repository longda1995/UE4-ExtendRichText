// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ExtendRichText : ModuleRules
{
	public ExtendRichText(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				//"InteractiveRichText/Private"
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "EditorStyle",
				"RenderCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "UnrealEd",
				"Projects",
				"Slate",
				"SlateCore",
				"InputCore",
				"UMG"
				// ... add private dependencies that you statically link with here ...	
			}
			);

#if UE_4_26_OR_LATER
        PrivateDependencyModuleNames.Add("DeveloperSettings");
#endif

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
