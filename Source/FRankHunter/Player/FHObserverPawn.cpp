// Copyright F Rank Hunter. All Rights Reserved.


#include "Player/FHObserverPawn.h"
#include "Player/FHPlayerBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h" 
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Player/Input/InputConfigData.h"
#include "EnhancedInputSubsystems.h"
#include "Player/FHPlayerDescriptor.h"
#include "Core/FHPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "UI/FHObserverHud.h"
#include "Core/FHGateGameStateBase.h"
#include "Core/FHPlayerStateBase.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"

DEFINE_LOG_CATEGORY(LogFHObserver);

AFHObserverPawn::AFHObserverPawn()
{
    PrimaryActorTick.bCanEverTick = true;

	//GetMovementComponent()->SetUpdatedComponent(nullptr);
	bAddDefaultMovementBindings = false;

    bReplicates = true;

    CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
    CameraSpringArm->SetupAttachment(RootComponent);
    CameraSpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
    CameraSpringArm->TargetArmLength = 1000.0f;
    CameraSpringArm->bEnableCameraLag = true;
    CameraSpringArm->bUsePawnControlRotation = true;
    CameraSpringArm->bDoCollisionTest = true;
    CameraSpringArm->ProbeChannel = ECC_Camera;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

    UPrimitiveComponent* CollisionComp = GetCollisionComponent();
    if (CollisionComp)
    {
        CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    }

    SetActorEnableCollision(false);
}

void AFHObserverPawn::SetObservingTarget(APawn* NewTarget)
{
    ObservingTarget = Cast<AFHPlayerBase>(NewTarget);
}

APawn* AFHObserverPawn::GetObservingTarget() const
{
    return Cast<APawn>(ObservingTarget.Get());
}

void AFHObserverPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsLocallyControlled())
    {
        if (ObservingTarget.IsValid())
        {
            SetActorLocation(ObservingTarget->GetActorLocation());
        }
        else
        {
            ElapsedTime += DeltaTime;
            if (ElapsedTime >= CheckObservingTargetInterval)
            {
                ElapsedTime -= CheckObservingTargetInterval;
                ObserveNext();
            }
        }
    }
}

void AFHObserverPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    UInputMappingContext* InputMapping = PlayerDescriptor->ObserverModeInputMapping;
    UInputConfigData* InputActions = PlayerDescriptor->InputActions;
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

    bool bIsValidInputComponent = IsValid(EnhancedInputComponent);
	bool bIsValidPlayerController = IsValid(PlayerController);
	bool bIsValidInputActions = IsValid(InputActions);

    if (bIsValidInputComponent && bIsValidPlayerController && bIsValidInputActions)
    {
        UE_LOG(LogFHObserver, Warning, TEXT("SetupPlayerInputComponent called and InputConfig is valid."));
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

        if (Subsystem)
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(InputMapping, 0);
        }


        EnhancedInputComponent->BindAction(InputActions->InputObserveTurn, ETriggerEvent::Triggered, this, &AFHObserverPawn::Turn);
        EnhancedInputComponent->BindAction(InputActions->InputObserveLookUp, ETriggerEvent::Triggered, this, &AFHObserverPawn::LookUp);
        EnhancedInputComponent->BindAction(InputActions->InputObserveZoom, ETriggerEvent::Triggered, this, &AFHObserverPawn::Zoom);

        EnhancedInputComponent->BindAction(InputActions->InputObserveNext, ETriggerEvent::Started, this, &AFHObserverPawn::ObserveNext);
        EnhancedInputComponent->BindAction(InputActions->InputObservePrev, ETriggerEvent::Started, this, &AFHObserverPawn::ObservePrev);
    }
}

void AFHObserverPawn::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    UE_LOG(LogFHObserver, Warning, TEXT("AFHObserverPawn has been possessed by: %s"), *NewController->GetName());


    //GetWorldTimerManager().SetTimer(InitHudTimer, this, &AFHObserverPawn::DelayedInitializeHUD, 0.2f, false);

    Client_ActivateObserverInput();
}

void AFHObserverPawn::UnPossessed()
{
    Super::UnPossessed();

    //InitHudTimer.Invalidate();

    Client_RemoveHUD();
}

void AFHObserverPawn::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    Super::EndPlay(EndPlayReason);
}

void AFHObserverPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFHObserverPawn, ObservingTarget);
    DOREPLIFETIME(AFHObserverPawn, ObserverHudClass);
}

void AFHObserverPawn::DelayedInitializeHUD()
{
    GetWorldTimerManager().SetTimer(InitHudTimer, this, &AFHObserverPawn::Client_InitializeHUD, 0.5f, false);
}

