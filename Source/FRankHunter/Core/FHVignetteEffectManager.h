// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/ManagerActor.h"
#include "FHVignetteEffectManager.generated.h"

UENUM(BlueprintType)
enum class EVignetteEffectType : uint8
{
	Hurt			UMETA(DisplayName = "Hurt"),
	Speedy			UMETA(DisplayName = "Speedy"),
	Healing			UMETA(DisplayName = "Healing"),
	Shield			UMETA(DisplayName = "Shield"),
	Glitch			UMETA(DisplayName = "Glitch"),
	Drunk			UMETA(DisplayName = "Drunk"),
	Goo				UMETA(DisplayName = "Goo"),
	Poisoned		UMETA(DisplayName = "Poisoned"),
}; 

UCLASS()
class FRANKHUNTER_API AFHVignetteEffectManager : public AManagerActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bEnable : 1{ true };

public:
	virtual FName GetManagerName() const { return TEXT("VignetteManager"); }
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayEffect(EVignetteEffectType EffectType, float Duration);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StopEffect(EVignetteEffectType EffectType);

	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StopAllEffect();
};
