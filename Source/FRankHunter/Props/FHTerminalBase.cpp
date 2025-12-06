// Copyright F Rank Hunter. All Rights Reserved.

#include "Props/FHTerminalBase.h"

#include "FRankHunter.h"
#include "Net/UnrealNetwork.h"

#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Core/FHPlayerController.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Core/FHGateGameStateBase.h"
#include "Core/FHGateGameModeBase.h"
#include "UI/FHMinimapBase.h"
#include "Player/FHPlayerBase.h"
#include "ProceduralDungeonUtils.h"
#include "Room.h"
#include "Door.h"
#include "DataAsset/FHRoomData.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/VoiceInterface.h"
#include "Core/FHPlayerStateBase.h"
#include "Core/FHGateGameStateBase.h"
#include "Lobby/FH_GS_LobbyGameState.h"

#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"
#include "Net/VoiceConfig.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"

#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"

AFHTerminalBase::AFHTerminalBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	SetRootComponent(BodyMesh);
	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(BodyMesh);
	DisplayWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DisplayWidget"));
	DisplayWidget->SetupAttachment(BodyMesh);

	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"), false);
	BodyMesh->SetCanEverAffectNavigation(false);
	DisplayMesh->SetCollisionProfileName(TEXT("NoCollision"), false);
	DisplayMesh->SetCanEverAffectNavigation(false);

	DisplayWidget->SetRelativeScale3D({ 0, 0, 0 });
	DisplayWidget->SetTickWhenOffscreen(true);

	//VOIPComponent = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalker"));
	//VOIPComponent->Settings.ComponentToAttachTo = BodyMesh;
	//VOIPComponent->SetIsReplicated(true);
	LastCachedVolume = 0.0f;
}

void AFHTerminalBase::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayer = Cast<AFHPlayerBase>(GetOwner());
	CurrentBattery = InitialBattery;

	GetWorldTimerManager().SetTimer(InitializationTimer, this, &AFHTerminalBase::InitializeTerminal, 0.5f, false);

	//GetWorld()->GetTimerManager().SetTimerForNextTick();

	/*if (HasAuthority())
	{
		AFHGateGameModeBase* GateMode = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		bIsInGate = GateMode != nullptr ? true : false;
	}*/
	
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	FName SubsystemName = OSS->GetSubsystemName();
	IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface(GetWorld(), SubsystemName);
	if (OnlineVoicePtr.IsValid())
	{
		OnlineVoicePtr->OnPlayerTalkingStateChangedDelegates.AddUObject(this, &AFHTerminalBase::OnPlayerTalking);
	}
}

void AFHTerminalBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void AFHTerminalBase::InitializeTerminal()
{
	if (OwnerPlayer == nullptr)
	{
		OwnerPlayer = Cast<AFHPlayerBase>(GetOwner());
	}

	if (OwnerPlayer.IsValid() == false || OwnerPlayer->IsLocallyControlled() == false)
	{
		return;
	}

	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (GameState)
	{
		AFH_GS_LobbyGameState* LobbyState = Cast<AFH_GS_LobbyGameState>(GameState);
		if (LobbyState == nullptr) return;

		DisplayMesh->SetMaterial(0, LobbyTerminalMaterial);

		LobbyWidgetInstance = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), LobbyWidgetClass, TEXT("LobbyDisplay"));

		DisplayWidget->SetWidget(LobbyWidgetInstance);
		DynamicDisplayMaterial = DisplayMesh->CreateDynamicMaterialInstance(0);

		DisplayMesh->SetMaterial(0, DynamicDisplayMaterial);
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateLambda([this]()
		{
			DynamicDisplayMaterial->SetTextureParameterValue(DisplayMaterialRenderTargetTextureParameterName, DisplayWidget->GetRenderTarget());
		})
		);
	}
	else
	{
		GetWorldTimerManager().SetTimer(InitializationTimer, this, &AFHTerminalBase::InitializeTerminal, 0.1f, false);
	}
}

void AFHTerminalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsInGate == false) return;
	if (OwnerPlayer.IsValid() == false) return;
	if (OwnerPlayer->IsLocallyControlled() == false) return;

	if (bUpdateMinimap && MinimapWidgetInstance)
	{
		ElapsedTimeForLastMinimapUpdate += DeltaTime;
		if (ElapsedTimeForLastMinimapUpdate >= MinimapUpdateInterval)
		{
			ElapsedTimeForLastMinimapUpdate -= MinimapUpdateInterval;
			UpdateMinimapWidget();
		}
	}

	if (bUpdateBattery)
	{
		ElapsedTimeFromLastBatteryUpdate += DeltaTime;
		if (ElapsedTimeFromLastBatteryUpdate >= BatteryReduceIntervalSecond)
		{
			ElapsedTimeFromLastBatteryUpdate -= BatteryReduceIntervalSecond;

			UpdateBattery();
		}
	}


	/*if (bIsWidgetAttached)
	{
		DynamicDisplayMaterial->SetTextureParameterValue(*DynamicMaterialParameterName, Cast<UTexture>(DisplayWidget->GetRenderTarget()));
	}
	else
	{
		AttachPlayerMinimapInstance();
	}*/
}

void AFHTerminalBase::StartTalk()
{
	if (bIsRemoteTalking)
	{
		return;
	}

	bIsLocalTalking = true;
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	FName SubsystemName = OSS->GetSubsystemName();
	IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface(GetWorld(), SubsystemName);
	OnlineVoicePtr->StartNetworkedVoice(0);
}

void AFHTerminalBase::StopTalk()
{
	if (!bIsLocalTalking)
	{
		return;
	}

	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	FName SubsystemName = OSS->GetSubsystemName();
	IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface(GetWorld(), SubsystemName);
	OnlineVoicePtr->StopNetworkedVoice(0);
}

void AFHTerminalBase::VoiceChannelOn()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	FName SubsystemName = OSS->GetSubsystemName();
	IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface(GetWorld(), SubsystemName);

	AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(GetWorld());
	TArray<TObjectPtr<APlayerState>>& Players = GameStateBase->PlayerArray;
	for (int32 Index = 1; Index < Players.Num(); Index++)
	{
		OnlineVoicePtr->UnmuteRemoteTalker(0, *(Players[Index]->GetUniqueId()), false);
	}
	UKismetSystemLibrary::PrintString(this, TEXT("Remote Unmutted."));
}

void AFHTerminalBase::VoiceChannelOff()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	FName SubsystemName = OSS->GetSubsystemName();
	IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface(GetWorld(), SubsystemName);

	AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(GetWorld());
	TArray<TObjectPtr<APlayerState>>& Players = GameStateBase->PlayerArray;
	for (int32 Index = 1; Index < Players.Num(); Index++)
	{
		OnlineVoicePtr->MuteRemoteTalker(0, *(Players[Index]->GetUniqueId()), false);
	}
	UKismetSystemLibrary::PrintString(this, TEXT("Remote Mutted."));
}

void AFHTerminalBase::OnPlayerTalking(TSharedRef<const FUniqueNetId> UniqueNetIdRef, bool IsTalking)
{
	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	if (!(UniqueNetIdRef.Get() == *(PlayerState->GetUniqueId())))
	{
		bIsRemoteTalking = IsTalking;
	}

	UVOIPTalker* Talker = UVOIPStatics::GetVOIPTalkerForPlayer(UniqueNetIdRef);
	if (!Talker)
	{
		return;
	}


}

void AFHTerminalBase::SetVOIPTalker(APlayerState* PlayerState)
{
	//if (PlayerState && VOIPComponent)
	//{
	//	PRINT_LOG(TEXT("VOIP Component Set Success."));
	//	VOIPComponent->RegisterWithPlayerState(PlayerState);
	//}
}



void AFHTerminalBase::OnOffMute()
{
	bIsMute ^= 1;
	if (bIsMute)
	{
		VoiceChannelOff();
	}
	else
	{
		VoiceChannelOn();
	}
}