void AFHObserverPawn::Client_InitializeHUD_Implementation()
{
    if (!IsValid(this))
    {
        return;
    }

    if (bHasBeenInitialized)
    {
        UE_LOG(LogFHObserver, Warning, TEXT("[Client %d] Already initialized, returning."), UE::GetPlayInEditorID());
        return;
    }
    bHasBeenInitialized = true;

    AFHPlayerController* PC = GetController<AFHPlayerController>();
    if (PC == nullptr)
    {
        UE_LOG(LogFHObserver, Error, TEXT("[Client %d] PlayerController is NULL!"), UE::GetPlayInEditorID());
        return;
    }

    if (ObserverHudClass == nullptr)
    {
        UE_LOG(LogFHObserver, Error, TEXT("[Client %d] ObserverHudClass is NULL! Cannot create widget."), UE::GetPlayInEditorID());
        return;
    }

    if (ObserverHudInstance != nullptr)
    {
        UE_LOG(LogFHObserver, Warning, TEXT("[Client %d] ObserverHudInstance already exists!"), UE::GetPlayInEditorID());
        return;
    }

    if (PC->GetLocalPlayer())
    {
        ObserverHudInstance = CreateWidget<UFHObserverHud>(PC, ObserverHudClass);
        if (ObserverHudInstance)
        {
            ObserverHudInstance->AddToViewport();
            ObserverHudInstance->UpdateRescuedInfo();
            UE_LOG(LogFHObserver, Warning, TEXT("[Client %d] Observer HUD CREATED AND ADDED to viewport!"), UE::GetPlayInEditorID());
        }
        else
        {
            UE_LOG(LogFHObserver, Error, TEXT("[Client %d] CreateWidget FAILED!"), UE::GetPlayInEditorID());
        }
    }

    AFHGateGameStateBase* GateGameState = GetWorld()->GetGameState<AFHGateGameStateBase>();
    if (GateGameState)
    {
        GateGameState->OnTimerTickEvent.AddDynamic(this, &AFHObserverPawn::OnGameStateTimeChanged);
        GateGameState->OnCoreCountChangedEvent.AddDynamic(this, &AFHObserverPawn::OnGameStateCoreChanged);
        GateGameState->OnQuotaInfoChangedEvent.AddDynamic(this, &AFHObserverPawn::OnGameStateQuotaChanged);

        OnGameStateTimeChanged(GateGameState->TimeLimit);
        OnGameStateCoreChanged(GateGameState->GetCurrentCoreCount());
        OnGameStateQuotaChanged(GateGameState->PlayerMoney, GateGameState->RequiredMoney);
    }
}

void AFHObserverPawn::Client_ActivateObserverInput_Implementation()
{
    APlayerController* PC = GetController<APlayerController>();
    if (PC == nullptr)
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
    if (Subsystem == nullptr)
    {
        return;
    }

    if (PlayerDescriptor == nullptr)
    {
        return;
    }

    UInputMappingContext* ObserverIMC = PlayerDescriptor->ObserverModeInputMapping.Get();
    if (ObserverIMC == nullptr)
    {
        return;
    }

    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(ObserverIMC, 0);

    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = false;

    UE_LOG(LogFHObserver, Warning, TEXT("Successfully activated observer input on client."));
}

void AFHObserverPawn::Client_RemoveHUD_Implementation()
{
    if (IsValid(ObserverHudInstance) && ObserverHudInstance->IsInViewport())
    {
        ObserverHudInstance->RemoveFromParent();
        ObserverHudInstance = nullptr;
    }
}

void AFHObserverPawn::OnGameStateTimeChanged(int32 NewTime)
{
    AFHGateGameStateBase* GateGameState = GetWorld()->GetGameState<AFHGateGameStateBase>();
    if (GateGameState)
    {
        if (ObserverHudInstance)
        {
            FText text = UFHBlueprintFunctionLibrary::GetLocalizeText(TEXT("OBS_HUD_TIME_REMAINING"));
            FString ModifiedTime = GateGameState->GetModifiedStringFromTimeLimit();
            ObserverHudInstance->UpdateTimeText(text, ModifiedTime);
        }
    }
}

void AFHObserverPawn::OnGameStateCoreChanged(int32 NewCoreCount)
{
    if (ObserverHudInstance)
    {
        FText text = UFHBlueprintFunctionLibrary::GetLocalizeText(TEXT("OBS_HUD_CORE_LIVE"));
        ObserverHudInstance->UpdateCoreText(text, NewCoreCount);
    }
}

void AFHObserverPawn::OnGameStateQuotaChanged(int32 CurrentMoney, int32 RequiredMoney)
{
    // TODO : Localizing
    
    if (ObserverHudInstance)
    {
        ObserverHudInstance->UpdateQuotaText(CurrentMoney, RequiredMoney);
    }
}

void AFHObserverPawn::OnRep_ObservingTargetChanged()
{
    // TODO : Localizing

    if (ObserverHudInstance)
    {
        ObserverHudInstance->UpdateObservingTargetName(ObservingTarget.Get());
    }
}

void AFHObserverPawn::Turn(const FInputActionValue& Value)
{
    const float TurnValue = Value.Get<float>();
    AddControllerYawInput(TurnValue);
}

void AFHObserverPawn::LookUp(const FInputActionValue& Value)
{
    const float LookUpValue = Value.Get<float>();
    AddControllerPitchInput(-LookUpValue);
}

void AFHObserverPawn::Zoom(const FInputActionValue& Value)
{
    const float ZoomValue = Value.Get<float>();
    if (FMath::Abs(ZoomValue) > 0.f)
    {
        float NewArmLength = CameraSpringArm->TargetArmLength - (ZoomValue * ZoomStep);
        CameraSpringArm->TargetArmLength = FMath::Clamp(NewArmLength, MinZoomDistance, MaxZoomDistance);
    }
}

void AFHObserverPawn::ObserveNext()
{
    AFHPlayerController* FHPC = GetController<AFHPlayerController>();
    if (FHPC)
    {
        FHPC->RequestChangeViewTarget(this, true);
    }
}

void AFHObserverPawn::ObservePrev()
{
    AFHPlayerController* FHPC = GetController<AFHPlayerController>();
    if (FHPC)
    {
        FHPC->RequestChangeViewTarget(this, false);
    }
}
