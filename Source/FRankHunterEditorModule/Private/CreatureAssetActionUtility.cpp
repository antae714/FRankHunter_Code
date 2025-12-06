// Copyright F Rank Hunter. All Rights Reserved.


#include "CreatureAssetActionUtility.h"
#include "EditorUtilityLibrary.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataAsset.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Engine/SCS_Node.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"

DECLARE_LOG_CATEGORY_CLASS(LogCreatureAssetActionUtility, Log, All)


UFHAssetActionUtility::UFHAssetActionUtility()
{
	bIsActionForBlueprints = true;
}

void UFHAssetActionUtility::SettingConstructionScriptData()
{
	TArray<UClass*> SelectedBPClass = UEditorUtilityLibrary::GetSelectedBlueprintClasses();
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	
	for (size_t i = 0; i < SelectedObjects.Num(); i++)
	{
		UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(SelectedBPClass[i]);
		UBlueprint* Blueprint = Cast<UBlueprint>(SelectedObjects[i]);
		


		UClass* SelectedClass = Blueprint->ParentClass;
		UObject* DefaultObj = SelectedClass->GetDefaultObject();

		FProperty* ConstructionDataProperty = SelectedClass->FindPropertyByName(TEXT("ConstructionData"));

		TMap<FName, UObject*> Datas = UFHBlueprintFunctionLibrary::GetConstructionScriptData(BPClass);
		// ConstructionDataProperty에 Datas세팅
		if (ConstructionDataProperty)
		{
			void* PropertyAddress = ConstructionDataProperty->ContainerPtrToValuePtr<void>(DefaultObj);
			FMapProperty* MapProp = CastField<FMapProperty>(ConstructionDataProperty);
			if (MapProp)
			{
				// 기존 데이터를 교체
				MapProp->CopyCompleteValue(PropertyAddress, &Datas);
			}

			// 변경된 CDO가 에디터에 반영되도록 MarkPackageDirty 호출 (저장 플래그)
			DefaultObj->MarkPackageDirty();
		}
	}
}

UCreatureAssetActionUtility::UCreatureAssetActionUtility()
{
	bIsActionForBlueprints = true;
}

void UCreatureAssetActionUtility::ExportToDataAsset(TSubclassOf<UDataAsset> DataAssetClass)
{
	TArray<FAssetData> SelectedAssetData = UEditorUtilityLibrary::GetSelectedAssetData();
	for (auto& AssetData : SelectedAssetData)
	{
		UObject* LoadedAsset = AssetData.GetAsset();
		UClass* LoadedAssetClass;
		if (!LoadedAsset)
		{
			UE_LOG(LogCreatureAssetActionUtility, Log, TEXT("%s is Load Failed"), *AssetData.GetFullName());
			continue;
		}
		if (UBlueprint* BlueprintAsset = Cast<UBlueprint>(LoadedAsset); BlueprintAsset)
		{
			LoadedAsset = BlueprintAsset->GeneratedClass->GetDefaultObject();
		}
		LoadedAssetClass = LoadedAsset->GetClass();


		FString PackagePath = AssetData.PackagePath.ToString();
		FString NewName = AssetData.AssetName.ToString() + TEXT("_Data");
		FString NewPath = PackagePath + TEXT("/") + NewName;

		UPackage* Package = CreatePackage(*NewPath);
		Package->FullyLoad();

		UObject* NewAsset = NewObject<UObject>(Package, DataAssetClass, *NewName, RF_Public | RF_Standalone);
		if (!NewAsset)
		{
			UE_LOG(LogCreatureAssetActionUtility, Log, TEXT("%s Load Failed"), *AssetData.GetFullName());
			continue;
		}


		for (TFieldIterator<FProperty> PropIt(LoadedAssetClass); PropIt; ++PropIt)
		{
			FProperty* SourceProp = *PropIt;
			FProperty* DestProp = NewAsset->GetClass()->FindPropertyByName(SourceProp->GetFName());
			if (DestProp && SourceProp->SameType(DestProp))
			{
				void* SrcPtr = SourceProp->ContainerPtrToValuePtr<void>(LoadedAsset);
				void* DstPtr = DestProp->ContainerPtrToValuePtr<void>(NewAsset);
				SourceProp->CopyCompleteValue(DstPtr, SrcPtr);
			}
		}

		FSavePackageArgs SavePackageArgs;
		SavePackageArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SavePackageArgs.Error = GLog;
		SavePackageArgs.SaveFlags = SAVE_Async;

		// Asset 저장
		FAssetRegistryModule::AssetCreated(NewAsset);
		NewAsset->MarkPackageDirty();
		FString PackageFileName = FPackageName::LongPackageNameToFilename(NewPath, FPackageName::GetAssetPackageExtension());
		UPackage::SavePackage(Package, NewAsset, *PackageFileName, SavePackageArgs);

		UE_LOG(LogCreatureAssetActionUtility, Log, TEXT("Exported %s -> %s"), *AssetData.AssetName.ToString(), *NewPath);
	}
}

void UCreatureAssetActionUtility::MigrateToDataAsset(UDataAsset* DataAsset)
{
	if (!DataAsset)
	{
		UE_LOG(LogCreatureAssetActionUtility, Warning, TEXT("MigrateToDataAsset: DataAsset is null"));
		return;
	}

	TSubclassOf<UDataAsset> DataAssetClass = DataAsset->GetClass();
	TArray<FAssetData> SelectedAssetData = UEditorUtilityLibrary::GetSelectedAssetData();
	if (SelectedAssetData.Num() != 1)
	{
		UE_LOG(LogCreatureAssetActionUtility, Warning, TEXT("MigrateToDataAsset is only One Selected"));
		return;
	}

	auto& AssetData = SelectedAssetData[0];
	UObject* LoadedAsset = AssetData.GetAsset();
	UClass* LoadedAssetClass;
	if (!LoadedAsset)
	{
		UE_LOG(LogCreatureAssetActionUtility, Warning, TEXT("AssetData is Not Load"));
		return;
	}

	if (UBlueprint* BlueprintAsset = Cast<UBlueprint>(LoadedAsset); BlueprintAsset)
	{
		LoadedAsset = BlueprintAsset->GeneratedClass->GetDefaultObject();
	}
	LoadedAssetClass = LoadedAsset->GetClass();

	for (TFieldIterator<FProperty> SrcPropIt(LoadedAssetClass); SrcPropIt; ++SrcPropIt)
	{
		FProperty* SrcProp = *SrcPropIt;
		FProperty* DstProp = DataAsset->GetClass()->FindPropertyByName(SrcProp->GetFName());

		if (DstProp && SrcProp->SameType(DstProp))
		{
			void* SrcPtr = SrcProp->ContainerPtrToValuePtr<void>(LoadedAsset);
			void* DstPtr = DstProp->ContainerPtrToValuePtr<void>(DataAsset);
			SrcProp->CopyCompleteValue(DstPtr, SrcPtr);
		}
	}

	// 변경사항 저장
	DataAsset->MarkPackageDirty();
	UE_LOG(LogTemp, Log, TEXT("Migrated %s -> %s"), *AssetData.AssetName.ToString(), *DataAsset->GetName());

	UPackage* Package = DataAsset->GetOutermost();

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.Error = GLog;
	SaveArgs.SaveFlags = SAVE_Async;

	FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
	UPackage::SavePackage(Package, DataAsset, *PackageFileName, SaveArgs);

}