UVOIPTalker* AFHTerminalBase::GetVOIPTalker(APlayerState* PlayerState)
{
	UVOIPTalker* Talker = UVOIPStatics::GetVOIPTalkerForPlayer(PlayerState->GetUniqueId());
	return Talker;
}


void AFHTerminalBase::ConstructMinimapWidget(FVector _RoomUnit, const TArray<FMinimapRoomSpec>& RoomSpecs, int32 DungeonLowestFloor, int32 DungeonHighestFloor, float RoomHeight, float _GeneratorPosZ)
{
	if (OwnerPlayer->IsLocallyControlled() == false) return;

	bIsInGate = true;
	this->RoomUnit = _RoomUnit;
	
	if (MinimapWidgetInstance != nullptr)
	{
		PRINT_LOG(TEXT("Something goes wrong but the game have no choice but to keep going..."));
	}
	ensureMsgf(MinimapWidgetClass, TEXT("There is no minimap widget class."));

	MinimapWidgetInstance = CreateWidget<UFHMinimapBase>(GetWorld()->GetFirstPlayerController(), MinimapWidgetClass, TEXT("Minimap"));
	MinimapWidgetInstance->ScaleMult = MinimapScaleVersusRealWorld;

	MinimapWidgetInstance->ConstructFloors(DungeonLowestFloor, DungeonHighestFloor);

	this->FloorHeight = RoomHeight;
	this->GeneratorPosZ = _GeneratorPosZ;
	RoomDescs.Reserve(RoomSpecs.Num());

	for (const FMinimapRoomSpec& spec : RoomSpecs)
	{
		FFHMinimapRoomDescriptor desc;
		desc.Room = spec.Room;
		desc.RoomData = spec.RoomData;
		desc.MinFloor = spec.MinFloor;
		desc.MaxFloor = spec.MaxFloor;

		desc.Location = spec.Room->GetBoundsCenter();
		desc.Extension = spec.Room->GetBoundsExtent();

		switch (spec.Room->Direction)
		{
		case EDoorDirection::North:
			desc.YawAngle = 0.0f;
			break;
		case EDoorDirection::East:
			desc.YawAngle = 90.0f;
			break;
		case EDoorDirection::South:
			desc.YawAngle = 180.0f;
			break;
		case EDoorDirection::West:
			desc.YawAngle = 270.0f;
			break;
		}

		RoomDescs.Add({ spec.Room->GetRoomID(), desc });

		// Construct room icons
		for (int32 i = 0; i < desc.RoomData->RoomIconImages.Num(); ++i)
		{
			if (desc.RoomData->MinimapImages.Num() - 1 < i)
			{
				break; // room icon is only available with room texture.
			}
			if (desc.RoomData->MinimapImages[i] == nullptr)
			{
				continue; // room icon is only available with room texture.
			}

			TObjectPtr<UTexture2D> roomIcon = desc.RoomData->RoomIconImages[i];
			if (roomIcon != nullptr)
			{
				int32 roomIconFloor = desc.MinFloor + i;

				TObjectPtr<UImage> roomIconImage = MinimapWidgetInstance->AttachMinimapObjectImage(roomIcon, RoomIconSize, desc.Location, roomIconFloor);

				roomIconImage->SetVisibility(ESlateVisibility::Hidden);

				int64 RoomID = desc.Room->GetRoomID();
				if (RoomMinimapObjectMap.Contains(RoomID) == false)
				{
					RoomMinimapObjectMap.Add(RoomID);
				}
				RoomMinimapObjectMap[RoomID].Images.Add(roomIconImage);
			}
		}
	}

	DisplayWidget->SetWidget(Cast<UUserWidget>(MinimapWidgetInstance));

	DisplayMesh->SetMaterial(0, GateTerminalMaterial);

	DynamicDisplayMaterial = DisplayMesh->CreateDynamicMaterialInstance(0);
	DisplayMesh->SetMaterial(0, DynamicDisplayMaterial);

	DynamicDisplayMaterial->SetTextureParameterValue(DisplayMaterialRenderTargetTextureParameterName, DisplayWidget->GetRenderTarget());

	AttachOtherPlayerIcon();

	// Initial update
	UpdateMinimapWidget();

	GateState = Cast<AFHGateGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (GateState)
	{
		UpdateLeftTimeText(GateState->TimeLimit);
		UpdateLeftCoreText(GateState->CurrentCoreCount);

		GateState->OnTimerTickEvent.AddDynamic(this, &AFHTerminalBase::UpdateLeftTimeText);
		GateState->OnCoreCountChangedEvent.AddDynamic(this, &AFHTerminalBase::UpdateLeftCoreText);

		MinimapWidgetInstance->SetLeftTaxText(GateState->GetPlayerMoney(), GateState->GetRequiredMoney());
	}

	bIsWidgetAttached = true;
}

