// Copyright F Rank Hunter. All Rights Reserved.


#include "FHPlayerBase.h"
#include "FRankHunter.h"
#include "UI/FHStatusBase.h"
#include "Net/UnrealNetwork.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/FHCharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "FHPlayerAnimInstance.h"
#include "Input/InputConfigData.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Core/FHPlayerController.h"
#include "Core/FHPlayerStateBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item/FHInventoryComponent.h"
#include "Component/FHInteractableComponent.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "Camera/PlayerCameraManager.h"
#include "Data/FHSkillTable.h"

// GAS
#include "AbilitySystemComponent.h"
#include "GAS/FHGameplayAbility.h"
#include "GameplayEffect.h"
#include "GAS/FHAttributeSet_Health.h"
#include "GAS/FHAttributeSet_Stamina.h"
#include "GAS/Attributes/FHAttributeSet_Movement.h"
#include "GAS/FHAttributeSet_PlayerStatus.h"

#include "GameFramework/HUD.h"
#include "UI/FHPlayerHUDBase.h"
#include "UI/ToolTip/FHInteractTooltipWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/VoiceConfig.h"
#include "Props/FHCart.h"

#include "Item/FHItemBase.h"
#include "Item/FHItemDropPointComponent.h"
#include "Item/Actors/FHBackpack.h"
#include "DataAsset/FHPlayerAnimMontageDataAsset.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "Components/ChildActorComponent.h"
#include "GroomComponent.h"
#include "Components/WidgetComponent.h"

#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "GAS/FHGameplayAbilityTargetDatas.h"
#include "Props/FHTerminalBase.h"
#include "Materials/MaterialParameterCollection.h"
#include "Core/FHGateGameModeBase.h"
#include "Core/FHGateGameStateBase.h"
#include "Creature/FHCreatureBase.h"
#include "Component/FHCloakingComponent.h"

// OSS
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/VoiceInterface.h"

// Sounds
#include "Core/FHSoundManagerActor.h"

// VFX
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraActor.h"
#include "Core/FHVignetteEffectManager.h"

#include "CommonActivatableWidget.h"
#include "DataAsset/FHSelectInfoIconDataAsset.h"


// Sets default values
AFHPlayerBase::AFHPlayerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFHCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;


	// Third person mesh (other player view)
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	USkeletalMeshComponent* BodyMesh = GetMesh();
	//BodyMesh->SetIsReplicated(true); // 이거 필요해?
	BodyMesh->SetRelativeLocation(FVector(0, 0, -CapsuleHalfHeight));
	BodyMesh->SetRelativeRotation(FRotator{0, -90, 0});

	// Metahumans
	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	FaceMesh->SetupAttachment(BodyMesh);
	//FaceMesh->SetLeaderPoseComponent(BodyMesh);
	FaceMesh->bOwnerNoSee = true;
	FaceMesh->bCastHiddenShadow = true;

	TorsoMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Torso"));
	TorsoMesh->SetupAttachment(BodyMesh);
	TorsoMesh->SetLeaderPoseComponent(BodyMesh);

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	LegsMesh->SetupAttachment(BodyMesh);
	LegsMesh->SetLeaderPoseComponent(BodyMesh);

	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	FeetMesh->SetupAttachment(BodyMesh);
	FeetMesh->SetLeaderPoseComponent(BodyMesh);

	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Backpack"));
	BackpackMesh->SetupAttachment(BodyMesh);
	BackpackMesh->SetLeaderPoseComponent(BodyMesh);
	

	// 털은 실시간으로 생성해서 부착 (그냥 템플릿으로 생성하는 비용보다 프로퍼티 순회하면서 복사하는 비용이 더 나올 거 같음)




	// camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(BodyMesh, "head");
	CameraBoom->TargetArmLength = 0.f;
	CameraBoom->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraBoom);

	// Flashlight
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	Flashlight->SetIntensity(10000.f);
	Flashlight->SetInnerConeAngle(20);
	Flashlight->SetAttenuationRadius(1000.f);
	Flashlight->SetVisibility(false);

	// face light
	Facelight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	Facelight->SetAttenuationRadius(300.f);
	Facelight->SetIntensity(600.0f);

	Facelight->AttachToComponent(FaceMesh, FAttachmentTransformRules::KeepRelativeTransform);

	//AttachLightsAtCamera();


	GrabMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TPGrabMesh"));
	GrabMesh->SetupAttachment(BodyMesh, RightGrabToolSocketName);
	GrabMesh->SetCollisionProfileName(TEXT("NoCollision"), false);


	//VOIPComponent = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalker"));
	//LastCachedVolume = 0.0f;
	//VOIPComponent->Settings.ComponentToAttachTo = BodyMesh;
	//VOIPComponent->SetIsReplicated(true);

	ItemDropPointComp = CreateDefaultSubobject<UFHItemDropPointSocketComponent>(TEXT("ItemDropPoint"));
	ItemDropPointComp->SetHandleComponent(GetMesh());


	MovementComponent = Cast<UFHCharacterMovementComponent>(GetCharacterMovement());
	MovementComponent->CartDriveSpeed = 600.0f;
	MovementComponent->CartDriveSteerThreathHold = 100.0f;
	MovementComponent->CartDriveMaxSteerAngle = 180.0f;
	MovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;


	NameTagWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTagComponent"));
	NameTagWidgetComponent->SetupAttachment(BodyMesh);
	NameTagWidgetComponent->SetRelativeLocation(FVector(0, 0, 200));


	TerminalChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("TerminalChildActor"));
	TerminalChildActor->SetupAttachment(BodyMesh, RightGrabToolSocketName);

	Flashlight->AttachToComponent(TerminalChildActor, FAttachmentTransformRules::KeepRelativeTransform);


	TwoPassMesh_Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TwoPassMesh_Torso"));
	TwoPassMesh_Torso->SetupAttachment(GetMesh());
	TwoPassMesh_Torso->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TwoPassMesh_Torso->CastShadow = false;
	TwoPassMesh_Torso->SetIsReplicated(true);
	TwoPassMesh_Torso->SetVisibility(false);

	TwoPassMesh_Face = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TwoPassMesh_Face"));
	TwoPassMesh_Face->SetupAttachment(GetMesh());
	TwoPassMesh_Face->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TwoPassMesh_Face->CastShadow = false;
	TwoPassMesh_Face->SetIsReplicated(true);
	TwoPassMesh_Face->SetVisibility(false);

	TwoPassMesh_Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TwoPassMesh_Legs"));
	TwoPassMesh_Legs->SetupAttachment(GetMesh());
	TwoPassMesh_Legs->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TwoPassMesh_Legs->CastShadow = false;
	TwoPassMesh_Legs->SetIsReplicated(true);
	TwoPassMesh_Legs->SetVisibility(false);

	TwoPassMesh_Feet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TwoPassMesh_Feet"));
	TwoPassMesh_Feet->SetupAttachment(GetMesh());
	TwoPassMesh_Feet->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TwoPassMesh_Feet->CastShadow = false;
	TwoPassMesh_Feet->SetIsReplicated(true);
	TwoPassMesh_Feet->SetVisibility(false);

	CloakingComponent = CreateDefaultSubobject<UFHCloakingComponent>(TEXT("CloakingComponent"));
}

// Called when the game starts or when spawned
void AFHPlayerBase::BeginPlay()
{
	NET_DEBUG_LOG(TEXT("Before Super"));

	Super::BeginPlay();
	
	NET_DEBUG_LOG(TEXT("After Super"));

	if (IsLocallyControlled())
	{
		GetMesh()->SetAnimInstanceClass(PlayerDescriptor->FirstPersonAnimInstance);
	}
	else
	{
		GetMesh()->SetAnimInstanceClass(PlayerDescriptor->ThirdPersonAnimInstance);
		Facelight->SetIntensity(0.0f);
	}

	NameTagWidgetComponent->SetVisibility(true);
	GetMesh()->SetVisibility(true);

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		if (CameraManager)
		{
			CameraManager->ViewPitchMin = -PlayerDescriptor->PitchLimitAngle;
			CameraManager->ViewPitchMax = PlayerDescriptor->PitchLimitAngle;
		}
	}
	
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			PlayerAnimInstance = Cast<UFHPlayerAnimInstance>(AnimInstance);
		}
	}

	IsComfortTag = GET_GAMEPLAY_TAG_PLAYER_STATE_ISCOMFORT;


	//AFHPlayerStateBase* FHPS = Cast<AFHPlayerStateBase>(GetPlayerState());

	// host load skill
	// Not Use
	//if (FHPS)
	//{
	//	PRINT_LOG(TEXT("Host Load Skill Hud."));
	//	FHPS->UpdateSkillWidget();
	//}
	// Host Regist Talker.
	//if (FHPS)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("[Pawn::BeginPlay] PlayerState Is Valid."));
	//	FHPS->InitVoiceSetting();
	//}


	//if (GetPlayerState() && VOIPComponent)
	//{
	//	PRINT_LOG(TEXT("[Pawn::BeginPlay] VOIP Component Add Success."));
	//	VOIPComponent->RegisterWithPlayerState(GetPlayerState());
	//}
	//AFHTerminalBase* Terminal = Cast<AFHTerminalBase>(TerminalChildActor->GetChildActor());
	//Terminal->OwnerPlayer = this;


	DefaultThirdPersonMeshRelativeLocation = GetMesh()->GetRelativeLocation();
	DefaultThirdPersonMeshRelativeRotation = GetMesh()->GetRelativeRotation();





	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			InitEquipmentStateTimer,
			this,
			&AFHPlayerBase::InitializeEquipmentState,
			0.1f,
			false   // 반복 여부(false는 한 번만 실행)
		);
	}
	//ServerRPC_ToggleFlashlight(true);
}

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void AFHPlayerBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	bool isdestroyed = EndPlayReason == EEndPlayReason::Destroyed;
	bool isGate = UFHBlueprintFunctionLibrary::IsGate(this);
	bool IsAuthority = HasAuthority();
	if (isdestroyed && isGate && IsAuthority)
	{
		//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, GET_GAMEPLAY_TAG_GAMEPLAYEVENT_DEATH, {});
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);

	if (StunEffectTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(StunEffectTimerHandle);
	}

	if (DissolveTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DissolveTimerHandle);
	}

}

void AFHPlayerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 레벨 전환할 때 카메라가 캡슐 정중앙으로 가는 문제 예방용
	CameraBoom->SetupAttachment(GetMesh(), "head");
}

// Called every frame
void AFHPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (HasAuthority())
	{
		UpdateAimPitch();
		UpdateComfortState(DeltaTime);
	}

	if (IsLocallyControlled())
	{
		UpdateCurrentLookingInteractableActor();

		if (FocusingPhase != EPlayerFocusingPhase::None)
		{
			UpdateFocusingState(DeltaTime);
		}

		CheckNearbyInteractableObjects(DeltaTime);
	}
	else
	{
		UpdateNameTagRotation();
	}

	// ControlRotation은 클라이언트 자신과 서버에서만 유효
	if (HasAuthority() || IsLocallyControlled())
	{
		ControlRotation = GetControlRotation();
		Flashlight->SetWorldRotation(ControlRotation);
	}
	else
	{
		Flashlight->SetWorldRotation(ControlRotation);
	}

	// Voice Test
	//if (!GetLocalRole() == ROLE_SimulatedProxy && VOIPComponent)
	//{
	//	float CachedVolume = VOIPComponent->GetVoiceLevel();
	//	if (CachedVolume != LastCachedVolume)
	//	{
	//		PRINT_LOG(TEXT("Voice Level Changed. Prev: %f, Now: %f"), LastCachedVolume, CachedVolume);
	//		LastCachedVolume = CachedVolume;
	//	}
	//}

	if (bIsDissolving)
	{
		DissolveTimeLine.TickTimeline(DeltaTime);
	}
}

// Called to bind functionality to input
void AFHPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
	{
		return;
	}

	UInputConfigData* InputActions = PlayerDescriptor->InputActions;
	//Subsystem->ClearAllMappings();

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AFHPlayerBase::Move);
	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Completed, this, &AFHPlayerBase::Move);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AFHPlayerBase::Look);

	//시간 되면 배열로
	PEI->BindAction(InputActions->InputInteract1, ETriggerEvent::Started, this, &AFHPlayerBase::Interact);
	PEI->BindAction(InputActions->InputInteract1, ETriggerEvent::Ongoing, this, &AFHPlayerBase::InteractHold);
	PEI->BindAction(InputActions->InputInteract1, ETriggerEvent::Completed, this, &AFHPlayerBase::InteractReleased);

	PEI->BindAction(InputActions->InputInteract2, ETriggerEvent::Started, this, &AFHPlayerBase::Interact);
	PEI->BindAction(InputActions->InputInteract2, ETriggerEvent::Ongoing, this, &AFHPlayerBase::InteractHold);
	PEI->BindAction(InputActions->InputInteract2, ETriggerEvent::Completed, this, &AFHPlayerBase::InteractReleased);

	PEI->BindAction(InputActions->InputDropBackpack, ETriggerEvent::Started, this, &AFHPlayerBase::DropBackpackStart);
	PEI->BindAction(InputActions->InputDropBackpack, ETriggerEvent::Ongoing, this, &AFHPlayerBase::DropBackpack);
	PEI->BindAction(InputActions->InputDropBackpack, ETriggerEvent::Completed, this, &AFHPlayerBase::DropBackpackEnd);

	PEI->BindAction(InputActions->InputToggleFlashlight, ETriggerEvent::Started, this, &AFHPlayerBase::ToggleFlashlight);

	PEI->BindAction(InputActions->InputSwitchSlot1, ETriggerEvent::Triggered, this, &AFHPlayerBase::SwitchItemByInput, 1);
	PEI->BindAction(InputActions->InputSwitchSlot2, ETriggerEvent::Triggered, this, &AFHPlayerBase::SwitchItemByInput, 2);
	PEI->BindAction(InputActions->InputSwitchSlot3, ETriggerEvent::Triggered, this, &AFHPlayerBase::SwitchItemByInput, 3);
	PEI->BindAction(InputActions->InputSwitchSlot4, ETriggerEvent::Triggered, this, &AFHPlayerBase::SwitchItemByInput, 4);
	PEI->BindAction(InputActions->InputScrollQuickSlot, ETriggerEvent::Triggered, this, &ThisClass::ScrollQuickSlot);
	
	PEI->BindAction(InputActions->InputDropItem, ETriggerEvent::Started, this, &ThisClass::AbilityInputPressed, (int32)EFHPlayerAbilityInputID::DropItem);
	PEI->BindAction(InputActions->InputDropItem, ETriggerEvent::Canceled, this, &ThisClass::AbilityInputReleased, (int32)EFHPlayerAbilityInputID::DropItem);
	PEI->BindAction(InputActions->InputDropItem, ETriggerEvent::Completed, this, &ThisClass::AbilityInputReleased, (int32)EFHPlayerAbilityInputID::DropItem);

	PEI->BindAction(InputActions->InputClick, ETriggerEvent::Started, this, &ThisClass::AbilityInputPressed, (int32)EFHPlayerAbilityInputID::PrePareUseItem);
	PEI->BindAction(InputActions->InputClick, ETriggerEvent::Canceled, this, &ThisClass::AbilityInputReleased, (int32)EFHPlayerAbilityInputID::PrePareUseItem);
	PEI->BindAction(InputActions->InputClick, ETriggerEvent::Completed, this, &ThisClass::AbilityInputReleased, (int32)EFHPlayerAbilityInputID::PrePareUseItem);

	PEI->BindAction(InputActions->InputClick, ETriggerEvent::Started, this, &ThisClass::AbilityInputPressed, (int32)EFHPlayerAbilityInputID::ToggleSpeak);
	PEI->BindAction(InputActions->InputClick, ETriggerEvent::Canceled, this, &ThisClass::AbilityInputReleased, (int32)EFHPlayerAbilityInputID::ToggleSpeak);
	PEI->BindAction(InputActions->InputClick, ETriggerEvent::Completed, this, &ThisClass::AbilityInputReleased, (int32)EFHPlayerAbilityInputID::ToggleSpeak);

	PEI->BindAction(InputActions->InputClick, ETriggerEvent::Completed, this, &ThisClass::AbilityInputPressed, (int32)EFHPlayerAbilityInputID::UseItem);

	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Started, this, &AFHPlayerBase::StartCrouch, static_cast<int32>(EFHPlayerAbilityInputID::Crouch));
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Completed, this, &AFHPlayerBase::EndCrouch, static_cast<int32>(EFHPlayerAbilityInputID::Crouch));
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Started, this, &AFHPlayerBase::StartJump, static_cast<int32>(EFHPlayerAbilityInputID::Jump));
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Completed, this, &AFHPlayerBase::EndJump, static_cast<int32>(EFHPlayerAbilityInputID::Jump));
	PEI->BindAction(InputActions->InputRun, ETriggerEvent::Started, this, &AFHPlayerBase::StartSprint, static_cast<int32>(EFHPlayerAbilityInputID::Sprint));
	PEI->BindAction(InputActions->InputRun, ETriggerEvent::Completed, this, &AFHPlayerBase::EndSprint, static_cast<int32>(EFHPlayerAbilityInputID::Sprint));

	PEI->BindAction(InputActions->InputToggleTerminal, ETriggerEvent::Started, this, &AFHPlayerBase::ToggleTerminal);

	PEI->BindAction(InputActions->InputUseSkill, ETriggerEvent::Started, this, &ThisClass::UseSkill);
	PEI->BindAction(InputActions->InputPopupStatus, ETriggerEvent::Started, this, &ThisClass::PopUpStatus);
	//PEI->BindAction(InputActions->InputUseSkill, ETriggerEvent::Canceled, this, &ThisClass::UseSkill);
	//PEI->BindAction(InputActions->InputUseSkill, ETriggerEvent::Completed, this, &ThisClass::UseSkill);

	PEI->BindAction(InputActions->InputCloaking, ETriggerEvent::Started, this, &ThisClass::AbilityInputPressed, (int32)EFHPlayerAbilityInputID::Cloaking);


	PEI->BindAction(InputActions->InputEmote1, ETriggerEvent::Started, this, &AFHPlayerBase::PlayEmote, EPlayerEmoteType::Clapping);
	PEI->BindAction(InputActions->InputEmote2, ETriggerEvent::Started, this, &AFHPlayerBase::PlayEmote, EPlayerEmoteType::Dance_GangnamStyle);
	PEI->BindAction(InputActions->InputEmote3, ETriggerEvent::Started, this, &AFHPlayerBase::PlayEmote, EPlayerEmoteType::Pointing);
	PEI->BindAction(InputActions->InputEmote4, ETriggerEvent::Started, this, &AFHPlayerBase::PlayEmote, EPlayerEmoteType::Salute);
	PEI->BindAction(InputActions->InputEmote5, ETriggerEvent::Started, this, &AFHPlayerBase::PlayEmote, EPlayerEmoteType::Dance_SodaPop);



	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (LevelName != FString("Map_InGate"))
	{
		AddInputMappingContext();
	}
}

