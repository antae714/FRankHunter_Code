// Copyright F Rank Hunter.. All Rights Reserved.


#include "FHPlayerController.h"
#include <steam/isteamfriends.h>
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"

#include "FRankHunter.h"
#include "Player/FHPlayerBase.h"
#include "Props/FHTerminalBase.h"
#include "DungeonGeneration/FHDungeonStructs.h"

#include <Private/OnlineSubsystemSteamTypes.h>

#include "Kismet/GameplayStatics.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"

#include "Lobby/FH_GS_LobbyGameState.h"
#include "Lobby/FHLobbyGameMode.h"
#include "Core/FHGateGameModeBase.h"
#include "Core/FHGateGameStateBase.h"
#include "Core/FHPlayerStateBase.h"
#include "Core/FHPropManager.h"
#include "Player/FHObserverPawn.h"
#include "ManagerActorRegistrySubsystem.h"
#include "Core/FHGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "CommonActivatableWidget.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"
#include "GAS/FHGameplayTags.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DBNetConnection.h"
#include "GameAnalyzeData.h"

#include "Props/FHKioskBase.h"

#include "UI/Lobby/FH_W_GateEnterRequest.h"
#include "UI/Lobby/FH_W_SelectGate.h"
#include "UI/Lobby/FHPaymentBase.h"
#include "UI/Lobby/FHBuyUIBase.h"
#include "UI/Lobby/FHShopBase.h"
#include "UI/Lobby/LobbyUIUtils.h"
#include "UI/Lobby/FHKioskMain.h"
#include "UI/Lobby/FHSellUIBase.h"
#include "UI/Lobby/FHRestRequest.h"
#include "UI/Voice/FHVoiceWidget.h"
#include "UI/FHStatusBase.h"
#include "UI/FHMinimapBase.h"
#include "UI/FHStatusHudBase.h"
#include "FHUIManager.h"
#include "Net/UnrealNetwork.h"
#include "UI/ItemQuickSlot/FHItemQuickSlot.h"
#include "UI/Common/FHFadeInOut.h"
#include "UI/Lobby/FHQuotaNotice.h"
#include "UI/Gate/FHLoading.h"
#include "Component/FHRPCComponent.h"
#include "UI/FHObserverHud.h"
#include "Component/WidgetRPC/FHGateEnterRequestRPCComponent.h"

DEFINE_LOG_CATEGORY(LogFHController);


AFHPlayerController::AFHPlayerController()
{
	// 서버에서 지정된 view target으로 전환 하는 것을 방지.
	bAutoManageActiveCameraTarget = false;
}

void AFHPlayerController::BeginPlay()
{
	NET_DEBUG_LOG(TEXT(""));

	UE_LOG(LogTemp, Warning, TEXT("FHPlayerController Begin"));

	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("FHPlayerController End"));
	EnableCheats();
	/*if (!GetWorld()->IsPlayInEditor() && IsLocalController() && HasAuthority())
	{
		DBConnection = new DBNetConnection();
		if (DBConnection->Initialize())
		{
			UE_LOG(LogFHController, Warning, TEXT("DBConnection Initialize Success."));




			DBConnection->SaveSendLogPacket(Data.GetBuffer(), NumBytes, NumBits);
			int32 ByteSend = DBConnection->LogSend();
			UE_LOG(LogFHController, Log, TEXT("%dByte Sent."), ByteSend);
		}
		else
		{
			delete DBConnection;
			DBConnection = nullptr;
		}
	}*/

	if (IsLocalController())
	{
		ManagerActorSubsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();

		//AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
		//if (UIManager)
		//{
		//	UFHVoiceWidget* Widget = UIManager->GetWidget<UFHVoiceWidget>(TEXT("VoiceInterface"));
		//	Widget->SetVisibility(ESlateVisibility::Visible);
		//}

		// if assert this, "UI/Lobby/SelectCharacter/CharacterPreview" Blueprint to playercontroller
		check(PreviewCameraClass);

		// spawn PreviewCamera
		PreviewCameraInstance = GetWorld()->SpawnActor(PreviewCameraClass);
		PreviewCameraInstance->SetActorLocation(DefaultCameraPosition);


	}
	// Notify the AuthGameMode that PlayerController Is Ready on the Local Client.
	if (IsLocalController())
	{
		NotifyReady();

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}

	if (IsLocalController() && GameMenuWidgetClass)
	{
		if (!IsValid(GameMenuWidgetInstance))
		{
			GameMenuWidgetInstance = CreateWidget<UCommonActivatableWidget>(this, GameMenuWidgetClass);

			if (IsValid(GameMenuWidgetInstance) && !GameMenuWidgetInstance->IsInViewport())
			{
				GameMenuWidgetInstance->AddToViewport(1000);
				GameMenuWidgetInstance->DeactivateWidget();
				bIsGameMenuOpen = false;
			}
		}	
	}

	if (IsLocalController())
	{
		ShowQuotaCallback.BindDynamic(this, &AFHPlayerController::Callback_ShowQuota);
	}
}

