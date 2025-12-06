// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHPreviewActorBase.generated.h"

UCLASS()
class FRANKHUNTER_API AFHPreviewActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHPreviewActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PawnKey")
	FName PawnMeshKey;

	UFUNCTION(BlueprintCallable, Category="SelectPreview")
	FName GetPawnMeshKey() const { return PawnMeshKey; }


	UFUNCTION(NetMulticast, Reliable)
	void SetDisableActor();
	void SetDisableActor_Implementation();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DisableActor")
	void DisableActor();

};