void AFHPlayerBase::AddInputMappingContext_Implementation()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	ensureMsgf(PlayerController, TEXT("PlayerController Missing!!!"));
	if (PlayerController == nullptr)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	UInputMappingContext* InputMapping = PlayerDescriptor->InputMapping;
	CurrentIMC = InputMapping;
	Subsystem->AddMappingContext(InputMapping, 1);
}

void AFHPlayerBase::RemoveInputMappingContext_Implementation()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	ensureMsgf(PlayerController, TEXT("PlayerController Missing!!!"));
	if (PlayerController == nullptr)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	UInputMappingContext* InputMapping = PlayerDescriptor->InputMapping;
	Subsystem->RemoveMappingContext(InputMapping);
}

void AFHPlayerBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if (GetCharacterMovement()->CustomMovementMode == CMOVE_CartDrive)
	{
		//bUseControllerRotationYaw = false;
	}
	else if(PreviousCustomMode == CMOVE_CartDrive)
	{
		//bUseControllerRotationYaw = true;
		GetController()->SetControlRotation(GetActorRotation());
	}
}

USiInventoryComponent* AFHPlayerBase::GetInventoryComponent() const
{
	return UFHBlueprintFunctionLibrary::GetInventoryComponent(GetPlayerState());
}

FGenericTeamId AFHPlayerBase::GetGenericTeamId() const
{
	return bIsDie ? FGenericTeamId() : FGenericTeamId(uint8(PlayerDescriptor->TeamID));
}

UObject* AFHPlayerBase::FindAssetData_Implementation(FName VariationId) const
{
	if (IsValid(CurrentCharacterData) && CurrentCharacterData->ImplementsInterface(UHasCharacterVariation::StaticClass()))
	{
		return IHasCharacterVariation::Execute_FindAssetData(CurrentCharacterData->GetDefaultObject(), VariationId);
	}

	return nullptr;
}

void AFHPlayerBase::ChangeSuitColor(FLinearColor Color, float Multiplier /*= 0.1f*/)
{
	if (!TorsoMesh) return;

	

	UMaterialInterface* Mat = TorsoMesh->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMat = Cast<UMaterialInstanceDynamic>(Mat);
	if (DynamicMat)
	{
		DynamicMat->SetVectorParameterValue(TEXT("SuitColor"), Color);
		DynamicMat->SetScalarParameterValue(TEXT("SuitColorMultiplier"), Multiplier);
	}
}

bool AFHPlayerBase::IsDead()
{
	/*return abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISDEAD);*/
	return bIsDie;
}

bool AFHPlayerBase::IsObserving()
{
	return bIsObserving;
}

bool AFHPlayerBase::GetIsSprinting()
{
	if (!MovementComponent) return false;
	
	return MovementComponent->IsSprinting();
}

void AFHPlayerBase::Server_PlayMontage_Implementation(UAnimMontage* Montage, FName SectionName, float PlayRate, bool ExceptLocalPlayer /*= true*/)
{
	Multicast_PlayMontage(Montage, SectionName, PlayRate, ExceptLocalPlayer);
}

void AFHPlayerBase::Multicast_PlayMontage_Implementation(UAnimMontage* Montage, FName SectionName, float PlayRate, bool ExceptLocalPlayer /*= true*/)
{
	if (ExceptLocalPlayer == true && IsLocallyControlled())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (Montage == nullptr)
	{
		AnimInstance->Montage_Stop(0.0f);
		return;
	}

	AnimInstance->Montage_Play(Montage, PlayRate);
	if (SectionName != "")
	{
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void AFHPlayerBase::AbilityInputPressed(int32 KeyID)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	if (ASC && KeyID == static_cast<int32>(EFHPlayerAbilityInputID::PrePareUseItem))
	{
		Server_SetIsClicked(true);

		if (abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISLOOKTERMINAL))
		{
			return;
		}

		if (abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISACTIONOCCUPIED))
		{
			return;
		}

		if (CurrentEquipment.MeshType == EItemHoldingType::NoHanded)
		{
			UFHInventoryComponent* InventoryComp = Cast<UFHInventoryComponent>(GetInventoryComponent());
			if (InventoryComp && InventoryComp->GetCurrentItem() == nullptr)
			{
				ASC->AbilityLocalInputPressed(static_cast<int32>(EFHPlayerAbilityInputID::PunchAttack));
				return;
			}
		}
	}

	if (ASC)
	{
		ASC->AbilityLocalInputPressed(KeyID);
	}

	if (bIsTerminalOpen)
	{

	}
	else
	{
		// TODO: 터미널을 통해 송신 기능 열기.
		// Terminal->StartTalk(); 임시 함수명.
		// 임시 코드 이식
		//IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
		//FName SubsystemName = OSS->GetSubsystemName();
		//IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface(GetWorld(), SubsystemName);
		//OnlineVoicePtr->StartNetworkedVoice(0);
	}
}

void AFHPlayerBase::AbilityInputReleased(int32 KeyID)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	if (ASC && KeyID == static_cast<int32>(EFHPlayerAbilityInputID::PrePareUseItem))
	{
		Server_SetIsClicked(false);

		if (abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISLOOKTERMINAL))
		{
			return;
		}
		if (CurrentEquipment.MeshType == EItemHoldingType::NoHanded)
		{
			return;
		}
	}

	if (ASC)
	{
		ASC->AbilityLocalInputReleased(KeyID);
	}

	if (bIsTerminalOpen)
	{
		//IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
		//FName SubsystemName = OSS->GetSubsystemName();
		//IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface(GetWorld(), SubsystemName);
		//OnlineVoicePtr->StopNetworkedVoice(0);
	}
	else
	{
		// TODO: 터미널을 통해 송신 기능 닫기.
		// Terminal->StopTalk(); 임시 함수명.
		// 임시 코드 이식

	}
}

float AFHPlayerBase::GetMovementSpeedScale(const FVector2D& MoveValueNormalized, float MoveForwardScale, float MoveSideScale, float MoveBackwardScale)
{
	const float ForwardAmount = MoveValueNormalized.Y;

	if (ForwardAmount >= 0.0f)
	{
		return FMath::Lerp(MoveSideScale, MoveForwardScale, ForwardAmount);
	}
	else
	{
		return FMath::Lerp(MoveSideScale, MoveBackwardScale, FMath::Abs(ForwardAmount));
	}
}

void AFHPlayerBase::Move(const FInputActionValue& Value)
{
	if (!abilitySystem)
	{
		return;
	}

	if (abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISUNMOVABLE))
	{
		return;
	}

	if (Controller == nullptr)
		return;
	/*if (GetCharacterMovement()->IsFalling())
		return;*/

	if (MovementComponent == nullptr)
	{
		MovementComponent = Cast<UFHCharacterMovementComponent>(GetMovementComponent());
	}



	FGameplayAbilitySpec* Spec = abilitySystem->FindAbilitySpecFromClass(PlayerDescriptor->MoveGameplayAbility);
	if (Value.IsNonZero())
	{
		if (Spec && !Spec->IsActive())
		{
			abilitySystem->TryActivateAbility(Spec->Handle);
		}
	}
	else
	{
		if (Spec && Spec->IsActive())
		{
			abilitySystem->AbilitySpecInputReleased(*Spec);
		}
	}



	FVector2D MoveValue = Value.Get<FVector2D>();
	const float InputMagnitude = FMath::Min(MoveValue.Size(), 1.0f);
	if (MoveValue.IsNearlyZero()) return;
	FVector2D MoveValueNormalized = MoveValue.GetSafeNormal();

	const float SpeedScale = GetMovementSpeedScale(MoveValueNormalized, 1.0f, PlayerDescriptor->SideStepSpeedMult, PlayerDescriptor->BackStepSpeedMult);

	// 월드 공간 기준
	const FRotator MovementRotation(0, GetActorRotation().Yaw, 0);
	const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
	const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

	// MoveValue 크기는 유지하며 방향을 구하고 정규화하여 최종 방향 벡터를 구하고 거기에 인풋 강도와 방향 보정치를 곱함
	FVector DesiredDirection = ForwardDirection * MoveValue.Y + RightDirection * MoveValue.X;
	DesiredDirection.Normalize();

	MovementComponent->AddInputVector(DesiredDirection * InputMagnitude * SpeedScale);


	/*{
		const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(Direction, MoveValue.Y);
	}

	{
		const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(Direction, MoveValue.X);
	}*/

	if (Cart)
	{
		Cart->Move(Value);
	}
	else 
	{
	}
}

void AFHPlayerBase::Look(const FInputActionValue& Value)
{
	if (Controller == nullptr)
		return;


	if (abilitySystem && abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISCAMERALOCK))
	{
		return;
	}


	const FVector2D LookValue = Value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(-LookValue.Y);
}

void AFHPlayerBase::StartJump(int32 InputID)
{
	if (!abilitySystem) return;

	FGameplayAbilitySpec* abilitySpec = abilitySystem->FindAbilitySpecFromInputID(InputID);
	if (!abilitySpec) return;

	//abilitySpec->InputPressed = true;
	if (abilitySpec->IsActive())
	{
		//abilitySystem->AbilitySpecInputPressed(*abilitySpec);
	}
	else
	{
		abilitySystem->TryActivateAbility(abilitySpec->Handle);
	}
}

void AFHPlayerBase::EndJump(int32 InputID)
{
	if (!abilitySystem) return;

	FGameplayAbilitySpec* abilitySpec = abilitySystem->FindAbilitySpecFromInputID(InputID);
	if (!abilitySpec) return;

	//abilitySpec->InputPressed = false;
	if (abilitySpec->IsActive())
	{
		abilitySystem->AbilitySpecInputReleased(*abilitySpec);
	}
}

void AFHPlayerBase::Attack(int32 InputID)
{
	//if (!abilitySystem) return;

	//FGameplayAbilitySpec* abilitySpec = abilitySystem->FindAbilitySpecFromInputID(InputID);
	//if (!abilitySpec) return;

	//if (abilitySpec->IsActive())
	//{
	//	//abilitySystem->AbilitySpecInputPressed(*abilitySpec);
	//}
	//else
	//{
	//	abilitySystem->TryActivateAbility(abilitySpec->Handle);
	//}
	AController* _Controller = GetController();
	if (_Controller)
	{
		UFHInventoryComponent* InventoryComp = _Controller->GetComponentByClass<UFHInventoryComponent>();
		if (InventoryComp)
		{
			int32 ItemIndex = InventoryComp->GetCurrentItemIndex();
			switch (InputID)
			{
			case 1:
				//InventoryComp->Server_PrePareUseItem(ItemIndex);
				break;

			case 2:
				//InventoryComp->Server_UseItem(ItemIndex);

				break;
			default:
				break;
			}
		}
	}
}

void AFHPlayerBase::UseSkill()
{
	if (!SkillSubclass)
	{
		const UFHFRankHunterSettings* Settings = GetDefault<UFHFRankHunterSettings>();
		TSoftObjectPtr<UDataTable> SkillTable = Settings->SkillDataTable;

		FName SkillID = Cast<AFHPlayerStateBase>(GetPlayerState())->GetSkillID();

		FFHSkillTable* SkillTableItem = SkillTable->FindRow<FFHSkillTable>(SkillID, TEXT("SkillTable"));
		if (!SkillTableItem)
		{
			return;
		}

		TSubclassOf<UGameplayAbility> Skill = SkillTableItem->SkillClass;

		SkillSubclass = Skill;
	}

	abilitySystem->TryActivateAbilityByClass(SkillSubclass);
}

void AFHPlayerBase::PopUpStatus()
{
	AFHPlayerController* FHPlayerController = GetController<AFHPlayerController>();
	if (FHPlayerController)
	{
		UFHStatusScreenBase* StatusUI = Cast<UFHStatusScreenBase>(FHPlayerController->StatusUIClassInstance);
		if (StatusUI)
		{

			StatusUI->ToggleUI();
		}
	}
}

void AFHPlayerBase::FellOutOfWorld(const class UDamageType& dmgType)
{
	// Super 호출 금지

	FGameplayEventData eventData;
	eventData.EventMagnitude = FLT_MAX;
	GetAbilitySystemComponent()->HandleGameplayEvent(GET_GAMEPLAY_TAG_GAMEPLAYEVENT_HIT, &eventData);
}

void AFHPlayerBase::InitInteractState()
{
	ElapsedTimeForHoldingInteract = 0.0f;
	HoldingInteractTarget = nullptr;
	HoldingInteractTriggered = false;
}

void AFHPlayerBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (!PlayerDescriptor->bEnableFallingDamage) return;

	FVector Velocity = GetVelocity();
	if (Velocity.Z >= -PlayerDescriptor->MinFallingDamageVelocity) return;

	float damage = FMath::Pow(-Velocity.Z / PlayerDescriptor->MinFallingDamageVelocity, PlayerDescriptor->FallingDamageExponent) * PlayerDescriptor->FallingDamagePerScore;

	if (damage > 0.0f)
	{
		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		SoundManager->PlaySoundAtLocationLocallyByTag(PlayerDescriptor->FallingDamageSoundTag, GetActorLocation());
	}

	FGameplayEventData eventData;
	eventData.EventMagnitude = damage;
	GetAbilitySystemComponent()->HandleGameplayEvent(GET_GAMEPLAY_TAG_GAMEPLAYEVENT_HIT, &eventData);
}

void AFHPlayerBase::RemoveInteractTooltipWidgetOnUnposess_Implementation()
{
	DetachWidgetFromViewportAndInitCurrentLookInteractableComp();
}

void AFHPlayerBase::SetAllowInteract(bool allowInteract)
{
	bAllowInteract = allowInteract;
}

