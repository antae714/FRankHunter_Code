// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Field/FieldSystemActor.h"
#include "FHAnchorFieldSystemActor.generated.h"

class UBoxComponent;
class UUniformInteger;
class UCullingField;
class UBoxFalloff;
class UFieldSystemMetaDataFilter;


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHAnchorFieldSystemActor : public AFieldSystemActor
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Enable();

	UFUNCTION(BlueprintCallable)
	void Disable();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> AnchorBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bIsIntialEnabled : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EFieldFilterType> FieldFilterType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EFieldObjectType> FieldObjectType;

public:
	AFHAnchorFieldSystemActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UUniformInteger> UniformInteger;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxFalloff> BoxFalloff;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCullingField> CullingField;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFieldSystemMetaDataFilter> Filter;
	
};
