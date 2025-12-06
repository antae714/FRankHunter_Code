// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "FHObserverPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputConfigData;
class UFHPlayerDescriptor;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHObserverPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:
	AFHObserverPawn();

    void SetObservingTarget(APawn* NewTarget);

    UFUNCTION(BlueprintPure, Category = "Observer")
    APawn* GetObservingTarget() const;

protected:
    UPROPERTY(EditDefaultsOnly)
    float CheckObservingTargetInterval{ 0.5f };

    float ElapsedTime{ 0.0f };
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    FTimerHandle InitHudTimer;
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    bool bHasBeenInitialized{ false };

    UPROPERTY(ReplicatedUsing = "OnRep_ObservingTargetChanged")
    TWeakObjectPtr<class AFHPlayerBase> ObservingTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraSpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UFHPlayerDescriptor> PlayerDescriptor;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "UI")
    TSubclassOf<class UFHObserverHud> ObserverHudClass;

    UPROPERTY()
    TObjectPtr<UFHObserverHud> ObserverHudInstance;

public:
    void DelayedInitializeHUD();

protected:
    UFUNCTION(Client, Reliable)
    void Client_InitializeHUD();
    void Client_InitializeHUD_Implementation();

    UFUNCTION(Client, Reliable)
    void Client_ActivateObserverInput();
    void Client_ActivateObserverInput_Implementation();

    UFUNCTION(Client, Reliable)
    void Client_RemoveHUD();
    void Client_RemoveHUD_Implementation();

    UFUNCTION()
    void OnGameStateTimeChanged(int32 NewTime);

    UFUNCTION()
    void OnGameStateCoreChanged(int32 NewCoreCount);

    UFUNCTION()
    void OnGameStateQuotaChanged(int32 CurrentMoney, int32 RequiredMoney);

    UFUNCTION()
    void OnRep_ObservingTargetChanged();

private:
    void Turn(const FInputActionValue& Value);
    void LookUp(const FInputActionValue& Value);

    void Zoom(const FInputActionValue& Value);
    void ObserveNext();
    void ObservePrev();

    UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float MinZoomDistance = 150.f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float MaxZoomDistance = 1000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float ZoomStep = 100.f;
};

DECLARE_LOG_CATEGORY_EXTERN(LogFHObserver, Log, All);