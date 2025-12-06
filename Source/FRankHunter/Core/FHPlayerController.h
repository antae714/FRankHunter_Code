// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Common/FHFadeInOut.h"
#include "FHPlayerController.generated.h"

class UFHStatusHudBase;
class UFHStatusBase;
class UUserWidget;
class UFHItemQuickSlot;
class UFH_W_GateEnterRequest;
class UFH_W_SelectGate;
class UFHLoading;

class UCommonActivatableWidget;
class DBNetConnection;
class UCommonActivatableWidget;

class AFHUIManager;

enum class EPurchaseError : uint8;
enum class ESellItemError : uint8;

struct FCachedSellItemInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, FName, NewStateName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);



/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFHPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Pawn() override;
	virtual void ChangeState(FName NewState) override;
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;
	virtual void PawnLeavingGame() override;
	void SuperPawnLeavingGame();

	/** For GameMenu */
	virtual bool InputKey(const FInputKeyEventArgs& Params) override;
	/** */

public:
	UPROPERTY(BlueprintAssignable)
	FOnStateChanged OnStateChanged;
	void CreateLoadingUI();

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerStateChanged;

public:
	void OnPlayerStateChangedFunction();

	UFUNCTION(Client, Reliable)
	void ClientRoomLoadStart();
	UFUNCTION(Client, Reliable)
	void ClientRoomLoadEnd();

	UFUNCTION(BlueprintCallable)
	void TurnOnPhysics();
	UFUNCTION(BlueprintCallable)
	void TurnOffPhysics();

	UFUNCTION(Client, Reliable)
	void StopMinimapUpdate();
	void StopMinimapUpdate_Implementation();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UFHLoading> LoadingUIClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UI")
	UFHLoading* LoadingUIInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UCommonActivatableWidget> StatusUIClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UCommonActivatableWidget> StatusUIClassInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UFHStatusHudBase> StatusHudClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UFHStatusHudBase> StatusHudInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bIsStatusUIOpen : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMenu")
	TSubclassOf<UCommonActivatableWidget> GameMenuWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UCommonActivatableWidget> GameMenuWidgetInstance;

	// TODO: client RPC 사용하지 않고, 로컬 Interact로 ui를 열도록 수정해야함.
	// Move to AOpenGateSelect::Interact_Impl  Last. 2025.07.22 @ 권세민
	//UFUNCTION(Client, Reliable)
	//void OpenGateSelectUI();
	//void OpenGateSelectUI_Implementation();
	// =================================================================

	UFUNCTION(NetMulticast, Reliable)
	void SeeGateEnd();
	void SeeGateEnd_Implementation();

	UFUNCTION(Client, Reliable)
	void CheckFirstPlay();
	void CheckFirstPlay_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnPreviewCameraAndSetView();
	void SpawnPreviewCameraAndSetView_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectCharacterCameraPosition")
	FVector DefaultCameraPosition;

	UFUNCTION(Client, Reliable)
	void StartVoiceChat();
	void StartVoiceChat_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewCamera")
	TSubclassOf<AActor> PreviewCameraClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PreviewCamera")
	TObjectPtr<AActor> PreviewCameraInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GateSelect")
	bool bIsOtherWidgetOpened = false;
	
	UFUNCTION(BlueprintCallable)
	void TurnOffStatusHud();
	UFUNCTION(BlueprintCallable)
	void TurnOnStatusHud();

	UFUNCTION(Client, Reliable)
	void Client_SetStatusHudVisibility(ESlateVisibility NewVisibility);
	void Client_SetStatusHudVisibility_Implementation(ESlateVisibility NewVisibility);

	// =======================================
	// ======= Client Environment ============
	// =======================================

	bool bIsAutoRefuseRequest = true;
	UFUNCTION(Client, Reliable)
	void SetAutoRefuse(bool State);
	void SetAutoRefuse_Implementation(bool State);

	UFUNCTION(Client, Reliable)
	void ShowQuota(int32 LoopCount);
	void ShowQuota_Implementation(int32 LoopCount);

	UFUNCTION()
	void Callback_ShowQuota();

	UFUNCTION(Client, Reliable)
	void FadeIn();
	void FadeIn_Implementation();

	UFUNCTION(Client, Reliable)
	void FadeOut();
	void FadeOut_Implementation();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void OpenKiosk();
	void OpenKiosk_Implementation();

	UFUNCTION(Client, Reliable)
	void OpenPaymentUI();
	void OpenPaymentUI_Implementation();

	UFUNCTION(BlueprintCallable)
	void OnEnterButtonPressed(const FString& GateName, const EGateRank& GateRank);

	UFUNCTION(Client, Reliable)
	void OpenEnterRequestUI(const FString& GateName);
	void OpenEnterRequestUI_Implementation(const FString& GateName);

	UFUNCTION(Client, Reliable)
	void OpenEnterErrorUI();
	void OpenEnterErrorUI_Implementation();

	UFUNCTION(Client, Reliable)
	void ResetGateButton(const TArray<uint8>& SelectedGateInfo);
	void ResetGateButton_Implementation(const TArray<uint8>& SelectedGateInfo);

	UFUNCTION(Client, Reliable)
	void OnPlayerRefuse(const FString& PlayerName);
	void OnPlayerRefuse_Implementation(const FString& PlayerName);

	UFUNCTION(Client, Reliable)
	void CloseEnterRequestUI();
	void CloseEnterRequestUI_Implementation();

	UFUNCTION(Client, Reliable)
	void PurchaseError(const EPurchaseError& Result);
	void PurchaseError_Implementation(const EPurchaseError& Result);

	UFUNCTION(Client, Reliable)
	void AddToSellingCartError(const ESellItemError& Result);
	void AddToSellingCartError_Implementation(const ESellItemError& Result);

	UFUNCTION(Client, Reliable)
	void ItemMoveToCartError(const ESellItemError& Result);
	void ItemMoveToCartError_Implementation(const ESellItemError& Result);

	UFUNCTION(Client, Reliable)
	void ItemSellError(const ESellItemError& Result);
	void ItemSellError_Implementation(const ESellItemError& Result);

	UFUNCTION(Client, Reliable)
	void OpenRestRequest();
	void OpenRestRequest_Implementation();

	UFUNCTION(Client, Reliable)
	void CloseRestRequest();
	void CloseRestRequest_Implementation();
	// =======================================

	// =======================================
	// ======= Server Environment ============
	// =======================================

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnPay(int32 Value);
	void OnPay_Implementation(int32 Value);

	UFUNCTION(Server, Reliable)
	void NotifyReady();
	void NotifyReady_Implementation();

	UFUNCTION(Server, Reliable)
	void AddToSellItemCart(const struct FSellItemInfo& ItemInfo);
	void AddToSellItemCart_Implementation(const struct FSellItemInfo& ItemInfo);

	UFUNCTION(Server, Reliable)
	void ItemMoveToCart(const struct FSellItemInfo& ItemInfo);
	void ItemMoveToCart_Implementation(const struct FSellItemInfo& ItemInfo);

	UFUNCTION(Server, Reliable)
	void SellItem(const TArray<FCachedSellItemInfo>& SellItems, int32 TotalPrice);
	void SellItem_Implementation(const TArray<FCachedSellItemInfo>& SellItems, int32 TotalPrice);
	
	// Obsolete, no need to call
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SendLoadEndToGateGameMode();
	void SendLoadEndToGateGameMode_Implementation();
	// =======================================

	UPROPERTY()
	class UManagerActorRegistrySubsystem* ManagerActorSubsystem;

	DBNetConnection* DBConnection;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget")
	//TSubclassOf<UUserWidget> GateSelect;

	//UPROPERTY()
	//UFH_W_SelectGate* GateSelectUI;

	// Begin StatusUI
	//UCommonUserWidget* asd;
	
	
	// ~End StatusUI
	



	// ============= Logic Test Area ===============
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetNextRound();
	void SetNextRound_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void IncreaseDeathCount();
	void IncreaseDeathCount_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void IncreasePlayerMoney();
	void IncreasePlayerMoney_Implementation();

	// ============== Game Menu ================
	UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleGameMenu();

	uint32 bIsGameMenuOpen : 1 { false };

	// ============== Observer Mode ==============
public:
	UFUNCTION(Server, Reliable)
	void EnterObserverMode();
	void EnterObserverMode_Implementation();

	UFUNCTION(Server, Reliable)
	void RequestChangeViewTarget(class AFHObserverPawn* ObserverPawn, bool bNext /* = true */);
	void RequestChangeViewTarget_Implementation(AFHObserverPawn* ObserverPawn, bool bNext);

	int32 ObservingTargetIndex = -1;

	// Fade In/Out Callback Delegate
private:
	FOnFadeEnd ShowQuotaCallback;



	// ================= In gate properties ====================
public:
	UFUNCTION(Server, Reliable)
	void Server_NotifyRoomDisclosed(int64 RoomID);
	void Server_NotifyRoomDisclosed_Implementation(int64 RoomID);



public:
	UFUNCTION(BlueprintCallable)
	void SetCameraRotation(FRotator NewControlRotation);

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_MyControlRotation)
	FRotator MyControlRotation;

	UFUNCTION()
	void OnRep_MyControlRotation();
};

DECLARE_LOG_CATEGORY_EXTERN(LogFHController, Log, All);