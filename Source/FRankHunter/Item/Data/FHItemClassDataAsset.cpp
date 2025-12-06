// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Data/FHItemClassDataAsset.h"
#include "GameplayEffect.h"
#include "AssetRegistry/AssetRegistryHelpers.h"
#include "Item/FHItemBase.h"
#include "Item/ItemTypes.h"
#include "Core/FHFRankHunterSettings.h"
#include "Misc/Optional.h"
#include "UObject/TopLevelAssetPath.h"
#include "Item/Data/FHItemSubsystem.h"
#include "Item/Actors/FHPickupItemActor.h"

DEFINE_LOG_CATEGORY(LogFHItemClassDataAsset);


TSoftObjectPtr<UTexture2D> UFHItemClassDataAsset::GetItemIcon(FName IconID) const
{
	return TSoftObjectPtr<UTexture2D>(AssetMap.FindRef(IconID).ToSoftObjectPath());
}

TSoftObjectPtr<UParticleSystem> UFHItemClassDataAsset::GetItemDropEffect(FName DropEffectID) const
{
	return TSoftObjectPtr<UParticleSystem>(AssetMap.FindRef(DropEffectID).ToSoftObjectPath());
}

TSoftObjectPtr<USoundBase> UFHItemClassDataAsset::GetItemDropSound(FName DropSoundID) const
{
	return TSoftObjectPtr<USoundBase>(AssetMap.FindRef(DropSoundID).ToSoftObjectPath());
}

TSoftObjectPtr<UStaticMesh> UFHItemClassDataAsset::GetItemMesh(FName MeshID) const
{
	return TSoftObjectPtr<UStaticMesh>(AssetMap.FindRef(MeshID).ToSoftObjectPath());
}

FText UFHItemClassDataAsset::GetItemName(const FString& ItemNameID) const
{ 
	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
	FName LocalizeName = ItemSettings->GetLocalizeStringTableID();

	return FText::FromStringTable(LocalizeName, ItemNameID);
}

FText UFHItemClassDataAsset::GetItemName(FName ItemNameID) const
{
	return GetItemName(ItemNameID.ToString());
}

FText UFHItemClassDataAsset::GetItemDescription(const FString& DescriptionID) const
{
	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
	FName LocalizeName = ItemSettings->GetLocalizeStringTableID();

	return FText::FromStringTable(LocalizeName, DescriptionID);
}

FText UFHItemClassDataAsset::GetItemDescription(FName DescriptionID) const
{
	return GetItemDescription(DescriptionID.ToString());
}

TSubclassOf<UGameplayEffect> UFHItemClassDataAsset::GetItemPassiveEffect(FName EffectID) const
{
	TSubclassOf<UGameplayEffect> PassiveEffectClass = AssetClassMap.FindRef(EffectID).Get();
	if (PassiveEffectClass)
	{
		return PassiveEffectClass;
	}
	else
	{
		return DefaultPassiveEffect;
	}
}

void UFHItemClassDataAsset::AutoSet()
{
	UDataTable* DataTable = GetDefault<UFHFRankHunterSettings>()->ItemDataTable.LoadSynchronous();
	if (!DataTable)
	{
		return;
	}

	// Init AssetClassMap
	{
		TArray<FProperty*> FHItemDataProperties =
		{
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, ItemID)),
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, PassiveEffect)),
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, CurrentHoldAbility)),
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, PrePareUseAbility)),
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, UseAbility)),
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, MeshID)),
		};
		TArray<FProperty*> FHThrowItemDataProperties =
		{
			 FFHThrowItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHThrowItemData, ImpactAbilityID))
		};
		TArray<FProperty*> FHDeployItemDataProperties =
		{
			FFHDeployItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHDeployItemData, DeployAbilityID))
		};
		UDataTable* ThrowableItemDataTable = GetDefault<UFHFRankHunterSettings>()->ThrowableItemDataTable.LoadSynchronous();
		UDataTable* DeployItemDataTable = GetDefault<UFHFRankHunterSettings>()->DeployItemDataTable.LoadSynchronous();

		TArray<FName> PropertyNames{};
		FindPropertyValues<FFHItemData>(DataTable, FHItemDataProperties, PropertyNames);
		FindPropertyValues<FFHThrowItemData>(ThrowableItemDataTable, FHThrowItemDataProperties, PropertyNames);
		FindPropertyValues<FFHDeployItemData>(DeployItemDataTable, FHDeployItemDataProperties, PropertyNames);

		AutoSet_Internal(PropertyNames, AssetClassMap);
	}

	// Init AssetMap
	{
		TArray<FProperty*> Properties =
		{
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, IconID)),
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, MeshID)),
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, PrePareUseAbility)),
			FFHItemData::StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FFHItemData, UseAbility)),
		};
		TArray<FName> PropertyNames{};
		FindPropertyValues<FFHItemData>(DataTable, Properties, PropertyNames);
		AutoSet_Internal(PropertyNames, AssetMap);
	}


	if (UFHItemSubsystem* ItemSubsystem = GEngine->GetEngineSubsystem<UFHItemSubsystem>(); ItemSubsystem)
	{
		ItemSubsystem->DataSetting();
	}
}

void UFHItemClassDataAsset::AutoSet_Internal(const TArray<FName>& Names, TMap<FName, TSoftObjectPtr<UObject>>& Map)
{
	FARFilter Filter;
	Filter.PackagePaths.Add("/Game");
	Filter.bRecursivePaths = true;

	TArray<FAssetData> OutAssetData;
	IAssetRegistry::Get()->GetAssets(Filter, OutAssetData);

	for (auto& AssetData : OutAssetData)
	{
		FName IconxtureName = AssetData.AssetName;
		for (auto& ID : Names)
		{
			if (IconxtureName == ID)
			{
				Map.FindOrAdd(FName(ID)) = AssetData.ToSoftObjectPath();
				UE_LOG(LogFHItemClassDataAsset, Log, TEXT("Found %s for ID %s"), *ID.ToString(), *AssetData.AssetName.ToString());

				break;
			}
		}
	}
}

void UFHItemClassDataAsset::AutoSet_Internal(const TArray<FName>& Names, TMap<FName, TSubclassOf<UObject>>& Map)
{
	FARFilter Filter;
	Filter.PackagePaths.Add("/Game");
	Filter.bRecursivePaths = true;

	TArray<FAssetData> OutAssetData;
	IAssetRegistry::Get()->GetAssets(Filter, OutAssetData);

	for (auto& AssetData : OutAssetData)
	{
		FString IconxtureName = AssetData.AssetName.ToString();
		for (auto& ID : Names)
		{
			if (IconxtureName == ID)
			{
				FName  GeneratedClassName;
				//{"GeneratedClass",L"/Script/Engine.BlueprintGeneratedClass'/Game/Main/Items/Blueprint/Pickaxe_Steel.Pickaxe_Steel_C'"}
				if (AssetData.GetTagValue(TEXT("GeneratedClass"), /*out*/ GeneratedClassName))
				{
					FString ClassPathString = GeneratedClassName.ToString();
					FSoftObjectPath SoftClassPath(ClassPathString);
					Map.FindOrAdd(ID) = Cast<UClass>(SoftClassPath.TryLoad());
				}

				UE_LOG(LogFHItemClassDataAsset, Log, TEXT("Found %s for ID %s"), *ID.ToString(), *AssetData.AssetName.ToString());

				break;
			}
		}
	}
}
