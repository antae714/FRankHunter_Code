// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayTagContainer.h"
#include "FHPlayMontageCue.generated.h"

UENUM(BlueprintType)
enum class EPlayMontageCueMode : uint8
{
	Play 					UMETA(DisplayName = "Play"),
	Jump 					UMETA(DisplayName = "Jump")
};

UENUM(BlueprintType)
enum class EPlayMontageCueTarget : uint8
{
	AllPlayer					UMETA(DisplayName = "All Player"),
	OnlyLocalPlayer				UMETA(DisplayName = "Only Local Player"),
	ExceptLocalPlayer			UMETA(DisplayName = "Except Local Player"),
	DiffrentLocalAndOthers		UMETA(DisplayName = "Diffrent Local and Others")
};

UCLASS()
class FRANKHUNTER_API UFHPlayMontageCue : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	EPlayMontageCueMode Mode{ EPlayMontageCueMode::Play };
	UPROPERTY(EditAnywhere)
	EPlayMontageCueTarget Target{ EPlayMontageCueTarget::AllPlayer };
	
	UPROPERTY(EditAnywhere)
	uint8 bStopAnyOtherMontageForcefully : 1{ true };

	UPROPERTY(EditAnywhere)
	uint8 bEnableBlending : 1{ true };
	UPROPERTY(EditAnywhere)
	float BlendTime{ 0.2f };
protected:
	FAlphaBlendArgs BlendingArg;

public:
	UPROPERTY(EditAnywhere)
	uint8 bConsiderPlayerActionSpeed : 1{ true };


	// =============================
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target != EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	TObjectPtr<class UAnimMontage> AnimMontage;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target != EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	FName SectionName;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target != EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	float PlayRate{ 1.0f };


	// =============================
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target == EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	TObjectPtr<class UAnimMontage> AnimMontageLocal;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target == EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	FName SectionNameLocal;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target == EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	float PlayRateLocal{ 1.0f };
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target == EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	TObjectPtr<class UAnimMontage> AnimMontageOther;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target == EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	FName SectionNameOther;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Target == EPlayMontageCueTarget::DiffrentLocalAndOthers", EditConditionHides))
	float PlayRateOther{ 1.0f };

public:
	virtual bool HandlesEvent(EGameplayCueEvent::Type EventType) const override;

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

	void PlayMontage(class UAnimInstance* AnimInstance, class UAnimMontage* AnimMontage, FName SectionName, float PlayRate);
};