// Server only
void AFHPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	NET_DEBUG_LOG(TEXT(""));

	if (AFHPlayerBase* PlayerPawn = Cast<AFHPlayerBase>(aPawn))
	{
		OnPlayerStateChangedFunction();

		AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GateGameMode && GateGameMode->GetGateState() == EGateState::Loading)
		{
			ClientRoomLoadStart();
			TurnOffPhysics();
		}

		if (AFHPlayerStateBase* PS = GetPlayerState<AFHPlayerStateBase>())
		{
			PlayerPawn->ActivateSkill(PS->GetSkillID());
			PS->UpdateSkillWidget();
		}
	}
	else if (AFHObserverPawn* ObserverPawn = Cast<AFHObserverPawn>(aPawn))
	{
		UE_LOG(LogFHController, Log, TEXT("Successfully possessed AFHObserverPawn."));
		RequestChangeViewTarget(ObserverPawn, true);
	}
}

void AFHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();


}

void AFHPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogFHController, Warning, TEXT("EndPlay"));

	if (DBConnection)
	{
		delete DBConnection;
	}
}

void AFHPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	OnPlayerStateChangedFunction();
	NET_DEBUG_LOG(TEXT(""));
}

void AFHPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	NET_DEBUG_LOG(TEXT(""));
	APawn* CurrentPawn = GetPawn();
	if (!CurrentPawn)
	{
		return;
	}

	/*if (AFHPlayerBase* PlayerPawn = Cast<AFHPlayerBase>(CurrentPawn))
	{
		AFHPlayerStateBase* MyPlayerState = GetPlayerState<AFHPlayerStateBase>();
		if (MyPlayerState && MyPlayerState->IsObserving())
		{
			UE_LOG(LogFHController, Log, TEXT("Setting input to Observer."));
			PlayerPawn->SetObserverInputMode(true);
		}
		else
		{
			UE_LOG(LogFHController, Log, TEXT("Setting input to Player."));
			PlayerPawn->SetObserverInputMode(false);
		}
	}*/
}

void AFHPlayerController::ChangeState(FName NewState)
{
	Super::ChangeState(NewState);
	OnStateChanged.Broadcast(NewState);
}

void AFHPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFHPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyControlRotation);
}

void AFHPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	// TODO: Release All Local and Remote Talkers.
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	FName SubsystemName = OSS->GetSubsystemName();
	IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface(SubsystemName);
	if (OnlineVoicePtr.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove All Talkers."));
		OnlineVoicePtr->UnregisterLocalTalkers();
		OnlineVoicePtr->RemoveAllRemoteTalkers();
	}

	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
}

void AFHPlayerController::PawnLeavingGame()
{
	if (UFHBlueprintFunctionLibrary::IsGate(this))
	{
		//Super::PawnLeavingGame();
	}
	else
	{
		Super::PawnLeavingGame();
	}
}

