// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "FHCoreObject.generated.h"


class UInputAction;

UCLASS()
class FRANKHUNTER_API AFHCoreObject : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    uint32 bIsFloating : 1{ true };
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    float FloatRange = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    float FloatSpeed = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
	TSubclassOf<class UCameraShakeBase> CameraShakeBaseOnDestroy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    TSubclassOf<class AActor> InnerCoreActor;





	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//TObjectPtr<class UFHInteractableComponent> InteractableComponent;
    
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<class USphereComponent> SphereCollision;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<class UGeometryCollectionComponent> CoreGeometry;
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<class UChildActorComponent> CoreDestructionField;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UFHAttributeSet_Health> HealthAttributeSet;


private:
    FVector initialLocation;
    float elapsedTime;
	
public:
	AFHCoreObject();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	void UpdateFloating(float DeltaTime);

    //UFUNCTION()
    //void OnInteract_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction);

    UFUNCTION(BlueprintCallable)
    void ShakeCamera();
};
