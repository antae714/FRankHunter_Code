#include "FRankHunterEditorModule.h"
#include "Modules/ModuleManager.h"
#include "FHPropertyCustomization.h"
#include "DungeonGeneration/FHSpawningPointDescriptor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "LevelEditor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "FRankHunterEditorModule"

void FFRankHunterEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomPropertyTypeLayout(FSpawnableActorDescriptor::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSpawnableActorDescriptorCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();

	if (IsRunningCommandlet())
	{
		return;
	}

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);

	MenuExtender->AddMenuBarExtension(
		"Help",  // 기존 메뉴 뒤에 붙이기 (File, Edit, Help 등)
		EExtensionHook::After,
		nullptr,
		FMenuBarExtensionDelegate::CreateRaw(this, &FFRankHunterEditorModule::AddTopLevelMenu)
	);

	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FFRankHunterEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FSpawnableActorDescriptor::StaticStruct()->GetFName());
	}
}

void FFRankHunterEditorModule::AddTopLevelMenu(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("MyMenuLabel", "FRankHunter"),
		LOCTEXT("MyMenuTooltip", "FRankHunter 관련 툴"),
		FNewMenuDelegate::CreateRaw(this, &FFRankHunterEditorModule::AddMenuEntry),
		"FRankHunter"
	);
}

void FFRankHunterEditorModule::AddMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(TEXT("SaveSection"), LOCTEXT("SaveSectionHeading", "세이브."));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("DeleteEditorSave", "에디터 세이브파일 삭제"),
		LOCTEXT("DeleteEditorSaveTooltip", "Saved/SaveGames/0 파일을 삭제합니다."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FFRankHunterEditorModule::DeleteEditorSaveGameFile))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("DeleteAllSaves", "전체 세이브파일 삭제"),
		LOCTEXT("DeleteAllSavesTooltip", "Saved/SaveGames의 모든 파일을 삭제합니다."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FFRankHunterEditorModule::DeleteAllFilesInSaveDirectory))
	);

	MenuBuilder.EndSection();
}

void FFRankHunterEditorModule::DeleteEditorSaveGameFile()
{
	UFHBlueprintFunctionLibrary::RemoveGame(TEXT("30"));
}

void FFRankHunterEditorModule::DeleteAllFilesInSaveDirectory()
{
	FString SaveDir = FPaths::ProjectSavedDir() / TEXT("SaveGames") / TEXT("");

	TArray<FString> Files;
	IFileManager::Get().FindFiles(Files, *SaveDir);

	for (const FString& File : Files)
	{
		FString FullPath = FPaths::Combine(SaveDir, File);
		if (IFileManager::Get().Delete(*FullPath, false, true))
		{
			UE_LOG(LogTemp, Log, TEXT("삭제됨: %s"), *FullPath);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("삭제 실패: %s"), *FullPath);
		}
	}
}



#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFRankHunterEditorModule, FRankHunterEditorModule);