void AFHTerminalBase::UpdateMinimapWidget()
{
	if (!GateState) return;

	// Update Floor
	CurrentFloor = GetPlayerStandsFloor(OwnerPlayer.Get());

	// Clear render target
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), MinimapRenderTarget, MinimapTextureClearColor);

	UCanvas* Canvas{ nullptr };
	FVector2D CanvasSize{ 0, 0 };
	FDrawToRenderTargetContext DrawContext;

	FVector PlayerPos = OwnerPlayer->GetActorLocation();
	// 픽셀 오차 줄이기
	//PlayerPos.X = FMath::CeilToDouble(PlayerPos.X);
	//PlayerPos.Y = FMath::CeilToDouble(PlayerPos.Y);

	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), MinimapRenderTarget, Canvas, CanvasSize, DrawContext);

	Doors.Empty();

	TArray<FFHMinimapRoomDescriptor*> DisclosedRooms;
	for (auto& pair : RoomDescs)
	{
		int64 RoomID = pair.Key;
		FFHMinimapRoomDescriptor& Desc = pair.Value;

		if (CurrentFloor < Desc.MinFloor || CurrentFloor > Desc.MaxFloor)
		{
			continue;
		}

		int32 targetFloorIdx = CurrentFloor - Desc.MinFloor;
		if (targetFloorIdx >= Desc.RoomData->MinimapImages.Num())
		{
			continue;
		}
		if (Desc.RoomData->MinimapImages[targetFloorIdx] == nullptr)
		{
			continue;
		}

		UTexture* floorMask = Desc.RoomData->MinimapImages[targetFloorIdx];

		if (GateState->IsRoomDisclosed(RoomID))
		{
			if (DiscloseTriggeredRooms.Contains(RoomID) == false) // on first update
			{
				DiscloseTriggeredRooms.Add(RoomID);
				OnDisclose(RoomID);
			}
			DisclosedRooms.Add(&Desc);
		}
		else
		{
			DrawRoom(Canvas, floorMask, CanvasSize, Desc, PlayerPos, 1.0f);
		}
	}

	float R_Step_for_visited_rooms = 1.0f / (DisclosedRooms.Num() + 2);
	float CurrentR = R_Step_for_visited_rooms;
	for (auto& Desc : DisclosedRooms)
	{
		int32 targetFloorIdx = CurrentFloor - Desc->MinFloor;
		DrawRoom(Canvas, Desc->RoomData->MinimapImages[targetFloorIdx], CanvasSize, *Desc, PlayerPos, CurrentR);
		CurrentR += R_Step_for_visited_rooms;
	}

	DrawDoors(RoomUnit.X, Canvas, CanvasSize, PlayerPos);

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), DrawContext);

	//DisplayWidget->RequestRedraw();

	UpdateTrackingIcons(PlayerPos);
	MinimapWidgetInstance->UpdateMinimapPosition(OwnerPlayer->GetActorLocation(), OwnerPlayer->GetActorRotation(), CurrentFloor);
}

