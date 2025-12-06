// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FHPlayerAnimMontageDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHPlayerAnimMontageDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Attack")
	TObjectPtr<class UAnimMontage> ThirdPersonAttackAnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Attack")
	TObjectPtr<class UAnimMontage> FirstPersonAttackAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Swap")
	TObjectPtr<class UAnimMontage> ThirdPersonSwitchingType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Swap")
	TObjectPtr<class UAnimMontage> FirstPersonSwitchingType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Throw")
	TObjectPtr<class UAnimMontage> ThirdPersonThrowAnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Throw")
	TObjectPtr<class UAnimMontage> FirstPersonThrowAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Common")
	TObjectPtr<class UAnimMontage> PlayerPunch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Common")
	TObjectPtr<class UAnimMontage> UseVariousItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Common")
	TObjectPtr<class UAnimMontage> PlayerEmote;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Face")
	TObjectPtr<class UAnimMontage> TalkingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Skill")
	TObjectPtr<class UAnimMontage> ThirdPersonUseSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Skill")
	TObjectPtr<class UAnimMontage> FirstPersonUseSkillMontage;
};
