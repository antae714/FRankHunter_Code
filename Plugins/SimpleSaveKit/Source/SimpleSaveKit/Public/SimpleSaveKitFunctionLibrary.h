// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/Optional.h"
#include "SimpleSaveKitFunctionLibrary.generated.h"

class USaveGame;

/**
 * 
 */
UCLASS()
class SIMPLESAVEKIT_API USimpleSaveKitFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "SaveKit", meta = (WorldContext = "WorldContextObject"))
	static void SaveGameObjects(const UObject* WorldContextObject, const FString& GameID, const FString& SlotName, const TArray<UObject*>& ObjectArray, bool bIsSpawn);

	UFUNCTION(BlueprintCallable, Category = "SaveKit", meta = (WorldContext = "WorldContextObject"))
	static void LoadGameObjects(const UObject* WorldContextObject, const FString& GameID, const FString& SlotName, const TArray<UObject*>& ObjectArray, bool bIsSpawn);

	UFUNCTION(BlueprintCallable, Category = "SaveKit")
	static void SaveGameFromObject(const FString& GameID, UObject* Object, bool bIsSpawn);
	static USaveGame* GetSaveGameFromObject(const FString& GameID, UObject* Object, bool bIsSpawn);

	UFUNCTION(BlueprintCallable, Category = "SaveKit")
	static void LoadGameFromObject(const FString& GameID, UObject* Object, bool bIsSpawn);

	UFUNCTION(BlueprintCallable, Category = "SaveKit")
	static void LoadGameFromObjectToSlot(const FString& SlotName, UObject* Object, bool bIsSpawn);

	UFUNCTION(BlueprintCallable, Category = "SaveKit")
	static bool IsExistSaveFile(const FString& GameID, UObject* object);


	UFUNCTION(BlueprintCallable, Category = "SaveKit")
	static void SaveGameData(UObject* WorldContextObject, const FString& GameID);
	UFUNCTION(BlueprintCallable, Category = "SaveKit")
	static void LoadGameData(UObject* WorldContextObject, const FString& GameID);

	static void SerializeActor(FArchive& Ar, UObject* object);
	static FString GetSlotName(const FString& GameID, UObject* object);
	static FString GetGameID(const FString& GameName);
private:
	static void GetSavableActorArray(const UObject* WorldContextObject, TArray<UObject*>& OutActorArray);
	static void GetSavableActorComponentArray(const UObject* WorldContextObject, TArray<UObject*>& ActorComponentArray);
	
	
};