void AFHPlayerController::SuperPawnLeavingGame()
{
	Super::PawnLeavingGame();
}

bool AFHPlayerController::InputKey(const FInputKeyEventArgs& Params)
{
	if (Params.Key == EKeys::Escape && Params.Event == IE_Pressed)
	{
		ToggleGameMenu();
	}

	return Super::InputKey(Params);
}

void AFHPlayerController::CreateLoadingUI()
{
	// Not Use. Move To GameInstance.
	// 
	// if nullptr, add WBP_Loading to BP_FHPlayerController.
	//check(LoadingUIClass);
	//
	//LoadingUIInstance = CreateWidget<UFHLoading>(this, LoadingUIClass);
	//LoadingUIInstance->AddToViewport(100);
	//LoadingUIInstance->Init();
}

void AFHPlayerController::OnPlayerStateChangedFunction()
{
	NET_DEBUG_LOG(TEXT(""));

	if (IsLocalController())
	{
		if (!StatusUIClassInstance)
		{
			StatusUIClassInstance = Cast<UCommonActivatableWidget>(CreateWidget(this, StatusUIClass));
		}
		if (StatusUIClassInstance)
		{
			StatusUIClassInstance->AddToViewport(1);
		}

		if (!StatusHudInstance)
		{
			StatusHudInstance = CreateWidget<UFHStatusHudBase>(this, StatusHudClass);
		}
		if (StatusHudInstance)
		{
			StatusHudInstance->AddToViewport(0);
		}

		//AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(PlayerState);
		//if (PS && PS->IsFirstPlay() && PreviewCameraClass)
		//{
		//	//bAutoManageActiveCameraTarget = false;

		//	//PreviewCameraInstance = GetWorld()->SpawnActor(PreviewCameraClass);
		//	//PreviewCameraInstance->SetActorLocation(FVector(-4740.f, 750.f, 270.f));
		//	bShowMouseCursor = true;
		//	SetViewTarget(PreviewCameraInstance);
		//	StatusHudInstance->SetVisibility(ESlateVisibility::Collapsed);
		//}
	}
	OnPlayerStateChanged.Broadcast(PlayerState);
}


void AFHPlayerController::TurnOnPhysics()
{
	if (!HasAuthority())
	{
		PRINT_LOG(TEXT("Why am I here?"));
	}

	ACharacter* PlayerCharacter = GetCharacter();
	if (PlayerCharacter)
	{
		UCharacterMovementComponent* moveComp = PlayerCharacter->GetCharacterMovement();

		moveComp->SetMovementMode(MOVE_Walking);
		//moveComp->GravityScale = 1.0f;
		//moveComp->Velocity = FVector::ZeroVector;
		//moveComp->SetComponentTickEnabled(true);

		/*FVector CurrentLocation = PlayerCharacter->GetActorLocation();
		CurrentLocation.Z += 100.0f;
		FRotator CurrentRotation = PlayerCharacter->GetActorRotation();
		PlayerCharacter->SetActorLocationAndRotation(CurrentLocation, CurrentRotation, false, nullptr, ETeleportType::TeleportPhysics);*/
	}
}

void AFHPlayerController::TurnOffPhysics()
{
	if (!HasAuthority())
	{
		return;
	}

	ACharacter* PlayerCharacter = GetCharacter();
	if (PlayerCharacter)
	{
		UCharacterMovementComponent* moveComp = PlayerCharacter->GetCharacterMovement();

		moveComp->SetMovementMode(MOVE_None);
		//moveComp->GravityScale = 0.0f;
		//moveComp->Velocity = FVector::ZeroVector;
		//moveComp->SetComponentTickEnabled(false);
	}
}


void AFHPlayerController::StopMinimapUpdate_Implementation()
{
	AFHPlayerBase* player = Cast<AFHPlayerBase>(GetPawn());
	if (player)
	{
		player->GetTerminalActor()->ShutOffTerminal();
	}
}

