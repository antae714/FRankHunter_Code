// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Props/Traps/FHTrapBase.h"
#include "FHChalco.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHChalco : public AFHTrapBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ACharacter> LockedCharacter;

	UPROPERTY(BlueprintReadOnly)
	uint8 bIsLocked : 1{ false };


public:
	AFHChalco();

protected:
	virtual void BeginPlay() override;
	virtual FName GetTrapName() override { return TEXT("Chalco"); }
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	/*
		Server Only Function.
	*/
	UFUNCTION(BlueprintCallable)
	void LockCharacter(class ACharacter* Target);
	
	UFUNCTION(BlueprintCallable)
	void ReleaseCharacter();
};
