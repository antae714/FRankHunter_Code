// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ManagerActorRegistrySubsystem.generated.h"

class AManagerActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnManagerActorRegisted);

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UManagerActorRegistrySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	bool RegisterManagerActor(AManagerActor* ManagerActor);
	bool UnRegisterManagerActor(AManagerActor* ManagerActor);

	bool RegisterManager(UObject* ManagerActor, FName ManagerName);
	bool UnRegisterManager(FName ManagerName);

	UFUNCTION(BlueprintCallable)
	UObject* GetManagerActor(FName ManagerName) const
	{
		return Managers.FindRef(ManagerName);
	}

	template<typename T>
	T* GetManagerActor(FName ManagerName) const
	{
		return Cast<T>(GetManagerActor(ManagerName));
	}

	FOnManagerActorRegisted& GetManagerRegistDelegate(FName ManagerName);

private:
	UPROPERTY()
	TMap<FName, UObject*> Managers;

	UPROPERTY()
	TMap<FName, FOnManagerActorRegisted> ManagerRegistDelegates;
};