//void AFHPlayerController::Client_RequestChangeViewTarget_Implementation(AActor* NewViewTarget, float BlendTime)
//{
//	if (NewViewTarget)
//	{
//		FViewTargetTransitionParams AutoBlend{};
//		AutoBlend.BlendTime = BlendTime;
//		SetViewTarget(NewViewTarget, AutoBlend);
//	}
//}

void AFHPlayerController::ClientRoomLoadStart_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	//LoadingUIInstance = CreateWidget<UFHLoading>(this, LoadingUIClass);
	//LoadingUIInstance->AddToViewport(100);
	//LoadingUIInstance->Init();

	//AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	//if (UIManager)
	//{
	//	//UIManager->RegistWidget(LoadingUIInstance, TEXT("Loading"));
	//	UFHLoading* Widget = UIManager->GetWidget<UFHLoading>(TEXT("Loading"));
	//	if (Widget)
	//	{
	//		Widget->SetVisibility(ESlateVisibility::Visible);
	//	}
	//}
}

void AFHPlayerController::ClientRoomLoadEnd_Implementation()
{
	//if (LoadingUIInstance)
	//{
	//	LoadingUIInstance->RemoveFromParent();
	//	LoadingUIInstance = nullptr;
	//}
	GetGameInstance<UFHGameInstance>()->HideLoadingWidget();

	/*AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GateGameMode)
	{
		GateGameMode->PlayerLoadEnd(this);
	}*/
	StartVoiceChat();
}

void AFHPlayerController::SeeGateEnd_Implementation()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("SeeGateEnd: World is not valid."));
		return;
	}

	AFHPropManager* PropManager = UFHBlueprintFunctionLibrary::GetPropManager(World);
	if (IsValid(PropManager))
	{
		AActor* GateEndCamera = PropManager->GetProp<AActor>(TEXT("GateEndCamera"));
		if (IsValid(GateEndCamera))
		{
			if (IsValid(StatusHudInstance))
			{
				TurnOffStatusHud();
			}

			// ViewTarget을 설정하기 전에 PlayerCameraManager가 유효한지 확인
			if (IsValid(PlayerCameraManager))
			{
				SetViewTarget(GateEndCamera);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("SeeGateEnd: PlayerCameraManager is not valid. Cannot set view target."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SeeGateEnd: GateEndCamera is not valid."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SeeGateEnd: PropManager is not valid."));
	}

	//AFHPropManager* PropManager = UFHBlueprintFunctionLibrary::GetPropManager(GetWorld());
	//if (PropManager)
	//{
	//	AActor* GateEndCamera = PropManager->GetProp<AActor>(TEXT("GateEndCamera"));
	//	if (GateEndCamera)
	//	{
	//		TurnOffStatusHud();
	//		SetViewTarget(GateEndCamera);
	//	}
	//}
}


void AFHPlayerController::CheckFirstPlay_Implementation()
{
	AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(PlayerState);
	if (!PS)
	{
		return;
	}

	if (PS->IsFirstPlay() && PreviewCameraInstance)
	{
		//AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
		//if (!UIManager)
		//{
		//	return;
		//}

		//UFHFadeInOut* FadeInOut = UIManager->GetWidget<UFHFadeInOut>(TEXT("FadeInOut"));
		//if (!FadeInOut)
		//{
		//	return;
		//}
		//FadeInOut->PlayFadeOut();

		FOnFadeEnd OnFadeEndDelegate;
		GetGameInstance<UFHGameInstance>()->PlayFadeOut(OnFadeEndDelegate);

		bShowMouseCursor = true;
		SetViewTarget(PreviewCameraInstance);
		StatusHudInstance->SetVisibility(ESlateVisibility::Collapsed);
	}	
}

void AFHPlayerController::SpawnPreviewCameraAndSetView_Implementation()
{
	if (PreviewCameraClass)
	{


		/*PreviewCameraInstance = GetWorld()->SpawnActor(PreviewCameraClass);
		PreviewCameraInstance->SetActorLocation(FVector(-4740.f, 750.f, 270.f));*/
		
	}
}