void AFHPlayerBase::Interact(const FInputActionInstance& InputActionInstance)
{
	/*if (bIsTerminalOpen)
	{
		AFHTerminalBase* Terminal = GetTerminalActor();
		if (Terminal)
		{
			Terminal->OnOffMute();
			UKismetSystemLibrary::PrintString(this, TEXT("Terminal Mute On Off."));
		}
		return;
	}*/

	if (!CurrentLookInteractableComp)
	{
		CurrentLookInteractableComp = nullptr;
		InitInteractState();
		return;
	}

	if (bAllowInteract == false) return;

	if (abilitySystem && abilitySystem->HasAnyMatchingGameplayTags(PlayerDescriptor->BlockInteractTags))
	{
		return;
	}

	HoldingInteractTarget = CurrentLookInteractableComp; 
	if (CurrentLookInteractableComp->IsInteractEnable(InputActionInstance.GetSourceAction()))
	{
		CurrentLookInteractableComp->OnInteractClient.Broadcast(this, InputActionInstance.GetSourceAction());
		Server_Interact(CurrentLookInteractableComp.Get(), InputActionInstance.GetSourceAction());
	}
}

void AFHPlayerBase::Server_Interact_Implementation(UFHInteractableComponent* InteractActor, const UInputAction* InputAction)
{
	if (InteractActor)
	{
		InteractActor->OnInteractServer.Broadcast(this, InputAction);
	}
}

void AFHPlayerBase::InteractHold(const struct FInputActionInstance& InputActionInstance)
{
	if (!CurrentLookInteractableComp)
	{
		CurrentLookInteractableComp = nullptr;
		InitInteractState();
		return;
	}
	if (HoldingInteractTarget != CurrentLookInteractableComp)
	{
		InitInteractState();
		return;
	}

	if (bAllowInteract == false) return;

	if (abilitySystem && abilitySystem->HasAnyMatchingGameplayTags(PlayerDescriptor->BlockInteractTags))
	{
		return;
	}

	if (!CurrentLookInteractableComp->IsInteractHoldingEnable(InputActionInstance.GetSourceAction()))
	{
		return;
	}

	float Percent = 0.0f;
	if (HoldingInteractTriggered == false &&
		CurrentLookInteractableComp->GetHoldDurationForHoldingInteract() <= ElapsedTimeForHoldingInteract)
	{
		CurrentLookInteractableComp->OnInteractHoldClient.Broadcast(this, InputActionInstance.GetSourceAction(), ElapsedTimeForHoldingInteract);
		Server_InteractHold(CurrentLookInteractableComp.Get(), ElapsedTimeForHoldingInteract, InputActionInstance.GetSourceAction());
		HoldingInteractTriggered = true;


		// 여기서 UI 지우기?
		return;
	}
	else
	{
		ElapsedTimeForHoldingInteract += GetWorld()->GetDeltaSeconds();

		if (CurrentLookInteractableComp->GetHoldDurationForHoldingInteract() > ElapsedTimeForHoldingInteract)
		{
			Percent = ElapsedTimeForHoldingInteract / CurrentLookInteractableComp->GetHoldDurationForHoldingInteract();
		}
		GetPlayerStateChecked<AFHPlayerStateBase>()->OnChangeInteractProgressPercentDelegate.Broadcast(Percent);
	}
}

void AFHPlayerBase::Server_InteractHold_Implementation(UFHInteractableComponent* InteractActor, float HoldDuration, const UInputAction* InputAction)
{
	InteractActor->OnInteractHoldServer.Broadcast(this, InputAction, HoldDuration);
}

void AFHPlayerBase::InteractReleased(const struct FInputActionInstance& InputActionInstance)
{
	GetPlayerStateChecked<AFHPlayerStateBase>()->OnChangeInteractProgressPercentDelegate.Broadcast(0.0f);
	if (!CurrentLookInteractableComp)
	{
		CurrentLookInteractableComp = nullptr;
		InitInteractState();
		return;
	}
	if (HoldingInteractTarget != CurrentLookInteractableComp)
	{
		InitInteractState();
		return;
	}

	if (bAllowInteract == false) return;

	if (abilitySystem && abilitySystem->HasAnyMatchingGameplayTags(PlayerDescriptor->BlockInteractTags))
	{
		return;
	}

	if (CurrentLookInteractableComp->IsBlockHoldingReleaseInteract() == false)
	{
		CurrentLookInteractableComp->OnInteractReleaseClient.Broadcast(this, InputActionInstance.GetSourceAction(), ElapsedTimeForHoldingInteract);
		Server_InteractReleased(CurrentLookInteractableComp.Get(), ElapsedTimeForHoldingInteract, InputActionInstance.GetSourceAction());
		InitInteractState();
	}
}

void AFHPlayerBase::Server_InteractReleased_Implementation(UFHInteractableComponent* InteractActor, float HoldDuration, const UInputAction* InputAction)
{
	if (InteractActor)
	{
		InteractActor->OnInteractReleaseServer.Broadcast(this, InputAction, HoldDuration);

	}
}



void AFHPlayerBase::ToggleFlashlight()
{
	if (bIsTerminalOpen == false) return;
	if (bCanToggleFlashlight == false) return;

	bIsFlashlightOn = !bIsFlashlightOn;
	Flashlight->SetVisibility(bIsFlashlightOn);
	ServerRPC_ToggleFlashlight(bIsFlashlightOn);

	AFHTerminalBase* Terminal = GetTerminalActor();
	if (Terminal)
	{
		if (bIsFlashlightOn)
		{
			Terminal->NextBatteryReduceIntervalSecond = PlayerDescriptor->BatteryReduceInterval_FlashlightOn;
		}
		else
		{
			Terminal->NextBatteryReduceIntervalSecond = PlayerDescriptor->BatteryReduceInterval_FlashlightOff;
		}
	}
}

bool AFHPlayerBase::IsFlashlightOn()
{
	return bIsFlashlightOn;
}

void AFHPlayerBase::ServerRPC_ToggleFlashlight_Implementation(bool toggle)
{
	MulticastRPC_ToggleFlashlight(toggle);

	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		if (toggle == true)
		{
			SoundManager->PlaySoundAtLocationMulticastByTag(PlayerDescriptor->FlashOnSoundTag, GetActorLocation());
		}
		else
		{
			SoundManager->PlaySoundAtLocationMulticastByTag(PlayerDescriptor->FlashOffSoundTag, GetActorLocation());
		}
	}
	if (GetAbilitySystemComponent())
	{
		if (toggle)
		{
			GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer{ GET_GAMEPLAY_TAG_PLAYER_ABILITY_FLASHLIGHTON });
		}
		else
		{
			GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer{ GET_GAMEPLAY_TAG_PLAYER_ABILITY_FLASHLIGHTOFF });
		}
	}
}

void AFHPlayerBase::MulticastRPC_ToggleFlashlight_Implementation(bool toggle)
{
	Flashlight->SetVisibility(toggle);

	if (IsLocallyControlled() == false)
	{
		bIsFlashlightOn = toggle;
	}
}

void AFHPlayerBase::OnChangedItem(USiInventoryComponent* InventoryComponent, int32 index)
{
	if (InventoryComponent->GetItemInstance(index) == nullptr)
	{
		return;
	}
	SwitchItem(index + 1);
}

void AFHPlayerBase::CheckNearbyInteractableObjects(float DeltaTime)
{
	ElapsedTimeFromLastCheckInteractables += DeltaTime;
	if (ElapsedTimeFromLastCheckInteractables >= PlayerDescriptor->CheckInteractableObjectInterval)
	{
		ElapsedTimeFromLastCheckInteractables -= PlayerDescriptor->CheckInteractableObjectInterval;
	}
	else
	{
		return;
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	TArray<AActor*> IgnoreList;
	IgnoreList.Add(this);

	TArray<AActor*> Overlapped;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),
		GetActorLocation(),
		PlayerDescriptor->CheckInteractableObjectRadius,
		ObjectTypes,
		nullptr,
		IgnoreList,
		Overlapped);

	TSet<UFHInteractableComponent*> CurrentOverrapedInteractables;
	for (AActor* actor : Overlapped)
	{
		if (actor == nullptr) continue;

		if (UFHInteractableComponent* comp = actor->FindComponentByClass<UFHInteractableComponent>(); comp != nullptr)
		{
			if (comp->IsActive() == false) continue;

			CurrentOverrapedInteractables.Add(comp);

			if (NearbyInteractables.Contains(comp))
			{
				continue;
			}
			else
			{
				comp->Highlight(PlayerDescriptor->HighlightInteractableObjectTime);
				NearbyInteractables.Add(comp);
			}
		}
	}

	for (auto Iter = NearbyInteractables.CreateIterator(); Iter; ++Iter)
	{
		if (CurrentOverrapedInteractables.Contains(*Iter) == false)
		{
			Iter.RemoveCurrent();
		}
	}
}

void AFHPlayerBase::StartSprint(int32 InputID)
{
	if (!abilitySystem) return;

	FGameplayAbilitySpec* abilitySpec = abilitySystem->FindAbilitySpecFromInputID(InputID);
	if (!abilitySpec) return;

	//abilitySpec->InputPressed = true;
	if (abilitySpec->IsActive())
	{
		//abilitySystem->AbilitySpecInputPressed(*abilitySpec);
	}
	else
	{
		abilitySystem->TryActivateAbility(abilitySpec->Handle);
	}
}

void AFHPlayerBase::EndSprint(int32 InputID)
{
	if (!abilitySystem) return;

	FGameplayAbilitySpec* abilitySpec = abilitySystem->FindAbilitySpecFromInputID(InputID);
	if (!abilitySpec) return;

	//abilitySpec->InputPressed = false;
	if (abilitySpec->IsActive())
	{
		abilitySystem->AbilitySpecInputReleased(*abilitySpec);
	}
}

void AFHPlayerBase::StartCrouch(int32 InputID)
{
	if (!abilitySystem) return;

	FGameplayAbilitySpec* abilitySpec = abilitySystem->FindAbilitySpecFromInputID(InputID);
	if (!abilitySpec) return;

	//abilitySpec->InputPressed = true;
	if (abilitySpec->IsActive())
	{
		//abilitySystem->AbilitySpecInputPressed(*abilitySpec);
	}
	else
	{
		abilitySystem->TryActivateAbility(abilitySpec->Handle);
	}
}

void AFHPlayerBase::EndCrouch(int32 InputID)
{
	if (!abilitySystem) return;

	FGameplayAbilitySpec* abilitySpec = abilitySystem->FindAbilitySpecFromInputID(InputID);
	if (!abilitySpec) return;

	//abilitySpec->InputPressed = false;
	if (abilitySpec->IsActive())
	{
		abilitySystem->AbilitySpecInputReleased(*abilitySpec);
	}
}

void AFHPlayerBase::SwitchItemByInput(const FInputActionValue& Value, int32 SlotNum)
{
	if (abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISUNSWAPPABLE))
	{
		return;
	}

	SwitchItem(SlotNum);
}

void AFHPlayerBase::SwitchItem(int32 Index)
{
	if (abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISLOOKTERMINAL) && bIsTerminalOpen == true)
	{
		bIsTerminalOpen = false;
		ToggleTerminal_Impl(false, false);
	}

	UFHInventoryComponent* InventoryComp = Cast<UFHInventoryComponent>(GetInventoryComponent());
	if (InventoryComp)
	{
		CurrentSelectedQuickslotIndex = Index - 1;
		InventoryComp->Server_SetCurrentItemIndex(Index - 1);
	}
}

EItemHoldingType AFHPlayerBase::GetCurrentPointedItemHoldingType()
{
	UFHInventoryComponent* InventoryComp = Cast<UFHInventoryComponent>(GetInventoryComponent());

	
	if (!InventoryComp)
	{
		return EItemHoldingType::NoHanded;
	}

	UFHItemBase* holdingItem = InventoryComp->GetCurrentItem();
	if (!holdingItem)
	{
		return EItemHoldingType::NoHanded;
	}

	return holdingItem->GetItemData().HoldingType;
}

void AFHPlayerBase::ScrollQuickSlot(const FInputActionValue& Value)
{
	if (abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISUNSWAPPABLE))
	{
		return;
	}

	float ScrollValue = Value.Get<float>();
	if (FMath::IsNearlyZero(ScrollValue))
	{
		return;
	}

	QuickSlotScrollAccumulator += ScrollValue;

	int32 ScrollCount = 0;

	// 누적 값이 ±1 이상일 경우 슬롯 이동
	while (FMath::Abs(QuickSlotScrollAccumulator) >= QuickSlotScrollThreshold)
	{
		if (QuickSlotScrollAccumulator > 0.f)
		{
			ScrollCount--;
			QuickSlotScrollAccumulator -= QuickSlotScrollThreshold;
		}
		else
		{
			ScrollCount++;
			QuickSlotScrollAccumulator += QuickSlotScrollThreshold;
		}
	}

	if (ScrollCount != 0)
	{
		UFHInventoryComponent* InventoryComp = Cast<UFHInventoryComponent>(GetInventoryComponent());
		if (InventoryComp)
		{
			const int32 QuickSlotCount = InventoryComp->GetItemNum();
			if (QuickSlotCount == 0) return;
			int32 idx = (InventoryComp->GetCurrentItemIndex() + ScrollCount + QuickSlotCount) % QuickSlotCount;
			SwitchItem(idx + 1);
		}
	}
}

void AFHPlayerBase::DoSomethingForDebugging(float param)
{

}






AFHTerminalBase* AFHPlayerBase::GetTerminalActor()
{
	if (AActor* terminal = TerminalChildActor->GetChildActor(); terminal != nullptr)
	{
		return Cast<AFHTerminalBase>(terminal);
	}
	else
	{
		return nullptr;
	}
}

void AFHPlayerBase::ToggleTerminal(const FInputActionValue& Value)
{
	/*if (bIsTerminalOpen == false && abilitySystem->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISACTIONOCCUPIED))
	{
		return;
	}*/

	//bIsTerminalOpen ^= 1;
	ToggleTerminal_Impl(!bIsTerminalOpen);
}

void AFHPlayerBase::ToggleTerminal_Impl(uint8 bIsOpen, uint8 SwapAnimation)
{
	bIsTerminalOpen = bIsOpen;

	Server_ToggleTerminal(bIsTerminalOpen);
	if (bIsTerminalOpen == false)
	{
		ServerRPC_ToggleFlashlight(false);
	}
	else if (bIsTerminalOpen && bIsFlashlightOn)
	{
		ServerRPC_ToggleFlashlight(true);
	}

	AFHTerminalBase* Terminal = GetTerminalActor();
	if (Terminal)
	{
		if (bIsTerminalOpen)
		{
			Terminal->bUpdateBattery = true;
		}
		else
		{
			Terminal->bUpdateBattery = false;
		}
	}
	if (bIsTerminalOpen)
	{
		//bAllowInteract = false;

		TerminalChildActor->SetVisibility(true);
		CurrentEquipment.Mesh = nullptr;
		CurrentEquipment.MeshType = EItemHoldingType::NoHanded;
		PendingMesh = nullptr;
		PendingType = EItemHoldingType::NoHanded;
		GrabMesh->SetStaticMesh(nullptr);
		PrevHoldingItem = nullptr;
		AnimHoldingType = EItemHoldingType::Terminal;

		abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISLOOKTERMINAL);
		abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISACTIONOCCUPIED); // 알아서 카운팅 함
		//abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISUNSWAPPABLE); // 알아서 카운팅 함

		FGameplayTagContainer CancelTargetTags{ GET_GAMEPLAY_TAG("Player.Ability.UseItem") };
		abilitySystem->CancelAbilities(&CancelTargetTags);

		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		SoundManager->PlaySoundAtLocationLocallyByTag(PlayerDescriptor->OpenTerminalSoundTag, GetActorLocation());

		UFHInventoryComponent* InventoryComp = Cast<UFHInventoryComponent>(GetInventoryComponent());
		if (InventoryComp)
		{
			InventoryComp->SetFocusEmpty();
		}
	}
	else
	{
		//bAllowInteract = true;

		TerminalChildActor->SetVisibility(false);

		UFHInventoryComponent* InventoryComp = Cast<UFHInventoryComponent>(GetInventoryComponent());
		if (SwapAnimation)
		{
			InventoryComp->Server_SetCurrentItemIndex(InventoryComp->GetCurrentItemIndex());
		}
		InventoryComp->OnRep_CurrentItemIndex();

		abilitySystem->RemoveLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISLOOKTERMINAL);
		abilitySystem->RemoveLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISACTIONOCCUPIED); // 알아서 카운팅 함
		//abilitySystem->RemoveLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISUNSWAPPABLE); // 알아서 카운팅 함

		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		SoundManager->PlaySoundAtLocationLocallyByTag(PlayerDescriptor->CloseTerminalSoundTag, GetActorLocation());
	}

	GetPlayerStateChecked<AFHPlayerStateBase>()->OnToggleTerminalDelegate.Broadcast(bIsTerminalOpen);
}