void AFHTerminalBase::AttachOtherPlayerIcon()
{
	AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(GetWorld());
	TArray<TObjectPtr<APlayerState>>& Players = GameStateBase->PlayerArray;

	for (TObjectPtr<APlayerState> PS : Players)
	{
		if (PS == nullptr || PS->GetPawn() == nullptr) continue;
		if (PS->GetPawn() == OwnerPlayer.Get()) continue;

		FFHMinimapTrackingIconDescriptor desc;
		desc.bEnabled = true;
		desc.Type = EMinimapObjectType::OtherPlayer;
		desc.TrackingActor = Cast<AActor>(PS->GetPawn());
		desc.ImageWidget = MinimapWidgetInstance->AttachOtherPlayerImage(OtherPlayerIconTexture, OtherPlayerIconSize);
		desc.bInheritRotation = true;
		TrackingIcons.Add(desc);
	}
}

void AFHTerminalBase::UpdateTrackingIcons(FVector& TerminalOwnerPos)
{
	for (FFHMinimapTrackingIconDescriptor& desc : TrackingIcons)
	{
		if (desc.bEnabled == false)
		{
			desc.ImageWidget->SetVisibility(ESlateVisibility::Hidden);
			continue;
		}
		if (desc.ImageWidget == nullptr) continue;

		if (desc.Type == EMinimapObjectType::Player || desc.Type == EMinimapObjectType::OtherPlayer)
		{
			if (desc.TrackingActor.IsValid() == false)
			{
				desc.bEnabled = false;
				continue;
			}
			AFHPlayerBase* player = Cast<AFHPlayerBase>(desc.TrackingActor);
			if (!player || player->IsDead()) {
				desc.bEnabled = false;
				continue;
			}

			int32 playerStandsFloor = GetPlayerStandsFloor(player);
			if (CurrentFloor != playerStandsFloor)
			{
				desc.ImageWidget->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				desc.ImageWidget->SetVisibility(ESlateVisibility::Visible);

				FVector playerPos = player->GetActorLocation();
				FWidgetTransform widgetTransform = desc.ImageWidget->GetRenderTransform();
				widgetTransform.Translation.X = -(TerminalOwnerPos.X - playerPos.X) * MinimapScaleVersusRealWorld;
				widgetTransform.Translation.Y = -(TerminalOwnerPos.Y - playerPos.Y) * MinimapScaleVersusRealWorld;
				widgetTransform.Angle = player->GetActorRotation().Yaw + 90.0f;
				desc.ImageWidget->SetRenderTransform(widgetTransform);
			}
			
		}
	}
}

void AFHTerminalBase::DrawRoom(UCanvas* canvas, UTexture* texture, FVector2D& canvasSize, FFHMinimapRoomDescriptor& desc, FVector& playerPos, float CurrentR)
{
	FVector2D RoomExtentOnCanvas = FVector2D{ desc.Extension.X, desc.Extension.Y } * MinimapScaleVersusRealWorld;
	FVector2D RoomSizeOnCanvas = RoomExtentOnCanvas * 2.0f;

	FVector2D RoomRelativeCenter{ desc.Location.X - playerPos.X, desc.Location.Y - playerPos.Y };
	// 픽셀 오차 줄이기
	//RoomRelativeCenter.X = FMath::CeilToDouble(RoomRelativeCenter.X);
	//RoomRelativeCenter.Y = FMath::CeilToDouble(RoomRelativeCenter.Y);
	FVector2D RoomCenterOnCanvas = (canvasSize * 0.5f) + (RoomRelativeCenter * MinimapScaleVersusRealWorld);

	float Rotation = desc.YawAngle;
	FVector2D DrawLeftTop{};
	if (Rotation == 90.0f || Rotation == 270.0f || Rotation == -90.0f || Rotation == -270.0f)
	{
		int32 targetId = desc.Room->GetRoomID();
		Swap(RoomSizeOnCanvas.X, RoomSizeOnCanvas.Y);
		DrawLeftTop.X = RoomCenterOnCanvas.X - RoomExtentOnCanvas.Y;
		DrawLeftTop.Y = RoomCenterOnCanvas.Y - RoomExtentOnCanvas.X;
	}
	else
	{
		DrawLeftTop = RoomCenterOnCanvas - RoomExtentOnCanvas;
	}

	if (DrawLeftTop.X > canvasSize.X || DrawLeftTop.Y > canvasSize.Y ||
		DrawLeftTop.X + RoomSizeOnCanvas.X < 0.0f || DrawLeftTop.Y + RoomSizeOnCanvas.Y < 0.0f)
	{
		return;
	}

	FVector2D CoordInit{ 0, 0 };
	FVector2D CoordSize{ 1, 1 };
	FLinearColor ColorMult{ CurrentR, 0.0f, 0.0f, 1.0f };
	EBlendMode BlendMode{ EBlendMode::BLEND_Translucent };

	canvas->K2_DrawTexture(texture, DrawLeftTop, RoomSizeOnCanvas, CoordInit, CoordSize, ColorMult, BlendMode, Rotation);



	// Collect door
	int32 RoomMinPosZ = desc.RoomData->FirstPoint.Z < desc.RoomData->SecondPoint.Z ? desc.RoomData->FirstPoint.Z : desc.RoomData->SecondPoint.Z;
	int32 CurrentRoomTargetFloor = CurrentFloor - desc.MinFloor;
	int32 TargetRoomPositionZ = RoomMinPosZ + CurrentRoomTargetFloor;

	int32 doorNum = desc.RoomData->GetNbDoor();
	for (int32 i = 0; i < doorNum; ++i)
	{
		const FDoorDef& doorDef = desc.Room->GetDoorDef(i);
		int32 DoorPosZ = doorDef.Position.Z;
		if (TargetRoomPositionZ == DoorPosZ)
		{
			ADoor* door = desc.Room->GetDoor(i);
			Doors.Add(door);
		}
	}
}

