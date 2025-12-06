// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHWorldItemActor.generated.h"

class UStaticMesh;
class UTexture2D;
class UProjectileMovementComponent;
class UAbilitySystemComponent;
class UFHInventoryComponent;
class USiInventoryComponent;

UCLASS(Blueprintable)
class FRANKHUNTER_API AFHWorldItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHWorldItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	UFHInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UFUNCTION()
	virtual void OnInventoryChangedFunction(USiInventoryComponent* Inventory, int32 Index);
	


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFHInventoryComponent> InventoryComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMesh> TestMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstance> TestMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true"))
	uint32 bIsSetSimulatePhysics : 1;

	void SetItemMesh(TSoftObjectPtr<UStaticMesh> ItemMeshSoftPtr);
	void SetTempTexture(TSoftObjectPtr<UTexture2D> ItemIconSoftPtr);
	void SetTempMaterial(TSoftObjectPtr<UMaterialInstance> ItemIconSoftPtr);
};