void AFHPlayerBase::ConstructMinimapWidget(FVector RoomUnit, const TArray<FMinimapRoomSpec>& RoomSpecs, int32 DungeonLowestFloor, int32 DungeonHighestFloor, float RoomHeight, float GeneratorPosZ)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	AFHTerminalBase* Terminal = GetTerminalActor();
	Terminal->OwnerPlayer = this;
	Terminal->ConstructMinimapWidget(RoomUnit, RoomSpecs, DungeonLowestFloor, DungeonHighestFloor, RoomHeight, GeneratorPosZ);
}

void AFHPlayerBase::Server_ToggleTerminal_Implementation(uint8 bIsOpen)
{
	Multicast_ToggleTerminal(bIsOpen);

	if (bIsTerminalOpen)
	{
		abilitySystem->ExecuteGameplayCue(GET_GAMEPLAY_TAG_GAMEPLAYCUE_PLAYER_TERMINAL_OPEN);
	}
}

void AFHPlayerBase::Multicast_ToggleTerminal_Implementation(uint8 bIsOpen)
{
	bIsTerminalOpen = bIsOpen;

	if (IsLocallyControlled())
	{
		return;
	}

	if (bIsTerminalOpen)
	{
		TerminalChildActor->SetVisibility(true);

		CurrentEquipment.Mesh = nullptr;
		CurrentEquipment.MeshType = EItemHoldingType::NoHanded;
		PendingMesh = nullptr;
		PendingType = EItemHoldingType::NoHanded;
		GrabMesh->SetStaticMesh(nullptr);
		PrevHoldingItem = nullptr;
		AnimHoldingType = EItemHoldingType::Terminal;
	}
	else
	{
		TerminalChildActor->SetVisibility(false);
	}
}












void AFHPlayerBase::PlayEmote(EPlayerEmoteType EmoteType)
{
	if (IsValid(abilitySystem) == false) return;

	FGameplayEventData Payload;
	Payload.EventTag = GET_GAMEPLAY_TAG_PLAYER_ABILITY_PLAYEMOTE;
	Payload.EventMagnitude = static_cast<float>(EmoteType);

	abilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void AFHPlayerBase::Server_HideGrabMeshTemporarily_Implementation()
{
	Multicast_HideGrabMeshTemporarily();
}

void AFHPlayerBase::Server_ShowGrabMeshBack_Implementation()
{
	Multicast_ShowGrabMeshBack();
}

void AFHPlayerBase::Multicast_HideGrabMeshTemporarily_Implementation()
{
	GrabMeshCache = GrabMesh->GetStaticMesh();
	GrabMesh->SetStaticMesh(nullptr);
}

void AFHPlayerBase::Multicast_ShowGrabMeshBack_Implementation()
{
	GrabMesh->SetStaticMesh(GrabMeshCache);
}



// ================== Ragdoll ===================

void AFHPlayerBase::SetRagdollingState(bool SetRagdolling)
{
	//if (bIsRagdolling == SetRagdolling)
	//{
	//	return;
	//}
	bIsRagdolling = SetRagdolling;

	USkeletalMeshComponent* thirdPersonMesh = GetMesh();
	UCapsuleComponent* capsuleComp = GetCapsuleComponent();

	if (!thirdPersonMesh || !MovementComponent || !capsuleComp) return;

	if (SetRagdolling)
	{
		//MovementComponent->StopMovementImmediately();
		MovementComponent->DisableMovement();
		MovementComponent->SetComponentTickEnabled(false);

		//AttachLightsAtHead();
		ShowThirdPerson(500.0f, {0, 0, 60.0f});

		capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		thirdPersonMesh->SetCollisionProfileName(TEXT("Ragdoll"));
		thirdPersonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		thirdPersonMesh->SetAllBodiesSimulatePhysics(true); // 모든 바디, 본 시뮬레이션 켜기 -> 래그돌용
		thirdPersonMesh->SetSimulatePhysics(true); // 루트 바디만 시뮬레이션 켜기 / 이거 중복으로 켜면 ECollisionEnabled 비호환 워닝 뜸. 이유는 몰?루 / <07-22> Knockdown GA 때문에 켰다.
		thirdPersonMesh->SetPhysicsAsset(RagdollPhysicsAssetSoft.LoadSynchronous()); // SetSimulate 이전에 호출하는 게 안전하다는데 그 전에 호출하면 Groom Component 삐져나가고 메쉬 땅이랑 겹치고 난리나는데?

 		thirdPersonMesh->WakeAllRigidBodies();

		// AController* CurrentController = GetController();
		// if (CurrentController)
		// {
		//     CurrentController->SetIgnoreMoveInput(true);
		// }


		// Copy Pose From skeletal 쓰고있으면 목이랑 몸 어긋남
		USkeletalMeshComponent* BodyMesh = GetMesh();
		FaceMesh->SetLeaderPoseComponent(BodyMesh);
	}
	else
	{
		MovementComponent->SetComponentTickEnabled(true);
		MovementComponent->SetMovementMode(MOVE_Walking);
		MovementComponent->Velocity = FVector::ZeroVector;

		//AttachLightsAtCamera();
		ShowFirstPerson();

		capsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//capsuleComp->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);

		thirdPersonMesh->SetAllBodiesSimulatePhysics(false);
		thirdPersonMesh->SetSimulatePhysics(false);
		thirdPersonMesh->SetCollisionProfileName(TEXT("CharacterMesh"));
		thirdPersonMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		thirdPersonMesh->SetPhysicsAsset(NormalPhysicsAssetSoft.LoadSynchronous()); // SetSimulate 이전에 호출하는 게 안전하다는데 그 전에 호출하면 Groom Component 삐져나가고 메쉬 땅이랑 겹치고 난리나는데?

		thirdPersonMesh->AttachToComponent(capsuleComp, FAttachmentTransformRules::KeepRelativeTransform);
		thirdPersonMesh->SetRelativeLocationAndRotation(DefaultThirdPersonMeshRelativeLocation, DefaultThirdPersonMeshRelativeRotation);


		FaceMesh->SetLeaderPoseComponent(nullptr);
	}
}

void AFHPlayerBase::Server_SetRagdollingState_Implementation(bool SetRagdolling)
{
	Multicast_SetRagdollingState(SetRagdolling);
	// ForceNetUpdate();
}

void AFHPlayerBase::Multicast_SetRagdollingState_Implementation(bool SetRagdolling)
{
	SetRagdollingState(SetRagdolling);
}

void AFHPlayerBase::Server_ThrowPlayerAway_Implementation(FVector Direction, float Power, FName TargetBone, bool bIsDead)
{
	FFHGACustomData_ThrowingPlayer* customData = new FFHGACustomData_ThrowingPlayer;
	customData->ThrowDirection = Direction;
	customData->ThrowPower = Power;
	customData->bIsDead = bIsDead;
	customData->Targetbone = TargetBone;

	FGameplayEventData Payload;
	Payload.EventTag = GET_GAMEPLAY_TAG_PLAYER_ABILITY_KNOCKDOWN;
	Payload.TargetData.Add(customData);

	abilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void AFHPlayerBase::ImpulseToPlayer_Implementation(FVector Direction, float Power, FName TargetBone)
{
	GetMesh()->AddImpulse(Direction * Power, TargetBone, true);
}

void AFHPlayerBase::Server_RecoverFromRagdolling_Implementation()
{
	// 스켈레탈 메쉬는 컨트롤러가 나가서 그런지 트랜스폼이 변하질 않는다. 그래서 본에서 가져와야 함.
	// 위치 정보는 서버 기준 / 회전(위를 보고 일어날지 아래를 보고 일어날지)은 각자 해도 상관없음
	FVector LastLocation = GetMesh()->GetBoneLocation(TEXT("pelvis"));
	FQuat LastQuat = GetMesh()->GetBoneQuaternion(TEXT("pelvis"));

	// 위치와 회전은 서버 기준으로 맞추기 / 이 과정에서 클라가 볼 때 텔레포트가 일어나도 어쩔 수 없음.
	LastLocation.Z = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	//SetActorLocation(LastLocation);
	// 머리 방향 맞추기
	FVector PelvisLocation = GetMesh()->GetBoneLocation(TEXT("pelvis"));
	FVector NeckLocation = GetMesh()->GetBoneLocation(TEXT("neck_01"));
	FVector PlayerDirection = (PelvisLocation - NeckLocation).GetSafeNormal();
	PlayerDirection.Z = 0.0f;
	FRotator LastRotation = PlayerDirection.Rotation();
	//SetActorRotation(LastRotation);

	Multicast_RecoverFromRagdolling(LastQuat, LastLocation, LastRotation);
}

void AFHPlayerBase::Multicast_RecoverFromRagdolling_Implementation(FQuat LastQuat, FVector LastLocation, FRotator LastRotation)
{
	// 기본적으로 스켈레탈 메쉬랑 그 밑의 본들은 Y축 방향이 정면으로 가게 돼있음
	FVector LastForward = LastQuat.GetRightVector();

	TeleportTo(LastLocation, LastRotation);
	/*SetActorLocation(LastLocation);
	SetActorRotation(LastRotation);*/

	// <07-23> 캡슐 - 본 거리가 본 트랜스폼에 저장되는 문제 해결 실패 / 임시로 일어나는 모션 첫 프레임에서 그냥 일어나게 함
	//GetMesh()->GetAnimInstance()->SavePoseSnapshot(TEXT("LastRagdollPose"));

	// 래그돌 상태일 때의 마지막 포즈 저장하기

	// 다음 틱에서 호출하여 스냅샷이 제대로 저장된 뒤 사용하게 함
	//GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	//{
	//}));
	if (LastForward.Z >= 0.0f)
	{
		bGetUpFromFront = true;
	}
	else
	{
		bGetUpFromFront = false;
	}

	SetRagdollingState(false);


	// 자연스러운 일어나기를 위해 몽타쥬 대신 스테이트 사용
	/*FGameplayCueParameters params;
	if (LastForward.Z >= 0.0f)
	{
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_WakeUpFront, params);
	}
	else
	{
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_WakeUpBack, params);
	}*/
}


















void AFHPlayerBase::ActivateBloodSplashEffect_Implementation(FVector Location, FRotator Rotation)
{
	if (!PlayerDescriptor->BloodSplashNiagara) return;

	Multicast_ActivateBloodSplashEffect(Location, Rotation);
}

void AFHPlayerBase::ActivateStunEffect_Implementation()
{
	if (!PlayerDescriptor->StunNiagaraActor) return;

	Multicast_ActivateStunEffect();
}

void AFHPlayerBase::Multicast_ActivateBloodSplashEffect_Implementation(FVector Location, FRotator Rotation)
{
	FVector ClosestBoneLocation;
	GetMesh()->FindClosestBone(Location, &ClosestBoneLocation);

	Location = ClosestBoneLocation;

	FActorSpawnParameters param{};
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ANiagaraActor* actor = GetWorld()->SpawnActor<ANiagaraActor>(ANiagaraActor::StaticClass(), Location, Rotation, param);
	if (actor)
	{
		actor->SetDestroyOnSystemFinish(true);

		actor->GetNiagaraComponent()->SetAsset(PlayerDescriptor->BloodSplashNiagara, true);
		actor->GetNiagaraComponent()->Activate(false);
	}
}

void AFHPlayerBase::Multicast_ActivateStunEffect_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(77);

	if (FaceMesh)
	{
		FaceMesh->SetRenderCustomDepth(true);
		FaceMesh->SetCustomDepthStencilValue(77);
	}
	if (TorsoMesh)
	{
		TorsoMesh->SetRenderCustomDepth(true);
		TorsoMesh->SetCustomDepthStencilValue(77);
	}
	if (LegsMesh)
	{
		LegsMesh->SetRenderCustomDepth(true);
		LegsMesh->SetCustomDepthStencilValue(77);
	}
	if (FeetMesh)
	{
		FeetMesh->SetRenderCustomDepth(true);
		FeetMesh->SetCustomDepthStencilValue(77);
	}
	if (BackpackMesh)
	{
		BackpackMesh->SetRenderCustomDepth(true);
		BackpackMesh->SetCustomDepthStencilValue(77);
	}
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation());
	SpawnTransform.SetRotation(FQuat::Identity);

	StunNiagaraActor = GetWorld()->SpawnActor<ANiagaraActor>(
		PlayerDescriptor->StunNiagaraActor,
		SpawnTransform
	);

	GetWorld()->GetTimerManager().SetTimer(
		StunEffectTimerHandle,
		FTimerDelegate::CreateLambda([this]()
	{
		GetMesh()->SetRenderCustomDepth(false);

		if (FaceMesh)
		{
			FaceMesh->SetRenderCustomDepth(false);
		}
		if (TorsoMesh)
		{
			TorsoMesh->SetRenderCustomDepth(false);
		}
		if (LegsMesh)
		{
			LegsMesh->SetRenderCustomDepth(false);
		}
		if (FeetMesh)
		{
			FeetMesh->SetRenderCustomDepth(false);
		}
		if (BackpackMesh)
		{
			BackpackMesh->SetRenderCustomDepth(false);
		}
		
		StunNiagaraActor = nullptr;
	}), 3.0f, false);
}

AActor* AFHPlayerBase::GetActorInfront(const float MaxLength, OUT float& Distance, bool bOnlyDetectInteractable)
{
	FVector Start = CameraComponent->GetComponentLocation();
	FVector ForwardVector = CameraComponent->GetForwardVector();
	Start += (ForwardVector * PlayerDescriptor->AdditionalViewRayStartDistanceFromCamera);
	FVector End = Start + (ForwardVector * MaxLength);

	/*TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECC_Camera, Params);

	if (bHit)
	{
		PRINT_LOG(TEXT("Collided with %s"), *HitResults[0].GetActor()->GetName());
		FHitResult& Nearest = HitResults[0];
		Distance = Nearest.Distance;
		return Nearest.GetActor();
	}
	return nullptr;*/

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionShape SweepShape = FCollisionShape::MakeSphere(PlayerDescriptor->GetInfrontActorSweepRadius);
	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_Camera, SweepShape, Params);

	if (bHit)
	{
		HitResults.Sort([](const FHitResult& A, const FHitResult& B)
		{
			return A.Distance < B.Distance;
		});

		if (bOnlyDetectInteractable)
		{
			for (FHitResult& hit : HitResults)
			{
				if (!hit.GetActor()) continue;
				UFHInteractableComponent* interactableComp = hit.GetActor()->GetComponentByClass<UFHInteractableComponent>();
				if (!interactableComp) continue;
				if (!interactableComp->IsActive()) continue;
				if (interactableComp->bIgnoreThisActorFromInteractCheck) continue;

				Distance = hit.Distance;
				return hit.GetActor();
			}
		}
		else
		{
			for (FHitResult& hit : HitResults)
			{
				if (!hit.GetActor()) continue;
				UFHInteractableComponent* interactableComp = hit.GetActor()->GetComponentByClass<UFHInteractableComponent>();
				if (interactableComp)
				{
					if (interactableComp->bIgnoreThisActorFromInteractCheck) continue;
				}

				Distance = hit.Distance;
				return hit.GetActor();
			}
		}

		
	}

	return nullptr;
}

AActor* AFHPlayerBase::GetActorInfront(const float MaxLength, bool bOnlyDetectInteractable)
{
	float dummy;
	return GetActorInfront(MaxLength, dummy, bOnlyDetectInteractable);
}

