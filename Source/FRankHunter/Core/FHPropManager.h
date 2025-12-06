// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ManagerActor.h"
#include "FHPropManager.generated.h"

UCLASS()
class FRANKHUNTER_API AFHPropManager : public AManagerActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHPropManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual FName GetManagerName() const { return TEXT("PropManager"); }

	void CacheProps();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
		Can Regist Blueprint Actor that not inherit cpp class.
	*/
	UFUNCTION(BlueprintCallable)
	void RegistProp(FName PropName, AActor* Actor);

	/*
		Can UnRegist Blueprint Actor that not inherit cpp class.
	*/
	UFUNCTION(BlueprintCallable)
	void UnRegistProp(FName PropName);

	UFUNCTION(BlueprintCallable)
	AActor* BP_GetProp(const FName& PropKey);

	template <class T>
	T* GetProp(const FName& PropKey)
	{
		if (Props.IsEmpty())
		{
			return nullptr;
		}

		if (AActor** TargetActor = Props.Find(PropKey))
		{
			return Cast<T>(*TargetActor);
		}
		return nullptr;
	}

	template<>
	AActor* GetProp(const FName& PropKey)
	{
		if (Props.IsEmpty())
		{
			return nullptr;
		}

		if (AActor** TargetActor = Props.Find(PropKey))
		{
			return *TargetActor;
		}
		return nullptr;
	}

protected:
	// ----------------------------------
	// ---------- LevelProps ------------
	// ----------------------------------


	UPROPERTY()
	TMap<FName, AActor*> Props;
};