void AFHPlayerController::StartVoiceChat_Implementation()
{
	UFHGameInstance* GameInstance = GetGameInstance<UFHGameInstance>();
	if (!GameInstance)
	{
		return;
	}
	GameInstance->VoiceChatOn();
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();
	if (IsLocalController() && VoiceInterface.IsValid())
	{
		int32 PlayerNum = UGameplayStatics::GetNumPlayerStates(GetWorld());
		for (int32 index = 0; index < PlayerNum; index++)
		{
			APlayerState* PS = UGameplayStatics::GetPlayerState(GetWorld(), index);
			if(!PS)
			{
				continue;
			}
			VoiceInterface->UnmuteRemoteTalker(0, *PS->GetUniqueId(), true);
		}
	}
}


void AFHPlayerController::TurnOffStatusHud()
{
	Client_SetStatusHudVisibility(ESlateVisibility::Collapsed);
}

void AFHPlayerController::TurnOnStatusHud()
{
	Client_SetStatusHudVisibility(ESlateVisibility::Visible);
}

void AFHPlayerController::Client_SetStatusHudVisibility_Implementation(ESlateVisibility NewVisibility)
{
	if (IsValid(StatusHudInstance))
	{
		StatusHudInstance->SetVisibility(NewVisibility);
	}
}

void AFHPlayerController::SetAutoRefuse_Implementation(bool State)
{
	bIsAutoRefuseRequest = State;
}

void AFHPlayerController::ShowQuota_Implementation(int32 LoopCount)
{
	if (!IsLocalController())
	{
		return;
	}

	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GameState)
	{
		return;
	}

	GameState->LoopCount = LoopCount;
	//AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	//if (UIManager)
	//{
	//	UFHFadeInOut* FadeInOut = UIManager->GetWidget<UFHFadeInOut>(TEXT("FadeInOut"));
	//	if (FadeInOut)
	//	{
	//		FadeInOut->OnFadeOutEndDelegate = ShowQuotaCallback;
	//		FadeInOut->PlayFadeOut();
	//	}
	//}
	bIsAutoRefuseRequest = false;
	if (!GameState->GetLobbyState())
	{
		return;
	}
	GetGameInstance<UFHGameInstance>()->PlayFadeOut(ShowQuotaCallback);
}

void AFHPlayerController::Callback_ShowQuota()
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (UIManager)
	{
		UFHQuotaNotice* Notice = UIManager->GetWidget<UFHQuotaNotice>(TEXT("Notice"));
		if (Notice)
		{
			Notice->Show();
		}
	}
}

void AFHPlayerController::FadeIn_Implementation()
{
	//AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	//if (UIManager)
	//{
	//	UFHFadeInOut* Widget = UIManager->GetWidget<UFHFadeInOut>(TEXT("FadeInOut"));
	//	if (Widget)
	//	{
	//		Widget->PlayFadeIn();
	//	}
	//}

	FOnFadeEnd FadeEndDelegate;
	GetGameInstance<UFHGameInstance>()->PlayFadeIn(FadeEndDelegate);
}

void AFHPlayerController::FadeOut_Implementation()
{
	//AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	//if (UIManager)
	//{
	//	UFHFadeInOut* Widget = UIManager->GetWidget<UFHFadeInOut>(TEXT("FadeInOut"));
	//	if (Widget)
	//	{
	//		if (Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld())))
	//		{
	//			Widget->OnFadeOutEndDelegate.AddDynamic(this, &AFHPlayerController::ShowQuota);
	//		}
	//		Widget->PlayFadeOut();
	//	}
	//}

	FOnFadeEnd OnFadeEndDelegate;
	GetGameInstance<UFHGameInstance>()->PlayFadeOut(OnFadeEndDelegate);
}

