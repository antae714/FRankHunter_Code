// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/GameTeamTypes.h"
#include "GameplayTagContainer.h"
#include "FHPlayerDescriptor.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHPlayerDescriptor : public UDataAsset
{
	GENERATED_BODY()

public:
	// =========== Common ===========
	UPROPERTY(EditAnywhere, Category = "Common")
	ETeamID TeamID;

	// =========== Camera ===========
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float PitchLimitAngle{ 60.0f };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float AdditionalViewRayStartDistanceFromCamera = 15.0f;


	// =========== Interact ===========
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	float GetInfrontActorSweepRadius{ 10.0f };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact")
	float MaxInteractDistance = 9999.f;
	UPROPERTY(EditAnywhere, Category = "Interact")
	FGameplayTagContainer BlockInteractTags;
	UPROPERTY(EditAnywhere, Category = "Interact")
	float CheckInteractableObjectInterval{ 0.1f };
	UPROPERTY(EditAnywhere, Category = "Interact")
	float CheckInteractableObjectRadius{ 100.0f };
	UPROPERTY(EditAnywhere, Category = "Interact")
	float HighlightInteractableObjectTime{ 1.0f };


	// =========== Falling ===========
	UPROPERTY(EditAnywhere, Category = "Falling")
	uint8 bEnableFallingDamage : 1{ true };
	UPROPERTY(EditAnywhere, Category = "Falling")
	float MinFallingDamageVelocity{ 1200.0f };
	UPROPERTY(EditAnywhere, Category = "Falling")
	float FallingDamageExponent{ 4.0f };
	UPROPERTY(EditAnywhere, Category = "Falling")
	float FallingDamagePerScore{ 10.0f };

	// =========== Animation ===========
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UFHPlayerAnimMontageDataAsset> AnimMontages;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<class UFHPlayerAnimInstance> ThirdPersonAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<class UFHPlayerAnimInstance> FirstPersonAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float SwitchingToolMontagePlayRate{ 1.5f };


	// =========== Input ===========
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMapping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputConfigData> InputActions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> ObserverModeInputMapping;

	// =========== Focusing ===========
	UPROPERTY(EditAnywhere, Category = "Focusing")
	float FocusingInterpSpeed{ 3.0f };
	UPROPERTY(EditAnywhere, Category = "Focusing")
	float FocusingFovMult{ 0.7f };

	// ========== Sounds ===========
	UPROPERTY(EditAnywhere, Category = "Sounds")
	FGameplayTag FlashOnSoundTag;
	UPROPERTY(EditAnywhere, Category = "Sounds")
	FGameplayTag FlashOffSoundTag;
	UPROPERTY(EditAnywhere, Category = "Sounds")
	FGameplayTag PickupBackpackSoundTag;
	UPROPERTY(EditAnywhere, Category = "Sounds")
	FGameplayTag DropBackpackSoundTag;
	UPROPERTY(EditAnywhere, Category = "Sounds")
	FGameplayTag OpenTerminalSoundTag;
	UPROPERTY(EditAnywhere, Category = "Sounds")
	FGameplayTag CloseTerminalSoundTag;
	UPROPERTY(EditAnywhere, Category = "Sounds")
	FGameplayTag FallingDamageSoundTag;
	UPROPERTY(EditAnywhere, Category = "Sounds")
	FGameplayTag BeChasedByCreatureSoundTag;


	// ========== VFX ===========
	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> BloodSplashNiagara;
	UPROPERTY(EditAnywhere, Category = "VFX")
	TSubclassOf<class ANiagaraActor> StunNiagaraActor;



	// ========== Move ===========
	UPROPERTY(EditAnywhere, Category = "Move")
	float SideStepSpeedMult{ 0.75f };
	UPROPERTY(EditAnywhere, Category = "Move")
	float BackStepSpeedMult{ 0.5f };
	UPROPERTY(EditAnywhere, Category = "Move")
	TSubclassOf<class UGameplayAbility> MoveGameplayAbility;


	// ========== Dead ===========
	UPROPERTY(EditAnywhere, Category = "Dead")
	float StartDissolvingDelayAfterDead{ 4.0f };
	UPROPERTY(EditAnywhere, Category = "Dead")
	float EnteringObserverModeDelayAfterDead{ 7.0f };


	// ========== NameTag ============
	UPROPERTY(EditAnywhere, Category = "NameTag")
	TObjectPtr<UTexture2D> LicenseImage_F;
	UPROPERTY(EditAnywhere, Category = "NameTag")
	TObjectPtr<UTexture2D> LicenseImage_E;
	UPROPERTY(EditAnywhere, Category = "NameTag")
	TObjectPtr<UTexture2D> LicenseImage_D;
	UPROPERTY(EditAnywhere, Category = "NameTag")
	TObjectPtr<UTexture2D> LicenseImage_C;
	UPROPERTY(EditAnywhere, Category = "NameTag")
	TObjectPtr<UTexture2D> LicenseImage_B;
	UPROPERTY(EditAnywhere, Category = "NameTag")
	TObjectPtr<UTexture2D> LicenseImage_A;

	// ========== Flashlight ============
	UPROPERTY(EditAnywhere, Category = "Flashlight")
	float BatteryReduceInterval_FlashlightOn{ 3.0f };
	UPROPERTY(EditAnywhere, Category = "Flashlight")
	float BatteryReduceInterval_FlashlightOff{ 4.0f };


	// =========== Test ===========
	UPROPERTY(EditAnywhere, Category="Test")
	TSubclassOf<class UGameplayEffect> TestEffect;
};
