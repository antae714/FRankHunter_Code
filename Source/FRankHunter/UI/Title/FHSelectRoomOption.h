// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "FHSelectRoomOption.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSelectRoomOption : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void FindSessionCompleted(bool Result);

	/*
		if session not found then active popup ui from blueprint.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void ErrorSessionNotFound();

	UFUNCTION(BlueprintCallable)
	void OnRefreshButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnJoinButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnCreateButtonClicked();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomMasterName;

protected:

	// ==== Has OnlineSubsystemNull ====
	TSharedRef<FOnlineSessionSearch> SearchOption = MakeShared<FOnlineSessionSearch>();
};