void AFHTerminalBase::DrawDoors(float RoomUnitX, class UCanvas* canvas, FVector2D& canvasSize, FVector& playerPos)
{
	float DoorWidth = RoomUnitX * DoorTextureWidthPerRoomUnit * MinimapScaleVersusRealWorld;
	float DoorHeight = RoomUnitX * DoorTextureHeightPerRoomUnit * MinimapScaleVersusRealWorld;

	for (ADoor* door : Doors)
	{
		if (IsValid(door) == false)
		{
			ensureMsgf(false, TEXT("Invalid door."));
			continue;
		}
		if (door->bIsWall) continue;

		FVector2D DoorSizeOnCanvas = { DoorWidth, DoorHeight };
		FVector2D DoorExtentOnCanvas = DoorSizeOnCanvas * 0.5f;

		FVector DoorLocation = door->GetActorLocation();
		// 픽셀 오차 줄이기
		//DoorLocation.X = FMath::CeilToDouble(DoorLocation.X);
		//DoorLocation.Y = FMath::CeilToDouble(DoorLocation.Y);
		FVector2D DoorRelativeCenter{ DoorLocation.X - playerPos.X, DoorLocation.Y - playerPos.Y };
		FVector2D DoorCenterOnCanvas = (canvasSize * 0.5f) + (DoorRelativeCenter * MinimapScaleVersusRealWorld);

		float DoorRotation = door->GetActorRotation().Yaw;
		FVector2D DoorDrawLeftTop{};
		if (DoorRotation == 90.0f || DoorRotation == 270.0f || DoorRotation == -90.0f || DoorRotation == -270.0f)
		{
			// 왜 회전하면 방향이 틀어지는지?
			DoorRotation = 0.0f;
			DoorDrawLeftTop = DoorCenterOnCanvas - DoorExtentOnCanvas;
		}
		else
		{
			Swap(DoorSizeOnCanvas.X, DoorSizeOnCanvas.Y);
			DoorDrawLeftTop.X = DoorCenterOnCanvas.X - DoorExtentOnCanvas.Y;
			DoorDrawLeftTop.Y = DoorCenterOnCanvas.Y - DoorExtentOnCanvas.X;
		}

		if (DoorDrawLeftTop.X > canvasSize.X || DoorDrawLeftTop.Y > canvasSize.Y ||
			DoorDrawLeftTop.X + DoorSizeOnCanvas.X < 0.0f || DoorDrawLeftTop.Y + DoorSizeOnCanvas.Y < 0.0f)
		{
			continue;
		}

		FVector2D DoorCoordInit{ 0, 0 };
		FVector2D DoorCoordSize{ 1, 1 };
		FLinearColor DoorColorMult{ 0.0f, 1.0f, 0.0f, 1.0f };
		EBlendMode DoorBlendMode{ EBlendMode::BLEND_Additive };

		canvas->K2_DrawTexture(MinimapDoorTexture, DoorDrawLeftTop, DoorSizeOnCanvas, DoorCoordInit, DoorCoordSize, DoorColorMult, DoorBlendMode, DoorRotation);
	}
}



