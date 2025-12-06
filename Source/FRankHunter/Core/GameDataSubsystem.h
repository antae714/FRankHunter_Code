// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSavable.h"
#include "Core/GameSaveData.h"
#include "GameDataSubsystem.generated.h"


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UGameDataSubsystem();

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FString GetGameID() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FString GetGameName() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetGameName(FString NewGameID);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	TArray<FString> LoadGameIDs();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	TArray<FString> LoadGameNames();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FString GetOwnerNickName() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FString GetPassword() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool IsLAN() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool IsPublic() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool IsFriendOnly() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool IsMorning() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetOwnerNickName(const FString& NewOwnerNickName);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetPassword(const FString& NewPassword);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetIsLAN(bool bNewIsLAN);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetIsPublic(bool bNewIsPublic);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetIsFriendOnly(bool bNewIsFriendOnly);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetIsMorning(bool bNewIsMorning);

	FGameSaveData& GetGameSaveData() { return GameSaveData; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void InitializeGameSaveData(FString SaveGameID = TEXT(""));

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FGameSaveData LoadGameSaveData(FString SaveGameID);

protected:
	FString GameID;



	UPROPERTY()
	FGameSaveData GameSaveData;
};



UCLASS()
class FRANKHUNTER_API UGameSaveDataForLoad : public UObject, public IGameSavable
{
	GENERATED_BODY()

public:
	// Begin IGameSavable Implementation
	virtual FString GetSaveSlot() const override { return TEXT("GameState"); }
	virtual bool IsGlobal() const override { return true; }
	virtual void SerializeData(FArchive& Ar) override;
	// ~End IGameSavable Implementation

	FGameSaveData* GameSaveData;
};