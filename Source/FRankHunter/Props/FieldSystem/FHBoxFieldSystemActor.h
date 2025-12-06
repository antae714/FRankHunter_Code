// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Field/FieldSystemActor.h"
#include "Field/FieldSystemTypes.h"
#include "FHBoxFieldSystemActor.generated.h"

class UBoxComponent;
class UFieldSystemMetaData;
class UFieldNodeBase; 
class UUniformInteger;
class UCullingField;
class UBoxFalloff;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHBoxFieldSystemActor : public AFieldSystemActor
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Enable();

	UFUNCTION(BlueprintCallable)
	void Disable();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TObjectPtr<UBoxComponent> FallofBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	uint32 bIsIntialEnabled : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	uint32 bIsTick : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TEnumAsByte<EFieldPhysicsType> FieldPhysicsType;

	
public:
	AFHBoxFieldSystemActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	UFieldNodeBase* GetReturnValueField();

	UFUNCTION(BlueprintNativeEvent)
	UFieldSystemMetaData* GetReturnMetaData();

private:
	UPROPERTY()
	TObjectPtr<UBoxFalloff> BoxFalloff;

	UPROPERTY()
	TObjectPtr<UCullingField> CullingField;

	
	UFieldNodeBase* GetAddField();
};