void AFHTerminalBase::UpdateLeftTimeText(int32 LeftTime)
{
	if (!MinimapWidgetInstance) return;
	if (!GateState) return;

	//FString text = FString::Printf(TEXT("Time Left %s"), *GateState->GetModifiedStringFromTimeLimit());
	FText text = UFHBlueprintFunctionLibrary::GetLocalizeText(TEXT("OBS_HUD_TIME_REMAINING"));
	FString ModifiedTime = GateState->GetModifiedStringFromTimeLimit();
	MinimapWidgetInstance->SetTimeLimitText(text, ModifiedTime);
}

void AFHTerminalBase::UpdateLeftCoreText(int32 LeftCore)
{
	if (!MinimapWidgetInstance) return;
	if (!GateState) return;

	//FString text = FString::Printf(TEXT("Core Left %d"), GateState->GetCurrentCoreCount());
	FText text = UFHBlueprintFunctionLibrary::GetLocalizeText(TEXT("OBS_HUD_CORE_LIVE"));
	MinimapWidgetInstance->SetLeftCoreText(text, LeftCore);
}



void AFHTerminalBase::OnDisclose(int64 RoomID)
{
	if (RoomMinimapObjectMap.Contains(RoomID) == false) return;

	// show icon widgets inside the room
	for (TObjectPtr<UImage> widget : RoomMinimapObjectMap[RoomID].Images)
	{
		widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AFHTerminalBase::ShutOffTerminal()
{
	if (OwnerPlayer.IsValid())
	{
		OwnerPlayer->ServerRPC_ToggleFlashlight(false);
		OwnerPlayer->bCanToggleFlashlight = false;
		OwnerPlayer->bIsFlashlightOn = false;
	}

	OwnerPlayer = nullptr;
	bUpdateMinimap = false;
	bUpdateBattery = false;

	Server_TurnOffTerminalDisplay();
}

void AFHTerminalBase::Server_TurnOffTerminalDisplay_Implementation()
{
	Multicast_TurnOffTerminalDisplay();
}

void AFHTerminalBase::Multicast_TurnOffTerminalDisplay_Implementation()
{
	if (ShutOffMaterial)
	{
		DisplayMesh->SetMaterial(0, ShutOffMaterial);
	}
}

int32 AFHTerminalBase::GetPlayerStandsFloor(AFHPlayerBase* player)
{
	double PlayerFootZ = player->GetActorLocation().Z + epsilon;
	int32 _curFloor = static_cast<int32>(PlayerFootZ / FloorHeight);
	if (PlayerFootZ < 0.0f) _curFloor -= 1;
	return _curFloor;
}

void AFHTerminalBase::UpdateBattery()
{
	if (BatteryReduceIntervalSecond != NextBatteryReduceIntervalSecond)
	{
		BatteryReduceIntervalSecond = NextBatteryReduceIntervalSecond;
	}

	CurrentBattery -= BatteryReduceAmountForEachCycle;
	if (CurrentBattery <= 0.0f)
	{
		ShutOffTerminal();
		CurrentBattery = 0.0f;
	}
	
	MinimapWidgetInstance->SetLeftBatteryText(InitialBattery, CurrentBattery);

	PRINT_LOG(TEXT("%f"), CurrentBattery);
}

//
//void AFHTerminalBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AFHTerminalBase, bIsInGate);
//}