//void AFHPlayerController::OpenGateSelectUI_Implementation()
//{
//	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
//	if (UIManager)
//	{
//		UFH_W_SelectGate* SelectGateWidget = UIManager->GetWidget<UFH_W_SelectGate>(TEXT("SelectGate"));
//		if (SelectGateWidget)
//		{
//			SelectGateWidget->SetVisibility(ESlateVisibility::Visible);
//			FInputModeUIOnly InputMode;
//			InputMode.SetWidgetToFocus(SelectGateWidget->TakeWidget());
//			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
//
//			SetInputMode(InputMode);
//
//			bShowMouseCursor = true;
//		}
//	}
//}

void AFHPlayerController::OpenKiosk_Implementation()
{
	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFHKioskMain* Kiosk = UIManager->GetWidget<UFHKioskMain>(TEXT("Kiosk"));
		if (Kiosk)
		{
			Kiosk->SetVisibility(ESlateVisibility::Visible);
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(Kiosk->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			SetInputMode(InputMode);

			bShowMouseCursor = true;
		}
	}	
}

void AFHPlayerController::OpenPaymentUI_Implementation()
{
	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFHPaymentBase* Payment = UIManager->GetWidget<UFHPaymentBase>(TEXT("Payment"));
		if (Payment)
		{
			Payment->SetVisibility(ESlateVisibility::Visible);
			Payment->OpenUI();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(Payment->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			SetInputMode(InputMode);

			bShowMouseCursor = true;
		}

	}

	//if (PaymentUI)
	//{
	//	PaymentUI->SetVisibility(ESlateVisibility::Visible);
	//	PaymentUI->OpenUI();

	//	FInputModeUIOnly InputMode;
	//	InputMode.SetWidgetToFocus(PaymentUI->TakeWidget());
	//	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	//	SetInputMode(InputMode);

	//	bShowMouseCursor = true;
	//}
}

void AFHPlayerController::OnEnterButtonPressed(const FString& GateName, const EGateRank& GateRank)
{
	AFHLobbyGameMode* GameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OpenEnterRequestWidget(GateName, GateRank);
	}
}

void AFHPlayerController::OpenEnterRequestUI_Implementation(const FString& GateName)
{
	if (bIsAutoRefuseRequest)
	{
		UFHGateEnterRequestRPCComponent* RPCComponent = GetComponentByClass<UFHGateEnterRequestRPCComponent>();
		if (RPCComponent)
		{
			RPCComponent->OnAnswerEnterRequest(false);
			return;
		}
	}

	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFH_W_GateEnterRequest* EnterRequest = UIManager->GetWidget<UFH_W_GateEnterRequest>(TEXT("EnterRequest"));
		if (EnterRequest)
		{
			EnterRequest->CurIdIndex = 0;

			EnterRequest->OpenAnswerWidget(GateName);
			EnterRequest->SetVisibility(ESlateVisibility::Visible);
			bShowMouseCursor = true;

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(EnterRequest->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			SetInputMode(InputMode);
		}
	}
}


void AFHPlayerController::OpenEnterErrorUI_Implementation()
{
	if (bIsAutoRefuseRequest)
	{
		return;
	}

	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFH_W_GateEnterRequest* EnterRequest = UIManager->GetWidget<UFH_W_GateEnterRequest>(TEXT("EnterRequest"));
		if (EnterRequest)
		{
			EnterRequest->OpenErrorWidget();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(EnterRequest->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			bShowMouseCursor = true;

			SetInputMode(InputMode);
		}
	}
}


void AFHPlayerController::ResetGateButton_Implementation(const TArray<uint8>& SelectedGateInfo)
{
	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFH_W_SelectGate* SelectGateWidget = UIManager->GetWidget<UFH_W_SelectGate>(TEXT("SelectGate"));
		if (SelectGateWidget)
		{
			SelectGateWidget->ResetGateSelectPannel(SelectedGateInfo);

		}
	}
}

