// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/Lobby/LobbyUIUtils.h"
#include "FHTray.generated.h"

UCLASS()
class FRANKHUNTER_API AFHTray : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHTray();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<class UBoxComponent> BoxCollider;

	UPROPERTY()
	TObjectPtr<class UBoxComponent> ItemValueCheckBox;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class AFHSellHelper> ConnectedButton;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnItemValueCheckBoxOverlapBegin(AActor* DroppedItem);

	UFUNCTION(BlueprintCallable)
	void OnItemValueCheckBoxOverlapEnd(AActor* DroppedItem);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTotalItemValueChanged();

	UFUNCTION(BlueprintCallable)
	void SellItem();

	UFUNCTION(BlueprintImplementableEvent)
	void ProcessSellItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SellItemActor")
	TSubclassOf<AActor> PickupItemActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	TObjectPtr<AActor> Door;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Working")
	bool bIsWorking{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TotalItemValue", ReplicatedUsing="OnRep_TotalItemValue")
	int32 TotalItemValue = 0;

	UFUNCTION()
	void OnRep_TotalItemValue();
};