void AFHPlayerBase::UpdateFocusingState(float DeltaTime)
{
	switch (FocusingPhase)
	{
		case EPlayerFocusingPhase::None:
		{

			break;
		}
		case EPlayerFocusingPhase::FocusIn:
		{
			/*FVector CameraLocation = CameraComponent->GetComponentLocation();
			FRotator CurrentRotation = CameraComponent->GetComponentRotation();
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, FocusingAtLocation);

			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, FocusingInterpSpeed);
			NewRotation.Roll = 0.0f;
			CameraComponent->SetWorldRotation(NewRotation);*/

			APlayerController* pc = Cast<APlayerController>(GetController());

			FVector CameraLocation = CameraComponent->GetComponentLocation();
			FRotator CurrentRotation = pc->GetControlRotation();
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, FocusingAtLocation);

			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, PlayerDescriptor->FocusingInterpSpeed);
			NewRotation.Roll = 0.0f;
			pc->SetControlRotation(NewRotation);

			float CurrentFOV = CameraComponent->FieldOfView;
			float NewFOV = FMath::FInterpTo(CurrentFOV, FocusingTargetFOV, DeltaTime, PlayerDescriptor->FocusingInterpSpeed);
			CameraComponent->SetFieldOfView(NewFOV);

			break;
		}
		case EPlayerFocusingPhase::FocusOut:
		{
			float CurrentFOV = CameraComponent->FieldOfView;
			float NewFOV = FMath::FInterpTo(CurrentFOV, FocusingDefaultFOV, DeltaTime, PlayerDescriptor->FocusingInterpSpeed);

			if (FMath::IsNearlyEqual(CurrentFOV, NewFOV))
			{
				CameraComponent->SetFieldOfView(FocusingDefaultFOV);
				FocusingPhase = EPlayerFocusingPhase::None;
			}
			else
			{
				CameraComponent->SetFieldOfView(NewFOV);
			}
			
			break;
		}
	}
}

void AFHPlayerBase::StartFocusAt(AActor* target)
{
	StartFocusAt(target->GetActorLocation());
}

void AFHPlayerBase::StartFocusAt(FVector target)
{
	FocusingPhase = EPlayerFocusingPhase::FocusIn;
	FocusingAtLocation = target;
	FocusingDefaultFOV = CameraComponent->FieldOfView; // 이 부분 설정 FOV로 변경
	FocusingTargetFOV = FocusingDefaultFOV * PlayerDescriptor->FocusingFovMult;
}

void AFHPlayerBase::EndFocusing()
{
	FocusingPhase = EPlayerFocusingPhase::FocusOut;
}

void AFHPlayerBase::AttachWidgetToViewport(TWeakObjectPtr<class UFHInteractableComponent> Interactable)
{
	if (!Interactable.IsValid())
	{
		return;
	}
	ElapsedTimeForHoldingInteract = 0.0f;
	CurrentInteractTooltipUIInstance = Interactable->GetTooltipUIWidget();
	// CreateWidget<UFHInteractTooltipWidget>(GetController<APlayerController>(), );

	if (CurrentInteractTooltipUIInstance)
	{
		CurrentInteractTooltipUIInstance->SetOwningPlayer(Cast<APlayerController>(GetController()));
		CurrentInteractTooltipUIInstance->AddToPlayerScreen();

		if (CurrentInteractTooltipUIInstance->bCalculateCursorPosition)
		{
			FVector2D MousePos{};
			APlayerController* PC = GetController<APlayerController>();
			check(PC);
			ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();

			if (LocalPlayer && LocalPlayer->ViewportClient)
			{
				LocalPlayer->ViewportClient->GetViewportSize(MousePos);
				MousePos *= 0.5f;
			}
			CurrentInteractTooltipUIInstance->SetPositionInViewport(MousePos, true);
		}

		CurrentInteractTooltipUIInstance->SetInteractTooltipText(Interactable->GetTooltipText(), Interactable->GetFilterInputActons());
	}
}

void AFHPlayerBase::DetachWidgetFromViewportAndInitCurrentLookInteractableComp()
{
	if (CurrentInteractTooltipUIInstance)
	{
		CurrentInteractTooltipUIInstance->RemoveFromParent();
	}
	CurrentLookInteractableComp = nullptr;
	CurrentInteractTooltipUIInstance = nullptr;
}

void AFHPlayerBase::UpdateCurrentLookingInteractableActor()
{
	if (!IsLocallyControlled()) return;

	if (bAllowInteract == false)
	{
		if (CurrentLookInteractableComp != nullptr)
		{
			InitInteractState();
			DetachWidgetFromViewportAndInitCurrentLookInteractableComp();
		}
		return;
	}


	float Distance{};
	AActor* actorInfront = GetActorInfront(PlayerDescriptor->MaxInteractDistance, Distance);

	UFHInteractableComponent* interactableCompInfront = nullptr;
	if (actorInfront)
	{
		interactableCompInfront = actorInfront->FindComponentByClass<UFHInteractableComponent>();
		if (interactableCompInfront)
		{
			if (interactableCompInfront->IsActive() == false)
			{
				interactableCompInfront = nullptr;
			}
		}
	}

	//TScriptInterface<IFHInteractable> interactableActor(actorInfront);
	//IFHInteractable* interactableActorInfront = interactableActor.GetInterface();

	if (interactableCompInfront != nullptr && interactableCompInfront->GetInteractDistance() >= Distance)
	{
		if (CurrentLookInteractableComp != nullptr)
		{
			if (CurrentLookInteractableComp == interactableCompInfront)
			{
 				if (interactableCompInfront->bNeedUpdateTooltipText == true)
				{
					CurrentInteractTooltipUIInstance->SetInteractTooltipText(interactableCompInfront->GetTooltipText(), interactableCompInfront->GetFilterInputActons());
					interactableCompInfront->bNeedUpdateTooltipText = false;
				}
				if (CurrentLookInteractableComp->IsBlockTooltipUI() != LastLookInteractableCompBlockShowUISetting)
				{
					LastLookInteractableCompBlockShowUISetting = !LastLookInteractableCompBlockShowUISetting;
					if (LastLookInteractableCompBlockShowUISetting == false)
					{
						AttachWidgetToViewport(CurrentLookInteractableComp);
					}
					else
					{

						DetachWidgetFromViewportAndInitCurrentLookInteractableComp();
					}
				}
				return;
			}
			else
			{
				InitInteractState();
				DetachWidgetFromViewportAndInitCurrentLookInteractableComp();
			}
		}
		if (CurrentLookInteractableComp)
		{
			DetachWidgetFromViewportAndInitCurrentLookInteractableComp();
		}
		CurrentLookInteractableComp = interactableCompInfront;
		if (interactableCompInfront->IsBlockTooltipUI() == false)
		{
			AttachWidgetToViewport(CurrentLookInteractableComp);
			LastLookInteractableCompBlockShowUISetting = false;
		}
		else
		{
			LastLookInteractableCompBlockShowUISetting = true;
		}
	}
	else
	{
		if (CurrentLookInteractableComp != nullptr)
		{
			DetachWidgetFromViewportAndInitCurrentLookInteractableComp();
		}
		InitInteractState();
	}
}

// Server only
void AFHPlayerBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	NET_DEBUG_LOG(TEXT(""));

	InitializeAbilitySystem();
	PRINT_LOG(TEXT("Server:: Initialize AbilitySystemComponents's ActorInfo."));

	InitializeVOIPTalker();

	// GameplayCue가 문젠지 한 0.3~0.5초는 후에 몽타쥬를 재생해야 제대로 작동..
	/*FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimerForNextTick(FTimerDelegate::CreateLambda([this, playerState]()
	{
		OnChangeHoldingItemType(playerState->GetInventoryComponent()->GetCurrentItem());
	}));*/
	//OnChangeHoldingItemType(playerState->GetInventoryComponent()->GetCurrentItem());

	FGameplayAbilitySpec MoveSpec;
	MoveSpec = FGameplayAbilitySpec(PlayerDescriptor->MoveGameplayAbility, 1, INDEX_NONE, this);
	MoveSpecHandle = abilitySystem->GiveAbility(MoveSpec);

	AFHPlayerStateBase* playerState = Cast<AFHPlayerStateBase>(GetPlayerState());
	if (playerState)
	{
		SetNameTag(playerState->GetLicenseRank(), playerState->GetPlayerName());
	}
}

void AFHPlayerBase::UnPossessed()
{
	Super::UnPossessed();

	RemoveInteractTooltipWidgetOnUnposess();
}

void AFHPlayerBase::OnRep_PlayerState()
{
	// 클라이언트에서 PlayerState가 지정된 후에 호출
	Super::OnRep_PlayerState();

	NET_DEBUG_LOG(TEXT(""));

	// 클라이언트가 호출하는 어빌리티는 GAS가 알아서 Authority 역할로부터의 호출인지 Autonomous 역할로부터의 호출인지 판단해서 처리해줌
	// 어빌리티에 인풋 바인딩을 할 때 AbilitySystemComponent가 필요하므로 클라에서도 넣어주도록 한다.
	InitializeAbilitySystem();
	PRINT_LOG(TEXT("Client:: Initialize AbilitySystemComponents's ActorInfo."));

	InitializeVOIPTalker();

	AFHPlayerStateBase* playerState = Cast<AFHPlayerStateBase>(GetPlayerState());
	if (playerState)
	{
		SetNameTag(playerState->GetLicenseRank(), playerState->GetPlayerName());
	}

	// Client Regist Voice Talker
	//AFHPlayerStateBase* FHPS = Cast<AFHPlayerStateBase>(GetPlayerState());
	//if (FHPS)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("[Pawn::OnRep_PlayerState] PlayerState Is Valid."));
	//	FHPS->InitVoiceSetting();
	//}


	//if (GetPlayerState() && VOIPComponent)
	//{
	//	PRINT_LOG(TEXT("[Pawn::OnRep_PlayerState] VOIP Component Add Success."));
	//	VOIPComponent->RegisterWithPlayerState(GetPlayerState());
	//}

	//
	//AFHPlayerStateBase* playerState = Cast<AFHPlayerStateBase>(GetPlayerState());
	//if (playerState)
	//{
	//	SetNameTag(playerState->GetLicenseRank(), playerState->GetPlayerName());

	//	// 여기에서는 SkillID와 PawnKey가 셋팅되지 않음.
	//	//if (IsLocallyControlled())
	//	//{
	//	//	NET_DEBUG_LOG(TEXT("Client Request To Server. Set Skill. SkillID : %s"), *playerState->GetSkillID().ToString());
	//	//	playerState->CallRPC_SetSkillID(playerState->GetSkillID());
	//	//	NET_DEBUG_LOG(TEXT("Client Load Skill Hud."));
	//	//	playerState->UpdateSkillWidget();
	//	//}
	//}
}

void AFHPlayerBase::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	NET_DEBUG_LOG(TEXT(""));

	UFHInventoryComponent* Inventory = UFHBlueprintFunctionLibrary::GetInventoryComponent(NewPlayerState);
	if (Inventory)
	{
		if (!Inventory->OnInventoryChanged.IsAlreadyBound(this, &AFHPlayerBase::OnChangedItem))
		{
			Inventory->OnInventoryChanged.AddDynamic(this, &AFHPlayerBase::OnChangedItem);
		}
	}
	//NameTagWidgetComponent->SetVisibility(true);
	//GetMesh()->SetVisibility(true);


	AFHPlayerStateBase* playerState = Cast<AFHPlayerStateBase>(NewPlayerState);
	if (playerState)
	{
		abilitySystem = playerState->GetAbilitySystemComponent();
		abilitySystem->InitAbilityActorInfo(playerState, this);
		MovementAttributeSet = playerState->GetMovementAttributeSet();
		MovementAttributeSet->OnRep_WalkSpeed(FGameplayAttributeData{ MovementAttributeSet->GetWalkSpeed() });

		SetNameTag(playerState->GetLicenseRank(), playerState->GetPlayerName());

		FOnLoadEnd::FDelegate loadEndDelegate = FOnLoadEnd::FDelegate::CreateUObject(this, &AFHPlayerBase::OnPlayerStateLoadEnd);
		playerState->BindOrExecuteOnPlayerStateLoaded(loadEndDelegate);


		if (playerState && playerState->GetCurrentBackpackActorCache())
		{
			PickUpBackpack(playerState->GetCurrentBackpackActorCache());
		}
	}

}

UAbilitySystemComponent* AFHPlayerBase::GetAbilitySystemComponent() const
{
	return abilitySystem;
}

void AFHPlayerBase::InitializeAbilitySystem()
{
	AFHPlayerStateBase* playerState = GetPlayerState<AFHPlayerStateBase>();
	if (playerState == nullptr) return;

	abilitySystem = playerState->GetAbilitySystemComponent();
	if (abilitySystem == nullptr) return;

	HealthAttributeSet = playerState->GetHealthAttributeSet();
	StaminaAttributeSet = playerState->GetStaminaAttributeSet();
	MovementAttributeSet = playerState->GetMovementAttributeSet();
	PlayerAttributeSet = playerState->GetPlayerAttributeSet();

	abilitySystem->InitAbilityActorInfo(playerState, this);

	if (!HealthAttributeSet->OnHealthZeroDelegate.IsBound())
	{
		HealthAttributeSet->OnHealthZeroDelegate.AddDynamic(this, &AFHPlayerBase::Die);
	}

	// become immortal in lobby level
	AFHGateGameModeBase* GateMode = Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GateMode == nullptr)
	{
		abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISIMMORTAL);
	}

	abilitySystem->RegisterGameplayTagEvent(GET_GAMEPLAY_TAG_DEBUFF_STUN, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnStunTagChanged);
	abilitySystem->RegisterGameplayTagEvent(GET_GAMEPLAY_TAG_PLAYER_STATE_ISCLOAKING, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCloakTagChanged);
}

float AFHPlayerBase::GetRunSpeed() const
{
	if (!MovementAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return MovementAttributeSet->GetRunSpeed();
}

float AFHPlayerBase::GetCurrentStamina() const
{
	if (!StaminaAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return StaminaAttributeSet->GetStamina();
}

float AFHPlayerBase::GetCurrentHealth() const
{
	if (!HealthAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return HealthAttributeSet->GetHealth();
}

float AFHPlayerBase::GetVitality() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetVitality();
}

float AFHPlayerBase::GetStrength() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetStrength();
}

float AFHPlayerBase::GetEndurance() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetEndurance();
}

float AFHPlayerBase::GetAgility() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetAgility();
}

float AFHPlayerBase::GetWillpower() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetWillpower();
}

float AFHPlayerBase::GetCarryWeight() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetCarryWeight();
}

float AFHPlayerBase::GetAttackRange() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetAttackRange();
}

float AFHPlayerBase::GetAttackPower() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetAttackPower();
}

float AFHPlayerBase::GetMiningPower() const
{
	if (!PlayerAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return PlayerAttributeSet->GetMiningPower();
}

float AFHPlayerBase::GetActionSpeedMult() const
{
	if (!MovementAttributeSet)
	{
		PRINT_LOG(TEXT("Invalid AttributeSet."));
		return 0.0f;
	}

	return MovementAttributeSet->GetActionSpeedMult();
}

void AFHPlayerBase::ActivateSkill(FName SkillID)
{
	NET_DEBUG_LOG(TEXT(""));

	const UFHFRankHunterSettings* Settings = GetDefault<UFHFRankHunterSettings>();
	if (!Settings)
	{
		return;
	}
	TSoftObjectPtr<UDataTable> SkillTable = Settings->SkillDataTable;
	if (!SkillTable.IsValid())
	{
		return;
	}
	FFHSkillTable* SkillTableItem = SkillTable->FindRow<FFHSkillTable>(SkillID, TEXT("SkillTable"));
	if (!SkillTableItem)
	{
		return;
	}
	TSubclassOf<UGameplayAbility> Skill = SkillTableItem->SkillClass;
	if (!Skill)
	{
		return;
	}
	if (AFHPlayerStateBase* playerState = GetPlayerState<AFHPlayerStateBase>())
	{
		UAbilitySystemComponent* ASC = playerState->GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->GiveAbility(Skill);
		}
	}
}

void AFHPlayerBase::GetChasedByCreature_Implementation(AFHCreatureBase* Creature)
{
	if (Creature->bPlayChasingSound == false)
	{
		return;
	}

	if (ChasingCreatures.Num() == 0)
	{
		PlayChasingSound();
	}
	ChasingCreatures.Add(Creature);
}

