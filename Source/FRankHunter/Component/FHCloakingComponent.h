// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FHCloakingComponent.generated.h"

class UGroomComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHCloakingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFHCloakingComponent();

	void Initialize(const TArray<USkeletalMeshComponent*>& InOriginalMeshes, const TArray<USkeletalMeshComponent*>& InTwoPassMeshes, const TArray<UGroomComponent*>& InGrooms);

	void BeginCloaking();
	void EndCloaking();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category = "Cloaking Settings")
	float CloakAmountMax = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Cloaking Settings")
	float CloakAmountMin = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Cloaking Settings")
	float CloakingInterpSpeed = 2.0f;

protected:
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> OriginalMIDs;

	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> TwoPassMIDs;

	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> OriginalMeshComponents;

	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> TwoPassMeshComponents;

	UPROPERTY()
	TArray<TObjectPtr<class UGroomComponent>> GroomComponents;

	UPROPERTY()
	TObjectPtr<UGroomComponent> HairGroom;

private:
	bool bIsInitialized = false;
	float CurrentCloakAmount = 0.0f;
	float TargetCloakAmount = 0.0f;
};
