// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FHBlueprintFunctionLibrary.generated.h"

struct FFHItemData;
struct FItemRarityColorArray;
class UFHInventoryComponent;
class UFHItemClassDataAsset;
enum class EItemRarity :uint8;
class AFHPropManager;
class AFHUIManager;
class UFHLevelAsset;
class UFHItemBase;
class UFHSkillDataAsset;
class USoundWaveProcedural;
class UAudioComponent;
class UFHSelectInfoIconDataAsset;
class ACreatureAIController;
struct FGameplayEffectSpec;
class UFHGateThemaAsset;

USTRUCT(BlueprintType)
struct FSpawnItemParam
{
	GENERATED_BODY()

	FSpawnItemParam();

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<class AFHWorldItemActor> ItemActorClass;

	UPROPERTY(BlueprintReadWrite)
	int32 index;

	UPROPERTY(BlueprintReadWrite)
	FVector SpawnLocation;
};


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static inline const FName Name_ExcludeFromSave = TEXT("ExcludeFromSave");

public:
	// Begin ItemFunctions

	UFUNCTION(BlueprintPure, Category = Inventory, Meta = (DefaultToSelf = "Actor"))
	static UFHInventoryComponent* GetInventoryComponent(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = Inventory, Meta = (DefaultToSelf = "Actor"))
	static void SpawnItemToActor(AActor* AvatarActor, FSpawnItemParam Param);

	UFUNCTION(BlueprintPure, Category = ItemInfo)
	static FFHItemData GetItemData(FName ItemID);

	UFUNCTION(BlueprintPure, Category = ItemInfo)
	static FFHItemData GetItemDataFromClass(TSubclassOf<UFHItemBase> ItemClass);

	UFUNCTION(BlueprintPure, Category = ItemInfo)
	static FLinearColor GetItemRarityColor(EItemRarity ItemRarity);

	UFUNCTION(BlueprintPure, Category = ItemInfo)
	static UFHItemClassDataAsset* GetItemClassDataAsset();



	// ~End ItemFunctions

	// Begin Utility

    UFUNCTION(BlueprintPure, Category = "Utility")
	static int32 GetRandIndexUsingDiscreteDistribution(const TArray<float>& Weights);
 
	UFUNCTION(BlueprintCallable, Category = "Utility")
    static TArray<FKey> FindMatchingKeysInInputMappingContext(const class UInputMappingContext* InputMappingContext, const class UInputAction* InputAction);

	UFUNCTION(BlueprintPure, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static FString GetMapName(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static bool IsInState(AController* Controller, FName InStateName);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static FName GetSpectatorName();

	UFUNCTION(BlueprintPure, Category = "Utility")
	static FName GetPlayerName();

	UFUNCTION(BlueprintPure, Category = "Utility")
	static bool IsEnenmy(AActor* TeamA, AActor* TeamB);

	UFUNCTION(BlueprintPure, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static bool IsGameOver(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static ACreatureAIController* GetCreatureAIController(AActor* ControlledActor);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static int32 GetCurrentYear(int32 LoopCount, int32 offsetYear = 2025);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static int32 GetCurrentMounth(int32 LoopCount);

	static FText GetLocalizeText(const FTextKey& InKey);


	// ~End Utility
	


	// Begin ManagerFunctions

	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static AFHPropManager* GetPropManager(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static AFHUIManager* GetUIManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static class AFHSoundManagerActor* GetSoundManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static class AFHVignetteEffectManager* GetVignetteEffectManager(const UObject* WorldContextObject);

	// ~End ManagerFunctions


	// Begin SaveGameFunctions

	UFUNCTION(BlueprintPure, Category = "Utility", BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"))
	static FString GetGameID(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Utility", BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"))
	static FString GetGameName(const UObject* WorldContextObject);

	// 네이밍 이건 아닌거같은데 일단 사용
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static void SaveActors(const UObject* WorldContextObject, const FString& SlotName, const TArray<TSubclassOf<AActor>>& FilterActor);

	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (WorldContext = "WorldContextObject"))
	static void LoadActors(const UObject* WorldContextObject, const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void SaveObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void LoadObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void RemoveGame(const FString& GameID);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void RemoveSaveGame(const FString& GameID, const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static UFHLevelAsset* GetLevelAsset();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void SaveData(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void LoadData(UObject* Object);

	// ~End SaveGameFunctions

	// Begin GateFunctions

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,  Category = "InGate")
	static void EscapeGate(AController* Controller);

	UFUNCTION(BlueprintCallable, Category = "InGate", meta = (WorldContext = "WorldContextObject"))
	static bool IsGate(const UObject* WorldContextObject);

	// ~End GateFunctions




	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
	static FString GetEncrptData(FString Input);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
	static FString ToUnicodeString(const FString& InString);


	// VOIP Utils
	UFUNCTION(BlueprintCallable, Category = "VoiceChat")
	static void ClearVoicePackets(UObject* WorldContextObject);

	// Sound Utils
	UFUNCTION(BlueprintCallable, Category = "Sound")
	static void CopyAndSetSound(UObject* WorldContectObject, USoundBase* Base, UAudioComponent* TargetAudioComponent);

	// widget utils
	UFUNCTION(BlueprintCallable, Category = "Select")
	static UFHSelectInfoIconDataAsset* GetSelectInfoDataAsset();


	UFUNCTION(BlueprintPure, Category = "Utility")
	static AActor* GetInstigatorFromEffectSpec(const FGameplayEffectSpec& Spec);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static TMap<FName, UObject*> GetConstructionScriptData(UClass* AssetClass);


	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void ReferenceNullPtr();

	UFUNCTION(BlueprintCallable, Category = "ThemaAsset")
	static UFHGateThemaAsset* GetGateThemaAsset();
};