void AFHPlayerBase::LooseAttentionFromCreature_Implementation(AFHCreatureBase* Creature)
{
	ChasingCreatures.Remove(Creature);
	if (ChasingCreatures.Num() == 0)
	{
		StopChasingSound();
	}
}

void AFHPlayerBase::PlayChasingSound_Implementation()
{
	if (!IsLocallyControlled()) return;

	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		SoundManager->PlayAndManageSound2DLocallyByTag(TEXT("Chased"), PlayerDescriptor->BeChasedByCreatureSoundTag);
	}
}

void AFHPlayerBase::StopChasingSound_Implementation()
{
	if (!IsLocallyControlled()) return;

	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		SoundManager->DestroyManagedAudioComponentLocally(TEXT("Chased"));
	}
}

void AFHPlayerBase::UpdateComfortState(float DeltaTime)
{
	if (!MovementComponent) return;
	if (!StaminaAttributeSet) return;

	if (MovementComponent->IsFalling())
	{
		elapsedTimeFromLastAct = 0.0f;
	}
	else if (MovementComponent->IsSprinting() && MovementComponent->Velocity.SquaredLength() > 0)
	{
		elapsedTimeFromLastAct = 0.0f;
	}
	else
	{
		elapsedTimeFromLastAct += DeltaTime;
	}

	//bool IsSprinting = moveComp->IsSprinting();
	//bool NoMove = moveComp->Velocity.SquaredLength() < 10.0f;

	//if (!IsSprinting)
	//{
	//	if (NoMove)
	//	{
	//		elapsedTimeFromLastAct += DeltaTime;
	//		//elapsedTimeFromLastSprinting += DeltaTime;
	//	}
	//	else
	//	{
	//		elapsedTimeFromLastAct = 0.0f;
	//		//elapsedTimeFromLastSprinting += DeltaTime;
	//	}
	//}
	//else
	//{
	//	if (NoMove)
	//	{
	//		elapsedTimeFromLastAct += DeltaTime;
	//		//elapsedTimeFromLastSprinting += DeltaTime;
	//	}
	//	else
	//	{
	//		elapsedTimeFromLastAct = 0.0f;
	//		//elapsedTimeFromLastSprinting = 0.0f;
	//	}
	//}

	if (elapsedTimeFromLastAct >= StaminaAttributeSet->GetNaturalRegenInitDelay())
	{
		if (IsComfort == false)
		{
			IsComfort = true;
			abilitySystem->AddLooseGameplayTag(IsComfortTag);
		}
	}
	else
	{
		if (IsComfort == true)
		{
			IsComfort = false;
			abilitySystem->RemoveLooseGameplayTag(IsComfortTag);
		}
	}
}

void AFHPlayerBase::ResetElapsedTimeFromLastMove()
{
	elapsedTimeFromLastAct = 0.0f;
	//elapsedTimeFromLastSprinting = 0.0f;
}

void AFHPlayerBase::ApplyTestEffect_Implementation()
{
	if (!PlayerDescriptor->TestEffect) return;

	FGameplayEffectSpecHandle effectSpecHandle = abilitySystem->MakeOutgoingSpec(PlayerDescriptor->TestEffect, 1, abilitySystem->MakeEffectContext());
	abilitySystem->ApplyGameplayEffectSpecToSelf(*effectSpecHandle.Data.Get());
}

void AFHPlayerBase::ChangeMetahuman(TSubclassOf<AActor> MetahumanActor, TSoftObjectPtr<class UPhysicsAsset> _NormalPhysicsAsset, TSoftObjectPtr<class UPhysicsAsset> _RagdollPhysicsAsset)
{
	if (MetahumanActor == nullptr) return;
	if (!_NormalPhysicsAsset.ToSoftObjectPath().IsValid() || !_RagdollPhysicsAsset.ToSoftObjectPath().IsValid())
	{
		PRINT_LOG(TEXT("There is no physics assets."));
		return;
	}

	CurrentCharacterData = MetahumanActor;
	NormalPhysicsAssetSoft = _NormalPhysicsAsset;
	RagdollPhysicsAssetSoft = _RagdollPhysicsAsset;

	NormalPhysicsAssetSoft.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateWeakLambda(this, [this](const FSoftObjectPath&, UObject* LoadedObject) { NormalPhysicsAsset = Cast<UPhysicsAsset>(LoadedObject); }));
	RagdollPhysicsAssetSoft.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateWeakLambda(this, 
									  [this](const FSoftObjectPath&, UObject* LoadedObject) 
									  { 
										  RagdollPhysicsAsset = Cast<UPhysicsAsset>(LoadedObject); 
										  // ======= Turn on CCD on RagdollPhysicsAsset =======
										  for (USkeletalBodySetup* BodySetup : RagdollPhysicsAsset->SkeletalBodySetups)
										  {
											  // 이게 해골물인지 아닌지 모르겠네
											  BodySetup->DefaultInstance.SetUseCCD(true);
										  }
									  }));




	TArray<TPair<USCS_Node*, FString>> MetaHumanSkeletalMeshes;
	TArray<TPair<USCS_Node*, FString>> MetaHumanGrooms;

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(MetahumanActor);
	if (USimpleConstructionScript* SCS = BPClass->SimpleConstructionScript)
	{
		for (USCS_Node* Node : SCS->GetAllNodes())
		{
			if (Node && Node->ComponentTemplate)
			{
				USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Node->ComponentTemplate);
				if (SkeletalMesh)
				{
					MetaHumanSkeletalMeshes.Add({Node, SkeletalMesh->GetName()});
					continue;
				}

				UGroomComponent* GroomComp = Cast<UGroomComponent>(Node->ComponentTemplate);
				if (GroomComp)
				{
					MetaHumanGrooms.Add({Node, GroomComp->GetName()});
					continue;
				}
			}
		}
	}


	DynamicMaterials.Empty();
	for (auto& pair : MetaHumanSkeletalMeshes)
	{
		FString name = pair.Value;
		USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(pair.Key->ComponentTemplate);
		if (name == TEXT("Body") || name == TEXT("Body_GEN_VARIABLE"))
		{
			ChangeMesh(GetMesh(), SkeletalMesh);
		}
		else if (name == TEXT("Face") || name == TEXT("Face_GEN_VARIABLE"))
		{
			ChangeMesh(FaceMesh, SkeletalMesh);
		}
		else if (name == TEXT("Torso") || name == TEXT("Torso_GEN_VARIABLE"))
		{
			ChangeMesh(TorsoMesh, SkeletalMesh);
		}
		else if (name == TEXT("Legs") || name == TEXT("Legs_GEN_VARIABLE"))
		{
			ChangeMesh(LegsMesh, SkeletalMesh);
		}
		else if (name == TEXT("Feet") || name == TEXT("Feet_GEN_VARIABLE"))
		{
			ChangeMesh(FeetMesh, SkeletalMesh);
		}
	}
	for (auto& pair : MetaHumanGrooms)
	{
		FString name = pair.Value;
		USCS_Node* Node = pair.Key;
		if (name == TEXT("Eyelashes") || name == TEXT("Eyelashes_GEN_VARIABLE"))
		{
			ChangeGroomComponent(EyelashesGroom, TEXT("Eyelashes"), Node->ComponentClass, Node->ComponentTemplate);
		}
		else if (name == TEXT("Fuzz") || name == TEXT("Fuzze_GEN_VARIABLE"))
		{
			ChangeGroomComponent(FuzzGroom, TEXT("Fuzz"), Node->ComponentClass, Node->ComponentTemplate);
		}
		else if (name == TEXT("Eyebrows") || name == TEXT("Eyebrows_GEN_VARIABLE"))
		{
			ChangeGroomComponent(EyebrowsGroom, TEXT("Eyebrows"), Node->ComponentClass, Node->ComponentTemplate);
		}
		else if (name == TEXT("Hair") || name == TEXT("Hair_GEN_VARIABLE"))
		{
			ChangeGroomComponent(HairGroom, TEXT("Hair"), Node->ComponentClass, Node->ComponentTemplate);
		}
		else if (name == TEXT("Mustache") || name == TEXT("Mustache_GEN_VARIABLE"))
		{
			ChangeGroomComponent(MustacheGroom, TEXT("Mustache"), Node->ComponentClass, Node->ComponentTemplate);
		}
		else if (name == TEXT("Beard") || name == TEXT("Beard_GEN_VARIABLE"))
		{
			ChangeGroomComponent(BeardGroom, TEXT("Beard"), Node->ComponentClass, Node->ComponentTemplate);
		}
	}



	// Leader pose 설정
	USkeletalMeshComponent* BodyMesh = GetMesh();
	FaceMesh->SetLeaderPoseComponent(nullptr);
	TorsoMesh->SetLeaderPoseComponent(BodyMesh);
	LegsMesh->SetLeaderPoseComponent(BodyMesh);
	FeetMesh->SetLeaderPoseComponent(BodyMesh);


	// TwoPassMesh 설정 - For Cloaking
	if (TwoPassMesh_Torso)
	{
		TwoPassMesh_Torso->SetSkeletalMesh(TorsoMesh->GetSkeletalMeshAsset());
		TwoPassMesh_Torso->SetLeaderPoseComponent(BodyMesh);
		if (RefractionMaterial)
		{
			for (int32 i = 0; i < TwoPassMesh_Torso->GetNumMaterials(); ++i)
			{
				TwoPassMesh_Torso->SetMaterial(i, RefractionMaterial);
			}
		}
	}
	if (TwoPassMesh_Face)
	{
		TwoPassMesh_Face->SetSkeletalMesh(FaceMesh->GetSkeletalMeshAsset());
		TwoPassMesh_Face->SetLeaderPoseComponent(nullptr);
		if (RefractionMaterial)
		{
			for (int32 i = 0; i < TwoPassMesh_Face->GetNumMaterials(); ++i)
			{
				TwoPassMesh_Face->SetMaterial(i, RefractionMaterial);
			}
		}
	}
	if (TwoPassMesh_Legs)
	{
		TwoPassMesh_Legs->SetSkeletalMesh(LegsMesh->GetSkeletalMeshAsset());
		TwoPassMesh_Legs->SetLeaderPoseComponent(BodyMesh);
		if (RefractionMaterial)
		{
			for (int32 i = 0; i < TwoPassMesh_Legs->GetNumMaterials(); ++i)
			{
				TwoPassMesh_Legs->SetMaterial(i, RefractionMaterial);
			}
		}
	}
	if (TwoPassMesh_Feet)
	{
		TwoPassMesh_Feet->SetSkeletalMesh(FeetMesh->GetSkeletalMeshAsset());
		TwoPassMesh_Feet->SetLeaderPoseComponent(BodyMesh);
		if (RefractionMaterial)
		{
			for (int32 i = 0; i < TwoPassMesh_Feet->GetNumMaterials(); ++i)
			{
				TwoPassMesh_Feet->SetMaterial(i, RefractionMaterial);
			}
		}
	}

	if (CloakingComponent)
	{
		TArray<USkeletalMeshComponent*> BodyMeshes = { BodyMesh, FaceMesh, TorsoMesh, LegsMesh, FeetMesh, BackpackMesh };
		TArray<USkeletalMeshComponent*> TwoPassMeshes = { TwoPassMesh_Torso, TwoPassMesh_Face, TwoPassMesh_Legs, TwoPassMesh_Feet };
		TArray<UGroomComponent*> Grooms = { EyelashesGroom, FuzzGroom, EyebrowsGroom, HairGroom, MustacheGroom, BeardGroom };

		BodyMeshes.RemoveAll([](USkeletalMeshComponent* Comp) 
			{ 
				return !IsValid(Comp); 
			});

		TwoPassMeshes.RemoveAll([](USkeletalMeshComponent* Comp) 
			{ 
				return !IsValid(Comp); 
			});

		Grooms.RemoveAll([](UGroomComponent* Comp) 
			{ 
				return !IsValid(Comp); 
			});

		CloakingComponent->Initialize(BodyMeshes, TwoPassMeshes, Grooms);
	}



	// Set SuitColor
	AFHPlayerStateBase* playerState = Cast<AFHPlayerStateBase>(GetPlayerState());
	if (playerState)
	{
		ChangeSuitColor(playerState->SuitColor, playerState->SuitColorMultiplier);
	}
}

void AFHPlayerBase::ChangeMetahumanWithSelectInfo(FName Key)
{
	UFHSelectInfoIconDataAsset* SelectInfoIconDataAsset = UFHBlueprintFunctionLibrary::GetSelectInfoDataAsset();
	TSubclassOf<AActor> MetahumanActor = SelectInfoIconDataAsset->GetCharacterBP(Key);
	TSoftObjectPtr<UPhysicsAsset> NormalPhysicsSoftAsset = SelectInfoIconDataAsset->GetNormalCharacterPhysicsAsset(Key);
	TSoftObjectPtr<UPhysicsAsset> RagdollPhysicsSoftAsset = SelectInfoIconDataAsset->GetRagdollCharacterPhysicsAsset(Key);
	ChangeMetahuman(MetahumanActor, NormalPhysicsSoftAsset, RagdollPhysicsSoftAsset);

	AFHPlayerController* PlayerController = GetController<AFHPlayerController>();
	if (PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(this);
	}

}