void AFHPlayerController::OnPlayerRefuse_Implementation(const FString& PlayerName)
{
	//FString Temp = FString::Printf(TEXT("%d"), PlayerId);
	//UE_LOG(LogTemp, Warning, TEXT("Refuse Player ID: %s"), *Temp);

	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFH_W_GateEnterRequest* EnterRequest = UIManager->GetWidget<UFH_W_GateEnterRequest>(TEXT("EnterRequest"));
		if (EnterRequest)
		{
			EnterRequest->SetRefusePlayerName(PlayerName);
			EnterRequest->CurIdIndex++;
		}	
	}
}

void AFHPlayerController::CloseEnterRequestUI_Implementation()
{
	if (bIsAutoRefuseRequest)
	{
		return;
	}

	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFH_W_GateEnterRequest* EnterRequest = UIManager->GetWidget<UFH_W_GateEnterRequest>(TEXT("EnterRequest"));
		if (EnterRequest)
		{
			EnterRequest->OpenTimeOutWidget();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(EnterRequest->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			SetInputMode(InputMode);
		}
	}
}



void AFHPlayerController::PurchaseError_Implementation(const EPurchaseError& Result)
{
	AFHPropManager* PropManager = UFHBlueprintFunctionLibrary::GetPropManager(GetWorld());
	if (PropManager)
	{
		AFHKioskBase* Kiosk = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
		if (Kiosk)
		{
			Kiosk->NotifyPurchaseError(Result);
		}
	}
}


void AFHPlayerController::AddToSellingCartError_Implementation(const ESellItemError& Result)
{

}


void AFHPlayerController::ItemMoveToCartError_Implementation(const ESellItemError& Result)
{

}


void AFHPlayerController::ItemSellError_Implementation(const ESellItemError& Result)
{

}


void AFHPlayerController::OpenRestRequest_Implementation()
{
	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFHRestRequest* Widget = UIManager->GetWidget<UFHRestRequest>(TEXT("RestRequest"));
		if (Widget)
		{
			Widget->OpenWidget();
			Widget->SetQuota();
		}
	}
}


void AFHPlayerController::CloseRestRequest_Implementation()
{
	AFHUIManager* UIManager = ManagerActorSubsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	if (UIManager)
	{
		UFHRestRequest* Widget = UIManager->GetWidget<UFHRestRequest>(TEXT("RestRequest"));
		if (Widget)
		{
			Widget->CloseWidget();
		}
	}
}

void AFHPlayerController::OnPay_Implementation(int32 Value)
{
	AFHLobbyGameMode* GameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->SubmitPay(Value);
	}
}

void AFHPlayerController::NotifyReady_Implementation()
{
	UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode());
		AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
		if (GateGameMode)
		{
			GateGameMode->PlayerReady(this);
		}
		else if(LobbyGameMode)
		{
			LobbyGameMode->PlayerReady(this);
		}
	}
}

void AFHPlayerController::AddToSellItemCart_Implementation(const FSellItemInfo& ItemInfo)
{
	AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->AddToSellItemCart(this, ItemInfo);
	}
}


void AFHPlayerController::ItemMoveToCart_Implementation(const struct FSellItemInfo& ItemInfo)
{
	AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->ItemMoveToCart(this, ItemInfo);
	}
}


void AFHPlayerController::SellItem_Implementation(const TArray<FCachedSellItemInfo>& SellItems, int32 TotalPrice)
{
	AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		//LobbyGameMode->SellItem(this, SellItems, TotalPrice);
	}
}

// Obsolete, no need to call
void AFHPlayerController::SendLoadEndToGateGameMode_Implementation()
{
	AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GateGameMode)
	{
		GateGameMode->PlayerLoadEnd(this);
	}
}

// ================= Logic Test Area ===================
void AFHPlayerController::SetNextRound_Implementation(){
	AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->SetNextRound();
	}
}

void AFHPlayerController::IncreaseDeathCount_Implementation()
{
	AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->IncreaseDeathCount();
	}
}

