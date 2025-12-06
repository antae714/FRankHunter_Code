// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/Interface.h"
#include "FHItemDropPointComponent.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDropPointInterface : public UInterface
{
	GENERATED_BODY()
};

class FRANKHUNTER_API IDropPointInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector PopNextPoint();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector PeekPoint();

	virtual FVector PopNextPoint_Implementation() { return FVector::ZeroVector; }
	virtual FVector PeekPoint_Implementation() { return FVector::ZeroVector; }
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UFHItemDropPointSocketComponent : public UActorComponent, public IDropPointInterface
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

public:
	virtual FVector PopNextPoint_Implementation() override;
	virtual FVector PeekPoint_Implementation() override;


	void SetHandleComponent(TObjectPtr<USceneComponent> SceneComponent)
	{
		HandleComponent = SceneComponent;
	}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> HandleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FName> PointNameArray;
	int32 CurrentIndex;
	TArray<FName> GetPointNames();
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UFHItemDropPointMarkerComponent : public USceneComponent, public IDropPointInterface
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

public:
	virtual FVector PopNextPoint_Implementation() override;
	virtual FVector PeekPoint_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<USceneComponent>> ChildComponents;

	int32 CurrentIndex;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UFHItemDropPoinManualComponent : public UActorComponent , public IDropPointInterface
{
	GENERATED_BODY()

public:
	virtual FVector PopNextPoint_Implementation() override;
	virtual FVector PeekPoint_Implementation() override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FVector> ManualPoints;

protected:
	int32 CurrentIndex;
};