void AFHPlayerBase::ChangeGroomComponent(TObjectPtr<UGroomComponent>& Storage, FString Name, TObjectPtr<UClass>& ComponentClass, TObjectPtr<UActorComponent>& ComponentTemplate)
{
	if (FaceMesh == nullptr)
	{
		PRINT_LOG(TEXT("Grooms must be attached on FaceMesh."));
		return;
	}

	if (Storage != nullptr)
	{
		Storage->DestroyComponent();
		Storage = nullptr;
	}

	Storage = NewObject<UGroomComponent>(FaceMesh, ComponentClass, *Name, RF_NoFlags, ComponentTemplate);
	Storage->RegisterComponent();
	Storage->AttachToComponent(FaceMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Storage->SetOwnerNoSee(true);
	Storage->SetCastHiddenShadow(true);

	for (int32 i = 0; i < Storage->GetNumMaterials(); ++i)
	{
		UMaterialInterface* GroomMat = Storage->GetMaterial(i);
		if (GroomMat)
		{
			UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(GroomMat, this);
			Storage->SetMaterial(i, MID);
			DynamicMaterials.Add(MID);
		}
	}
}

void AFHPlayerBase::ChangeMesh(USkeletalMeshComponent* LeftMesh, USkeletalMeshComponent* RightMesh)
{
	if (LeftMesh && RightMesh)
	{
		LeftMesh->SetSkeletalMesh(RightMesh->GetSkeletalMeshAsset());
		for (int32 i = 0; i < RightMesh->GetNumMaterials(); ++i)
		{
			UMaterialInterface* RightMaterial = RightMesh->GetMaterial(i);
			if (RightMaterial)
			{
				UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(RightMaterial, this);
				LeftMesh->SetMaterial(i, MID);
				DynamicMaterials.Add(MID);
			}
		}
	}

}

void AFHPlayerBase::OnPlayerStateLoadEnd(FPlayerSaveData PlayerSaveData)
{
	AFHPlayerStateBase* playerState = GetPlayerState<AFHPlayerStateBase>();
	SetNameTag(playerState->GetLicenseRank(), playerState->GetPlayerName());
	ChangeMetahumanWithSelectInfo(PlayerSaveData.PawnKey);
}

void AFHPlayerBase::PlayTalkingAnimation()
{
	if (!FaceMesh) return;
	if (!PlayerDescriptor) return;
	if (!PlayerDescriptor->AnimMontages) return;
	if (!PlayerDescriptor->AnimMontages->TalkingMontage) return;
	if (!FaceMesh->GetAnimInstance()) return;

	float startAt = FMath::RandRange(0.0f, 1.0f);
	FaceMesh->GetAnimInstance()->Montage_Play(PlayerDescriptor->AnimMontages->TalkingMontage, 1.0f, EMontagePlayReturnType::MontageLength, startAt);
}

void AFHPlayerBase::StopTalkingAnimation()
{
	if (!FaceMesh) return;
	if (!PlayerDescriptor) return;
	if (!PlayerDescriptor->AnimMontages) return;
	if (!PlayerDescriptor->AnimMontages->TalkingMontage) return;
	if (!FaceMesh->GetAnimInstance()) return;

	FaceMesh->GetAnimInstance()->Montage_Stop(0.5f, PlayerDescriptor->AnimMontages->TalkingMontage);
}

//void AFHPlayerBase::Server_PlayTalkingAnimation_Implementation()
//{
//	Multicast_PlayTalkingAnimation();
//}
//
//void AFHPlayerBase::Server_StopTalkingAnimation_Implementation()
//{
//	Multicast_StopTalkingAnimation();
//}
//
//void AFHPlayerBase::Multicast_PlayTalkingAnimation_Implementation()
//{
//	if (!FaceMesh) return;
//	if (IsLocallyControlled()) return;
//
//	float startAt = FMath::RandRange(0.0f, 1.0f);
//	FaceMesh->GetAnimInstance()->Montage_Play(PlayerDescriptor->AnimMontages->TalkingMontage, 1.0f, EMontagePlayReturnType::MontageLength, startAt);
//}
//
//void AFHPlayerBase::Multicast_StopTalkingAnimation_Implementation()
//{
//	if (!FaceMesh) return;
//	if (IsLocallyControlled()) return;
//
//	FaceMesh->GetAnimInstance()->Montage_Stop(0.5f, PlayerDescriptor->AnimMontages->TalkingMontage);
//}

//void AFHPlayerBase::AttachLightsAtCamera()
//{
//	Flashlight->AttachToComponent(CameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
//	Flashlight->SetRelativeLocation(Flashlight_NormalLocation);
//	Flashlight->SetRelativeRotation(Flashlight_NormalRotation);
//
//	//Facelight->AttachToComponent(CameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
//	//Facelight->SetRelativeLocation(Facelight_NormalLocation);
//	//Facelight->SetRelativeRotation(Facelight_NormalRotation);
//}

//void AFHPlayerBase::AttachLightsAtHead()
//{
//	Flashlight->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "head");
//	Flashlight->SetRelativeLocation(Flashlight_RagdollLocation);
//	Flashlight->SetRelativeRotation(Flashlight_RagdollRotation);
//
//	//Facelight->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "head");
//	//Facelight->SetRelativeLocation(Facelight_RagdollLocation);
//	//Facelight->SetRelativeRotation(Facelight_RagdollRotation);
//}

void AFHPlayerBase::ShowThirdPerson(float ArmLength, FVector TargetOffset)
{
	bUseControllerRotationYaw = false;

	CameraBoom->TargetArmLength = ArmLength;

	FaceMesh->SetOwnerNoSee(false);
	if (EyelashesGroom)
		EyelashesGroom->SetOwnerNoSee(false);
	if (FuzzGroom)
		FuzzGroom->SetOwnerNoSee(false);
	if (EyebrowsGroom)
		EyebrowsGroom->SetOwnerNoSee(false);
	if (HairGroom)
		HairGroom->SetOwnerNoSee(false);
	if (MustacheGroom)
		MustacheGroom->SetOwnerNoSee(false);
	if (BeardGroom)
		BeardGroom->SetOwnerNoSee(false);

	if (HasAuthority())
	{
		if (bIsTerminalOpen == false)
		{
			Server_ToggleTerminal(false);
		}
		/*FlashlightStateCache = Flashlight->IsVisible();
		if (FlashlightStateCache == true)
		{
			ServerRPC_ToggleFlashlight(false);
		}*/
	}

	CameraBoom->TargetOffset = TargetOffset;
}

void AFHPlayerBase::ShowFirstPerson(float ArmLength, FVector TargetOffset)
{
	bUseControllerRotationYaw = true;

	CameraBoom->TargetArmLength = ArmLength;
	if (GetController())
	{
		GetController()->SetControlRotation(GetActorRotation());
	}

	FaceMesh->SetOwnerNoSee(true);
	if (EyelashesGroom)
		EyelashesGroom->SetOwnerNoSee(true);
	if (FuzzGroom)
		FuzzGroom->SetOwnerNoSee(true);
	if (EyebrowsGroom)
		EyebrowsGroom->SetOwnerNoSee(true);
	if (HairGroom)
		HairGroom->SetOwnerNoSee(true);
	if (MustacheGroom)
		MustacheGroom->SetOwnerNoSee(true);
	if (BeardGroom)
		BeardGroom->SetOwnerNoSee(true);

	if (HasAuthority())
	{
		/*if (FlashlightStateCache == true)
		{
			ServerRPC_ToggleFlashlight(true);
		}*/
	}

	CameraBoom->TargetOffset = TargetOffset;
}
UInputMappingContext* AFHPlayerBase::GetInputMappingContext()
{
	return PlayerDescriptor->InputMapping.Get();
}

void AFHPlayerBase::UpdateNameTagRotation()
{
	FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	FVector TagLoc = NameTagWidgetComponent->GetComponentLocation();
	FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(TagLoc, CamLoc);
	NameTagWidgetComponent->SetWorldRotation(NewRot);
}

void AFHPlayerBase::SetNameTag(ELicenseRank Rank, FString Name)
{
	PlayerNickname = Name;

	ChangeNameTag(Rank, Name);
}

void AFHPlayerBase::SetNameTag_LicenseOnly(ELicenseRank Rank)
{
	ChangeNameTag(Rank, PlayerNickname);
}

void AFHPlayerBase::ChangeNameTag(ELicenseRank Rank, FString Name)
{
	UUserWidget* Widget = Cast<UUserWidget>(NameTagWidgetComponent->GetWidget());

	if (Widget)
	{
		UImage* LicenseImage = Cast<UImage>(Widget->GetWidgetFromName(TEXT("RankImage")));

		if (LicenseImage)
		{
			FSlateBrush brush;

			switch (Rank)
			{
			case ELicenseRank::F:
			{
				brush.SetResourceObject(PlayerDescriptor->LicenseImage_F);
				LicenseImage->SetBrush(brush);
				break;
			}
			case ELicenseRank::E:
			{
				brush.SetResourceObject(PlayerDescriptor->LicenseImage_E);
				LicenseImage->SetBrush(brush);
				break;
			}
			case ELicenseRank::D:
			{
				brush.SetResourceObject(PlayerDescriptor->LicenseImage_D);
				LicenseImage->SetBrush(brush);
				break;
			}
			case ELicenseRank::C:
			{
				brush.SetResourceObject(PlayerDescriptor->LicenseImage_C);
				LicenseImage->SetBrush(brush);
				break;
			}
			case ELicenseRank::B:
			{
				brush.SetResourceObject(PlayerDescriptor->LicenseImage_B);
				LicenseImage->SetBrush(brush);
				break;
			}
			case ELicenseRank::A:
			{
				brush.SetResourceObject(PlayerDescriptor->LicenseImage_A);
				LicenseImage->SetBrush(brush);
				break;
			}
			}
		}
	}

	if (Widget)
	{
		UTextBlock* NameText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("NameText")));
		if (NameText)
		{
			NameText->SetText(FText::FromString(Name));
		}
	}
}

void AFHPlayerBase::OnRep_PlayerNickname()
{
	AFHPlayerStateBase* playerState = GetPlayerState<AFHPlayerStateBase>();
	if (playerState)
	{
		ELicenseRank Rank = playerState->GetLicenseRank();
		ChangeNameTag(Rank, PlayerNickname);
	}
}

void AFHPlayerBase::Server_SetIsClicked_Implementation(bool isClicked)
{
	bIsClicked = isClicked;
}

void AFHPlayerBase::UpdateAimPitch()
{
	AimPitch = GetControlRotation().Pitch;
	AimPitch = FRotator::ClampAxis(AimPitch);
	if (AimPitch > 180.0f)
	{
		AimPitch -= 360.0f;
	}
}

void AFHPlayerBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFHPlayerBase, AimPitch);
	DOREPLIFETIME(AFHPlayerBase, bIsAiming);
	DOREPLIFETIME(AFHPlayerBase, bIsClicked);
	DOREPLIFETIME(AFHPlayerBase, ControlRotation);

	//DOREPLIFETIME(AFHPlayerBase, elapsedTimeFromLastAct);
	//DOREPLIFETIME(AFHPlayerBase, elapsedTimeFromLastSprinting);
	//DOREPLIFETIME(AFHPlayerBase, IsComfort);
	DOREPLIFETIME(AFHPlayerBase, CurrentEquipment);
	DOREPLIFETIME(AFHPlayerBase, PendingMesh);
	DOREPLIFETIME(AFHPlayerBase, PendingType);
	DOREPLIFETIME(ThisClass, CurrentBackpackActor);

	DOREPLIFETIME(AFHPlayerBase, PlayerNickname);
	
}

void AFHPlayerBase::SetObservingFlag_Implementation()
{
	bIsObserving = true;
}

// Server only
void AFHPlayerBase::Die_Implementation()
{
	FString CallerName = (GetInstigator()) ? GetInstigator()->GetName() : TEXT("Unknown Instigator");
	UE_LOG(LogTemp, Error, TEXT("Die_Implementation CALLED BY: %s for player %s"), *CallerName, *GetName());

	if (bIsDie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Die_Implementation: Called again, but already dead. Ignoring."));
		return;
	}

	bIsDie = true;

	DropBackpack_Internal();
	Client_Die();
	Multicast_Die();
	
	Server_SetRagdollingState(true);
	Server_EmptyGrabMeshForcefully();


	float dissolvingDelay = PlayerDescriptor->StartDissolvingDelayAfterDead;
	if (PlayerDescriptor->EnteringObserverModeDelayAfterDead < PlayerDescriptor->StartDissolvingDelayAfterDead)
	{
		dissolvingDelay = PlayerDescriptor->EnteringObserverModeDelayAfterDead;
	}

	GetWorldTimerManager().SetTimer(DissolveTimerHandle,
		FTimerDelegate::CreateLambda([this]()
	{
		Server_RequestToDissolvePlayer();
	}),
	dissolvingDelay, false);

	AFHPlayerController* FHPC = GetController<AFHPlayerController>();
	if (FHPC)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([FHPC]()
			{
				if (FHPC)
				{
					FHPC->EnterObserverMode();
				}
			});

		GetWorldTimerManager().SetTimer(ObserverTimerHandle, TimerDelegate, PlayerDescriptor->EnteringObserverModeDelayAfterDead, false);
	}

	K2_AfterDead();
}

void AFHPlayerBase::Client_Die_Implementation()
{
	bIsDie = true;
	
	if (IsLocallyControlled())
	{
		abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISDEAD);
		abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISUNMOVABLE);
		abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISUNRECOVABLE);
		abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISUNSWAPPABLE);
		abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_CANNOTOPENSTATUS);

		GetTerminalActor()->ShutOffTerminal();

		AFHVignetteEffectManager* vm = UFHBlueprintFunctionLibrary::GetVignetteEffectManager(GetWorld());
		if (vm)
		{
			vm->StopAllEffect();
			vm->bEnable = false;
		}

		AFHPlayerController* PC = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC)
		{
			if (PC->StatusUIClassInstance)
			{
				if (PC->StatusUIClassInstance->IsActivated())
				{
					PC->StatusUIClassInstance->DeactivateWidget();
					
				}
			}
		}
	}
}

void AFHPlayerBase::Multicast_Die_Implementation()
{
	bIsDie = true;
}

void AFHPlayerBase::SetCart(AFHCart* NewCart)
{
	Cart = NewCart;
	if (NewCart)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Custom, CMOVE_CartDrive);
		
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

}






void AFHPlayerBase::InitializeEquipmentState_Implementation()
{
	bEquipmentInitialized = true;

	UFHInventoryComponent* inventory = Cast<UFHInventoryComponent>(GetInventoryComponent());
	if (inventory == nullptr) return;
	UFHItemBase* currentItem = inventory->GetCurrentItem();

	const FFHItemData& itemData = currentItem ? currentItem->GetItemData() : FFHItemData{};

	CurrentEquipment.Mesh = UFHBlueprintFunctionLibrary::GetItemClassDataAsset()->GetItemMesh(itemData.MeshID);
	CurrentEquipment.MeshType = itemData.HoldingType;
	CurrentEquipment.TPHoldingDirection = itemData.TPHoldingDirection;
	CurrentEquipment.TPEquipOffset = itemData.TPEquipOffset;
	CurrentEquipment.TPEquipRotation = itemData.TPEquipRotation;
	CurrentEquipment.FPHoldingDirection = itemData.FPHoldingDirection;
	CurrentEquipment.FPEquipOffset = itemData.FPEquipOffset;
	CurrentEquipment.FPEquipRotation = itemData.FPEquipRotation;

	SetGrabMeshFromCurrentEquipment();
}

void AFHPlayerBase::Server_EmptyGrabMeshForcefully_Implementation()
{
	ForceEmptyCurrentEquipmentAndGrabMesh();
}

void AFHPlayerBase::Multicast_SetAnimHoldingTypeNoHanded_Implementation()
{
	AnimHoldingType = EItemHoldingType::NoHanded;
}

void AFHPlayerBase::OnRep_CurrentEquipment(FPlayerEquipment OldEquipment)
{
	if (bEquipmentInitialized == false)
	{
		SetGrabMeshFromCurrentEquipment();
		bEquipmentInitialized = true;
	}
	// Nobody here but us chicken!
}

void AFHPlayerBase::OnChangeHoldingItemType(UFHItemBase* NewItem)
{
	if (!HasAuthority()) return;
	if (bEquipmentInitialized == false)
	{
		PrevHoldingItem = NewItem;
		return;
	}
	if (PrevHoldingItem == NewItem)
	{
		if (NewItem == nullptr)
		{
			ForceEmptyCurrentEquipmentAndGrabMesh();
			//Multicast_SetAnimHoldingTypeNoHanded();
		}
		return;
	}

	if (!abilitySystem) return;

	const FFHItemData& itemData = NewItem ? NewItem->GetItemData() : FFHItemData{};
	EItemHoldingType PrevType = CurrentEquipment.MeshType;

	FFHGACustomData_SwitchingType* customData = new FFHGACustomData_SwitchingType;
	customData->FromType = PrevType;

	if (itemData.HoldingType == EItemHoldingType::NoHanded)
	{
		//CurrentEquipment.Mesh = nullptr;
		//PendingMesh = nullptr;
		//PendingType = EItemHoldingType::NoHanded;
		customData->ToType = EItemHoldingType::NoHanded;
		customData->PendingMesh_Ready = nullptr;
	}
	else
	{
		//CurrentEquipment.Mesh = UFHBlueprintFunctionLibrary::GetItemClassDataAsset()->GetItemMesh(itemData.MeshID);
		//PendingMesh = UFHBlueprintFunctionLibrary::GetItemClassDataAsset()->GetItemMesh(itemData.MeshID);
		//PendingType = itemData.HoldingType;
		customData->ToType = itemData.HoldingType;
		customData->PendingMesh_Ready = UFHBlueprintFunctionLibrary::GetItemClassDataAsset()->GetItemMesh(itemData.MeshID);

		CurrentEquipment.TPHoldingDirection = itemData.TPHoldingDirection;
		CurrentEquipment.TPEquipOffset = itemData.TPEquipOffset;
		CurrentEquipment.TPEquipRotation = itemData.TPEquipRotation;
		CurrentEquipment.FPHoldingDirection = itemData.FPHoldingDirection;
		CurrentEquipment.FPEquipOffset = itemData.FPEquipOffset;
		CurrentEquipment.FPEquipRotation = itemData.FPEquipRotation;
	}

	if (bEquippedItemDisappeardJustBefore == true)
	{
		bEquippedItemDisappeardJustBefore = false;
		if (NewItem == nullptr)
		{
			if (bUseMotionItemJustBefore)
			{
				bUseMotionItemJustBefore = false;
				// SetPlayerGrabMeshByCurrentEquipment 노티파이는 PendingMesh를 사용함
				PendingMesh = nullptr;
				PendingType = EItemHoldingType::NoHanded;
				return;
			}
			else
			{
				ForceEmptyCurrentEquipmentAndGrabMesh();
				PrevHoldingItem = nullptr;
				return;
			}
		}
	}

	FGameplayEventData Payload;
	Payload.EventTag = GET_GAMEPLAY_TAG_PLAYER_ABILITY_SWITCHINGTOOLS;
	Payload.TargetData.Add(customData);

	abilitySystem->HandleGameplayEvent(Payload.EventTag, &Payload);
	PrevHoldingItem = NewItem;


	// ======== Play Sound =========
	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		FName SoundID = itemData.SelectSoundID;
		SoundManager->PlaySoundAtLocationMulticastByName(SoundID, GetActorLocation());
	}
}

