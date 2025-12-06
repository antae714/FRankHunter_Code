// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TitleController.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API ATitleController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATitleController();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MainWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> SubWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> EnterWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MessageBoxWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	FString NickName;

	UPROPERTY(BlueprintReadWrite)
	FString Password;



	UFUNCTION(BlueprintCallable)
	void OpenLobby();

	UFUNCTION(BlueprintCallable)
	void ChangeWidget(FName WidgetName);

	UFUNCTION(BlueprintCallable)
	void VisibleEnterWidget();

	UFUNCTION(BlueprintCallable)
	void EnterToServer(FString Ip);

	UFUNCTION(BlueprintCallable)
	void ShowGateTooltip(const FString& GateName);

	UFUNCTION(BlueprintCallable)
	void HideGateTooltip();

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetMessageBoxWidget();


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PopUpMessageBoxByKey(FName MessageKey);

	UPROPERTY()
	TObjectPtr<UUserWidget> MainTitleWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> SubTitleWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> EnterServerWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> MessageBoxWidget;
	
	TSharedPtr<class FOnlineSessionSearch> SearchResult;

	void JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	//void SessionFindCompleted(bool bWasSuccessful);
	//void JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	//void FriendSessionFindCompleted(int32 LocalPlayerNum, bool success, const TArray<FOnlineSessionSearchResult>& Search);

	//void InviteReceive(const FUniqueNetId& receiverId, const FUniqueNetId& senderId, const FString& appId, const FOnlineSessionSearchResult& InviteResult);
	//void InviteAccept(const bool success, const int32 controllerId, FUniqueNetIdPtr userId, const FOnlineSessionSearchResult& InviteResult);

	//void PrintMessage(FString str);

	//void SessionCreated(FName SessionName, bool success);
};

DECLARE_LOG_CATEGORY_EXTERN(LogTitle, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSessionTest, Log, All);