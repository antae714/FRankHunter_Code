// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AssetRegistry/AssetRegistryHelpers.h"
#include "Item/FHItemBase.h"
#include "Item/ItemTypes.h"
#include "Core/FHFRankHunterSettings.h"
#include "Misc/Optional.h"
#include "UObject/TopLevelAssetPath.h"
#include "Abilities/GameplayAbility.h"
#include "FHItemClassDataAsset.generated.h"

class UFHItemBase;
class UParticleSystem;
class USoundBase;
class UStaticMesh;
class UGameplayEffect;
class UGameplayAbility;



DECLARE_LOG_CATEGORY_EXTERN(LogFHItemClassDataAsset, Log, All);

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemClassDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	TSubclassOf<UFHItemBase> GetItemClass(FName ItemID) const { return AssetClassMap.FindRef(ItemID).Get(); }

	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<UTexture2D> GetItemIcon(FName IconID) const;

	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<UParticleSystem> GetItemDropEffect(FName DropEffectID) const;

	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<USoundBase> GetItemDropSound(FName DropSoundID) const;

	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<UStaticMesh> GetItemMesh(FName MeshID) const;

	UFUNCTION(BlueprintPure)
	FText GetItemName(const FString& ItemNameID) const;
	FText GetItemName(FName ItemNameID) const;

	UFUNCTION(BlueprintPure)
	FText GetItemDescription(const FString& DescriptionID) const;
	FText GetItemDescription(FName DescriptionID) const;

	UFUNCTION(BlueprintPure)
	TSubclassOf<UGameplayEffect> GetItemEffect(FName EffectID) const { return AssetClassMap.FindRef(EffectID).Get(); }

	UFUNCTION(BlueprintPure)
	TSubclassOf<UGameplayEffect> GetItemPassiveEffect(FName EffectID) const;

	UFUNCTION(BlueprintPure)
	TSubclassOf<UGameplayAbility> GetItemAbility(FName AbilityID) const { return AssetClassMap.FindRef(AbilityID).Get(); }

private:

	UFUNCTION(CallInEditor, Category = "SettingsButton")
	void AutoSet();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path", meta = (AllowPrivateAccess = true))
	TMap<FName, TSoftObjectPtr<UObject>> AssetMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path", meta = (AllowPrivateAccess = true))
	TMap<FName, TSubclassOf<UObject>> AssetClassMap;


	UPROPERTY(EditAnywhere, Category = "Init", meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> DefaultPassiveEffect;



	void AutoSet_Internal(const TArray<FName>& Names, TMap<FName, TSoftObjectPtr<UObject>>& Map);
	void AutoSet_Internal(const TArray<FName>& Names, TMap<FName, TSubclassOf<UObject>>& Map);

	template<typename T>
	void FindPropertyValues(UDataTable* DataTable, TArray<FProperty*> Properties, TArray<FName>& PropertyNames);
};

template<typename T>
inline void UFHItemClassDataAsset::FindPropertyValues(UDataTable* DataTable, TArray<FProperty*> Properties, TArray<FName>& PropertyNames)
{
	if (DataTable)
	{
		DataTable->ForeachRow<T>(TEXT(""),
								 [this, &PropertyNames, Properties](const FName& RowName, const T& ItemData)
								 {
									 for (auto& Property : Properties)
									 {
										 FName PropertyOrigin;
										 Property->GetValue_InContainer(&ItemData, &PropertyOrigin);
										 if (PropertyOrigin.IsValid())
										 {
											 PropertyNames.Add(PropertyOrigin);
										 }
									 }
								 });

	}
	else
	{
		UE_LOG(LogFHItemClassDataAsset, Warning, TEXT("DataTable Is Null!"));
	}
}


