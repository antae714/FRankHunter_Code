// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Field/FieldSystemActor.h"
#include "FHDestructionFieldSystemActor.generated.h"

class USphereComponent;
class UUniformInteger;
class UOperatorField;
class URadialFalloff;
class UFieldSystemMetaDataIteration;
class URadialVector;
class UNoiseField;
class UCullingField;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHDestructionFieldSystemActor : public AFieldSystemActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	float DamageMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	float ForcesMagnitude;

	/** if FieldRadius <= 0 use collisionRadius*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	float FieldRadius;

public:
	UFUNCTION(BlueprintCallable)
	void Enable(float LifeTime = 0.2f);

	UFUNCTION(BlueprintCallable)
	void Disable();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> AnchorSphere;


public:
	AFHDestructionFieldSystemActor();

protected:
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;


private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URadialFalloff> DamageRadialFalloff;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNoiseField> ForceNoiseField;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URadialVector> ForceDirection;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOperatorField> ForceOperatorField;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URadialFalloff> ForceRadialFalloff;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCullingField> ForceCullingField;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URadialFalloff> DynamicRadialFalloff;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UUniformInteger> DynamicUniformInteger;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCullingField> DynamicCullingField;
	

	FTimerHandle FieldDeadHandle;
};