EItemHoldingType AFHPlayerBase::GetCurrentEquipmentHoldingType()
{
	return AnimHoldingType;
}

void AFHPlayerBase::UsePendingEquipment()
{
	CurrentEquipment.Mesh = PendingMesh;
	CurrentEquipment.MeshType = PendingType;
}

void AFHPlayerBase::SetGrabMeshFromCurrentEquipment()
{
	if (!GrabMesh) return;

	AnimHoldingType = CurrentEquipment.MeshType;

	// TSoftObjectPtr는 단순 nullptr 체크하면 경로가 유효해도 true를 반환하므로 주의
	if (CurrentEquipment.Mesh.IsNull())
	{
		GrabMesh->SetStaticMesh(nullptr);
		return;
	}

	if (CurrentEquipment.Mesh.IsPending())
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(CurrentEquipment.Mesh.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &AFHPlayerBase::OnGrabMeshLoaded));
	}
	else
	{
		bool isLocalPlayer = IsLocallyControlled();

		EItemHoldingSocketDirection Direction = isLocalPlayer ? CurrentEquipment.FPHoldingDirection : CurrentEquipment.TPHoldingDirection;
		FVector offset = isLocalPlayer ? CurrentEquipment.FPEquipOffset : CurrentEquipment.TPEquipOffset;
		FRotator rotation = isLocalPlayer ? CurrentEquipment.FPEquipRotation : CurrentEquipment.TPEquipRotation;

		switch (Direction)
		{
		case EItemHoldingSocketDirection::Nowhere:
		{
			break;
		}
		case EItemHoldingSocketDirection::LeftHand:
		{
			GrabMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, LeftGrabToolSocketName);
			break;
		}
		case EItemHoldingSocketDirection::RightHand:
		{
			GrabMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, RightGrabToolSocketName);
			break;
		}
		}

		GrabMesh->SetStaticMesh(CurrentEquipment.Mesh.Get());
		GrabMesh->SetRelativeLocationAndRotation(offset, rotation);
	}
}

void AFHPlayerBase::SetGrabMeshEmptyTemporarily()
{
	CurrentEquipment.Mesh = nullptr;
	CurrentEquipment.MeshType = EItemHoldingType::NoHanded;
	SetGrabMeshFromCurrentEquipment();
}

void AFHPlayerBase::ForceEmptyCurrentEquipmentAndGrabMesh_Implementation()
{
	CurrentEquipment.Mesh = nullptr;
	CurrentEquipment.MeshType = EItemHoldingType::NoHanded;
	PendingMesh = nullptr;
	PendingType = EItemHoldingType::NoHanded;
	SetGrabMeshFromCurrentEquipment();
}

void AFHPlayerBase::ForceUpdateCurrentEquipmentAndGrabMesh_Implementation()
{
	UsePendingEquipment();
	SetGrabMeshFromCurrentEquipment();
}

void AFHPlayerBase::OnGrabMeshLoaded()
{
	SetGrabMeshFromCurrentEquipment();
}

void AFHPlayerBase::InformPlayerEquippedItemDIsappeard()
{
	bEquippedItemDisappeardJustBefore = true;
}

void AFHPlayerBase::InformPlayerItemWhichHasUsingMotionUsed()
{
	bUseMotionItemJustBefore = true;
}

void AFHPlayerBase::PickUpBackpack(AActor* BackpackActor)
{
	AFHBackpack* TempBackpack = Cast<AFHBackpack>(BackpackActor);
	if (!TempBackpack->bIsEquipped)
	{
		CurrentBackpackActor = TempBackpack;


		// ======== Play Sound =========
		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		if (SoundManager)
		{
			SoundManager->PlaySoundAtLocationMulticastByTag(PlayerDescriptor->PickupBackpackSoundTag, GetActorLocation());
		}


		if (!HasAuthority())
		{
			ServerPickUpBackpack(BackpackActor);
		}
		OnRep_CurrentBackpackActor();
	}
}

void AFHPlayerBase::ServerPickUpBackpack_Implementation(AActor* BackpackActor)
{
	PickUpBackpack(BackpackActor);
}

void AFHPlayerBase::DropBackpackStart(const FInputActionInstance& InputActionInstance)
{
	if (CurrentBackpackActor)
	{
		bIsBackPackDrop = true;
	}
}

void AFHPlayerBase::DropBackpack(const FInputActionInstance& InputActionInstance)
{
	float Percent = 0.0f;
	if (bIsBackPackDrop)
	{
		if (BackPackDropTriggerTime <= InputActionInstance.GetElapsedTime())
		{
			DropBackpack_Internal();
		}
		else
		{
			Percent = InputActionInstance.GetElapsedTime() / BackPackDropTriggerTime;
		}
		GetPlayerStateChecked<AFHPlayerStateBase>()->OnChangeInteractProgressPercentDelegate.Broadcast(Percent);
	}

}

void AFHPlayerBase::DropBackpackEnd(const FInputActionInstance& InputActionInstance)
{
	bIsBackPackDrop = false;
}

void AFHPlayerBase::DropBackpack_Internal()
{
	CurrentBackpackActor = nullptr;
	OnRep_CurrentBackpackActor();
	ServerDropBackpack();
}

void AFHPlayerBase::ServerDropBackpack_Implementation()
{
	CurrentBackpackActor = nullptr;
	OnRep_CurrentBackpackActor();
}

AFHBackpack* AFHPlayerBase::GetCurrentBackpackActor() const
{
	return CurrentBackpackActor;
}

void AFHPlayerBase::OnRep_CurrentBackpackActor()
{
	if (CurrentBackpackActor)
	{
		UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(CurrentBackpackActor->GetRootComponent());
		if (ensure(PrimComp))
		{
			PrimComp->SetSimulatePhysics(false);
			PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PrimComp->SetVisibility(false);

			CurrentBackpackActor->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				BackPackSocketName
			);
		}
		USkeletalMeshComponent* BackpackMeshComponent = Cast<USkeletalMeshComponent>(Execute_FindAssetData(this, CurrentBackpackActor->BackpackName));


		if (ensure(BackpackMeshComponent))
		{
			USkeletalMesh* BackpackMeshAsset = BackpackMeshComponent->GetSkeletalMeshAsset();
			BackpackMesh->SetSkeletalMesh(BackpackMeshAsset);
		}
		
		CurrentBackpackActor->SetActorRelativeRotation(FRotator::ZeroRotator);
		CurrentBackpackActor->bIsEquipped = true;
		CurrentBackpackActor->PickedUpActor = this;
		Cast<UFHInventoryComponent>(CurrentBackpackActor->GetInventoryComponent())->RefreshASCInit();
		CurrentBackpackActor->Tags.Add(UFHBlueprintFunctionLibrary::Name_ExcludeFromSave);
	}
	else if(CurrentBackpackActorCache)
	{

		// ======== Play Sound =========
		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		if (SoundManager)
		{
			SoundManager->PlaySoundAtLocationLocallyByTag(PlayerDescriptor->DropBackpackSoundTag, GetActorLocation());
		}


		const FVector DropSocketLocation = GetMesh()->GetSocketLocation(BackPackDropSocketName);
		CurrentBackpackActorCache->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentBackpackActorCache->SetActorLocation(DropSocketLocation);
		CurrentBackpackActorCache->bIsEquipped = false;
		CurrentBackpackActorCache->PickedUpActor = nullptr;
		Cast<UFHInventoryComponent>(CurrentBackpackActorCache->GetInventoryComponent())->RefreshASCInit();
		CurrentBackpackActorCache->Tags.Remove(UFHBlueprintFunctionLibrary::Name_ExcludeFromSave);

		UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(CurrentBackpackActorCache->GetRootComponent());
		if (ensure(PrimComp))
		{
			PrimComp->SetSimulatePhysics(true);
			PrimComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			PrimComp->SetVisibility(true);
		}
		BackpackMesh->SetSkeletalMesh(nullptr);
	}

	CurrentBackpackActorCache = CurrentBackpackActor;
	AFHPlayerStateBase* FHPS = GetPlayerState<AFHPlayerStateBase>();
	if (FHPS)
	{
		FHPS->SetCurrentBackpackActorCache(CurrentBackpackActor);
	}
	
}












void AFHPlayerBase::Server_RequestToDissolvePlayer_Implementation()
{
	Multicast_StartDissolvePlayer();
}

void AFHPlayerBase::Multicast_StartDissolvePlayer_Implementation()
{
	//DynamicMaterials.Empty();
	
	/*CreateDynamicMaterials(GetMesh());
	if (FaceMesh) CreateDynamicMaterials(FaceMesh);
	if (TorsoMesh) CreateDynamicMaterials(TorsoMesh);
	if (LegsMesh) CreateDynamicMaterials(LegsMesh);
	if (FeetMesh) CreateDynamicMaterials(FeetMesh);
	if (EyelashesGroom) CreateDynamicMaterials(EyelashesGroom);
	if (FuzzGroom) CreateDynamicMaterials(FuzzGroom);
	if (EyebrowsGroom) CreateDynamicMaterials(EyebrowsGroom);
	if (HairGroom) CreateDynamicMaterials(HairGroom);
	if (MustacheGroom) CreateDynamicMaterials(MustacheGroom);
	if (BeardGroom) CreateDynamicMaterials(BeardGroom);*/

	if (DissolveFloatCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName{"UpdateDissolveScaleOfDynamicMaterials"});

		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName{ "OnPlayerDissolvedCompletely"});

		DissolveTimeLine.AddInterpFloat(DissolveFloatCurve, ProgressFunction);
		DissolveTimeLine.SetTimelineFinishedFunc(FinishedFunction);
		DissolveTimeLine.SetLooping(false);
		DissolveTimeLine.PlayFromStart();
		// TimeLineLength는 커브의 마지막 키까지로 자동 설정 된다고 함.

		bIsDissolving = true;
	}
}

void AFHPlayerBase::UpdateDissolveScaleOfDynamicMaterials(float Value)
{
	float NewScale = FMath::Lerp(-1.0f, 1.0f, Value);

	if (bIsDissolvingAndRemoveGrooms == false && NewScale > DissolvingRomveGroomThreshold)
	{
		if (EyelashesGroom)
		{
			EyelashesGroom->SetVisibility(false);
			EyelashesGroom->SetCastShadow(false);
		}
		if (FuzzGroom)
		{
			FuzzGroom->SetVisibility(false);
			FuzzGroom->SetCastShadow(false);
		}
		if (EyebrowsGroom)
		{
			EyebrowsGroom->SetVisibility(false);
			EyebrowsGroom->SetCastShadow(false);
		}
		if (HairGroom)
		{
			HairGroom->SetVisibility(false);
			HairGroom->SetCastShadow(false);
		}
		if (MustacheGroom)
		{
			MustacheGroom->SetVisibility(false);
			MustacheGroom->SetCastShadow(false);
		}
		if (BeardGroom)
		{
			BeardGroom->SetVisibility(false);
			BeardGroom->SetCastShadow(false);
		}

		bIsDissolvingAndRemoveGrooms = true;
	}

	for (TObjectPtr<class UMaterialInstanceDynamic> mat : DynamicMaterials)
	{
		mat->SetScalarParameterValue(DissolveParameterName, NewScale);
	}
}

void AFHPlayerBase::OnPlayerDissolvedCompletely()
{
	// 여기서 메쉬 안 보이게
	GetMesh()->SetVisibility(false);
	if (FaceMesh) FaceMesh->SetVisibility(false);
	if (TorsoMesh) TorsoMesh->SetVisibility(false);
	if (LegsMesh) LegsMesh->SetVisibility(false);
	if (FeetMesh) FeetMesh->SetVisibility(false);
	//if (BackpackMesh) BackpackMesh->SetVisibility(false);
	if (EyelashesGroom) EyelashesGroom->SetVisibility(false);
	if (FuzzGroom) FuzzGroom->SetVisibility(false);
	if (EyebrowsGroom) EyebrowsGroom->SetVisibility(false);
	if (HairGroom) HairGroom->SetVisibility(false);
	if (MustacheGroom) MustacheGroom->SetVisibility(false);
	if (BeardGroom) BeardGroom->SetVisibility(false);

	if (TwoPassMesh_Torso) TwoPassMesh_Torso->SetVisibility(false);
	if (TwoPassMesh_Face) TwoPassMesh_Face->SetVisibility(false);
	if (TwoPassMesh_Legs) TwoPassMesh_Legs->SetVisibility(false);
	if (TwoPassMesh_Feet) TwoPassMesh_Feet->SetVisibility(false);


	PRINT_LOG(TEXT("Player dead..."));
}

void AFHPlayerBase::SetObserverInputMode_Implementation(bool bIsObserverMode)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
		return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	UInputMappingContext* PlayerInputMapping = CurrentIMC;
	UInputMappingContext* ObserverModeInputMapping = PlayerDescriptor->ObserverModeInputMapping.Get();

	if (bIsObserverMode)
	{
		Subsystem->RemoveMappingContext(PlayerInputMapping);
		Subsystem->AddMappingContext(ObserverModeInputMapping, 1);
	}
	else
	{
		Subsystem->RemoveMappingContext(ObserverModeInputMapping);
		Subsystem->AddMappingContext(PlayerInputMapping, 1);
	}
}

void AFHPlayerBase::Multicast_PlayAlchemistSkillEffect_Implementation(FVector SpawnLocation)
{
	K2_PlayAlchemistSkillEffect(SpawnLocation);
}

void AFHPlayerBase::OnStunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		if (HasAuthority())
		{
			ActivateStunEffect();
		}

		if (IsLocallyControlled())
		{
			if (abilitySystem)
			{
				FGameplayTagContainer StunTagContainer(GET_GAMEPLAY_TAG_DEBUFF_STUN);
				TArray<FActiveGameplayEffectHandle> StunEffectHandles = abilitySystem->GetActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(StunTagContainer));
				float MaxDuration = 0.0f;
				for (const FActiveGameplayEffectHandle& Handle : StunEffectHandles)
				{
					const FActiveGameplayEffect* Effect = abilitySystem->GetActiveGameplayEffect(Handle);
					if (Effect)
					{
						MaxDuration = FMath::Max(MaxDuration, Effect->GetDuration());
					}
				}

				if (MaxDuration > 0.f)
				{
					Client_PlayStunVignette(MaxDuration);
				}
			}
		}
	}
}

void AFHPlayerBase::Client_PlayStunVignette_Implementation(float Duration)
{
	if (IsLocallyControlled())
	{
		AFHVignetteEffectManager* VignetteManager = UFHBlueprintFunctionLibrary::GetVignetteEffectManager(GetWorld());
		if (VignetteManager)
		{
			VignetteManager->PlayEffect(EVignetteEffectType::Glitch, Duration);
		}
	}
}

void AFHPlayerBase::Multicast_PlayStopperSkillEffect_Implementation(FVector SpawnLocation, float Radius)
{
	K2_PlayStopperSkillEffect(SpawnLocation, Radius);
}

void AFHPlayerBase::Multicast_PlaySeekerSkillEffect_Implementation(FVector SpawnLocation, float Radius)
{
	K2_PlaySeekerSkillEffect(SpawnLocation, Radius);
}

void AFHPlayerBase::SetTwoPassMeshesVisible(bool IsVisible)
{
	TwoPassMesh_Torso->SetVisibility(IsVisible);
	TwoPassMesh_Face->SetVisibility(IsVisible);
	TwoPassMesh_Legs->SetVisibility(IsVisible);
	TwoPassMesh_Feet->SetVisibility(IsVisible);
}

void AFHPlayerBase::OnCloakTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		Multicast_BeginCloaking();
	}
	else
	{
		Multicast_EndCloaking();
	}
}

void AFHPlayerBase::Multicast_BeginCloaking_Implementation()
{
	if (CloakingComponent)
	{
		CloakingComponent->BeginCloaking();
		UE_LOG(LogTemp, Log, TEXT("Player Begin Cloaking"));
	}
}

void AFHPlayerBase::Multicast_EndCloaking_Implementation()
{
	if (CloakingComponent)
	{
		CloakingComponent->EndCloaking();
		UE_LOG(LogTemp, Log, TEXT("Player End Cloaking"));
	}
}
