// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include <steam/isteammatchmaking.h>
#include "FHAdvancedFriendsGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHAdvancedFriendsGameInstance : public UAdvancedFriendsGameInstance, public ISteamMatchmakingPingResponse
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual void ServerResponded(gameserveritem_t& server) override;
	virtual void ServerFailedToRespond() override;


	UFUNCTION(BlueprintCallable)
	void PingServerInfo(FString IpPort);

	UFUNCTION(BlueprintCallable)
	void PingResponse(bool result, FString steamID);


	void SessionCreateCompleted(FName SessionName, bool success);

	// ==== Has OnlineSubsystemNull ====
	TSharedRef<FOnlineSessionSearch> SearchOption = MakeShared<FOnlineSessionSearch>();

	UFUNCTION(BlueprintCallable)
	void FindSession();

	UFUNCTION(BlueprintCallable)
	void DebugSessionResults(const TArray<FBlueprintSessionResult>& Results);

	void FindSessionCompleted(bool Result);
	void JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void DestroySessionCompleted(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void ReturnToMenu();
	
	UFUNCTION(Exec, BlueprintImplementableEvent)
	void ActiveJoinSession();

	UFUNCTION(Exec, BlueprintImplementableEvent)
	void DeactiveJoinSession();

	UFUNCTION(Exec)
	void GetPlayerLicense(int32 PlayerID);

	UFUNCTION(Exec)
	void UpgradePlayerLicense(int32 PlayerID);
	UFUNCTION(Exec)
	void ResetPlayerLicense(int32 PlayerID);
	UFUNCTION(Exec)
	void ShowExplorationResultWidget();
	UFUNCTION(Exec)
	void HideExplorationResultWidget();
	UFUNCTION(Exec)
	void PlayExplorationResult();
	UFUNCTION(Exec)
	void GateEnd();
	UFUNCTION(Exec)
	void ShowRoundResult();
	UFUNCTION(Exec)
	void VoiceChatOn();
	UFUNCTION(Exec)
	void VoiceChatOff();
	UFUNCTION(Exec)
	void SetRange(float Value);
	UFUNCTION(Exec)
	void SetAttenuationRange(float Value);
	UFUNCTION(Exec)
	void DebugVoiceInterface();
	UFUNCTION(Exec)
	void ShowMouse(bool state);
	UFUNCTION(Exec)
	void DumpOnlineSession();
	UFUNCTION(Exec)
	void DumpVoiceChat();
	UFUNCTION(Exec)
	void RegistLocalTalker();
	UFUNCTION(Exec)
	void MuteOnOff(bool IsMute, bool IsSystemMute);

	/*
		Steam Cloud Test Functions.
	*/
	UFUNCTION(Exec)
	void WriteData();

	UFUNCTION(Exec)
	void ReadData();

	UFUNCTION(Exec)
	void DumpFile(const FString& FileName);

	UFUNCTION(Exec)
	void DeleteFile(const FString& FileName);

	UFUNCTION(Exec)
	void SaveShopList();
	UFUNCTION(Exec)
	void LoadShopList();


	void ReadFileComplete(bool bWasSuccessful, const FUniqueNetId& UserID, const FString& FileName);
	void WriteFileComplete(bool bWasSuccessful, const FUniqueNetId& UserID, const FString& FileName);

private:
	uint16 ServerPort = 0;

};

DECLARE_LOG_CATEGORY_EXTERN(LogPingServer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFHOnlineSession, Log, All);