void AFHPlayerController::IncreasePlayerMoney_Implementation()
{
	AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->IncreaseMoney();
	}
}

// ============== Game Menu ================
void AFHPlayerController::ToggleGameMenu()
{
	if (!IsValid(GameMenuWidgetInstance))
	{
		return;
	}

	if (bIsGameMenuOpen)
	{
		// Close Menu
		if (GameMenuWidgetInstance->IsActivated())
		{
			GameMenuWidgetInstance->DeactivateWidget();
		}

		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
		bIsGameMenuOpen = false;
	}
	else
	{
		// Open Menu
		if (!GameMenuWidgetInstance->IsActivated())
		{
			GameMenuWidgetInstance->ActivateWidget();
		}
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(GameMenuWidgetInstance->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(false);
		SetInputMode(InputModeData);

		bShowMouseCursor = true;
		bIsGameMenuOpen = true;
	}
}

// ============== Observer Mode ================
void AFHPlayerController::EnterObserverMode_Implementation()
{
	AFHGateGameModeBase* GateGameModeBase = GetWorld()->GetAuthGameMode<AFHGateGameModeBase>();
	if (GateGameModeBase == nullptr)
	{
		return;
	}

	AFHPlayerBase* _Player = Cast<AFHPlayerBase>(GetPawn());
	if (_Player)
	{
		_Player->SetObservingFlag();
	}

	if (AFHPlayerStateBase* FHPlayerStateBase = GetPlayerState<AFHPlayerStateBase>())
	{
		FHPlayerStateBase->SetIsObserving(true);
	}

	//ChangeState(NAME_Spectating);

	/*if (StatusHudInstance)
	{
		TurnOffStatusHud();
	}*/

	GateGameModeBase->PlayerDied(this);
	GateGameModeBase->PossessObserverPawn(this);
}

void AFHPlayerController::RequestChangeViewTarget_Implementation(AFHObserverPawn* ObserverPawn, bool bNext)
{
	AFHGateGameStateBase* FHGateGameStateBase = GetWorld()->GetGameState<AFHGateGameStateBase>();
	if (!FHGateGameStateBase || !ObserverPawn)
	{
		return;
	}

	SetViewTarget(ObserverPawn);

	TArray<AFHPlayerStateBase*> AlivePlayers;
	FHGateGameStateBase->GetAlivePlayerList(AlivePlayers);
	if (AlivePlayers.Num() <= 0)
	{
		ObserverPawn->SetObservingTarget(nullptr);
		return;
	}
	if (bNext)
	{
		ObservingTargetIndex = (ObservingTargetIndex + 1) % AlivePlayers.Num();
	}
	else
	{
		ObservingTargetIndex--;
		if (ObservingTargetIndex < 0)
		{
			ObservingTargetIndex = AlivePlayers.Num() - 1;
		}
	}

	if (AlivePlayers.IsValidIndex(ObservingTargetIndex))
	{
		AFHPlayerStateBase* TargetPS = AlivePlayers[ObservingTargetIndex];
		APawn* ObservingTargetPawn = TargetPS ? TargetPS->GetPawn() : nullptr;

		if (ObservingTargetPawn)
		{
			ObserverPawn->SetObservingTarget(ObservingTargetPawn);
		}
	}
	else
	{
		ObserverPawn->SetObservingTarget(nullptr);
	}
}

void AFHPlayerController::Server_NotifyRoomDisclosed_Implementation(int64 RoomID)
{
	AFHGateGameStateBase* GateState = Cast<AFHGateGameStateBase>(GetWorld()->GetGameState());
	if (!GateState) return;

	GateState->Server_NotifyRoomDisclosed(RoomID);
}

void AFHPlayerController::SetCameraRotation(FRotator NewControlRotation)
{
	MyControlRotation = NewControlRotation;
	OnRep_MyControlRotation();
}

void AFHPlayerController::OnRep_MyControlRotation()
{
	SetControlRotation(MyControlRotation);
}
