// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/ManagerActorRegistrySubsystem.h"
#include "Core/ManagerActor.h"

void UManagerActorRegistrySubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
}

bool UManagerActorRegistrySubsystem::RegisterManagerActor(AManagerActor* ManagerActor)
{
	bool IsSucces = false;
	if (ManagerActor)
	{
		FName ManagerName = ManagerActor->GetManagerName();
		IsSucces = RegisterManager(ManagerActor, ManagerName);
	}
	return IsSucces;
}

bool UManagerActorRegistrySubsystem::UnRegisterManagerActor(AManagerActor* ManagerActor)
{
	bool IsSucces = false;
	if (ManagerActor)
	{
		FName ManagerName = ManagerActor->GetManagerName();
		IsSucces = UnRegisterManager(ManagerName);
	}
	return IsSucces;
}

bool UManagerActorRegistrySubsystem::RegisterManager(UObject* ManagerActor, FName ManagerName)
{
	if (!Managers.Contains(ManagerName))
	{
		Managers.Add(ManagerName, ManagerActor);
		if (FOnManagerActorRegisted* DelegatePtr = ManagerRegistDelegates.Find(ManagerName))
		{
			(*DelegatePtr).Broadcast();
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool UManagerActorRegistrySubsystem::UnRegisterManager(FName ManagerName)
{
	if (Managers.Contains(ManagerName))
	{
		Managers.Remove(ManagerName);
		return true;
	}
	else
	{
		return false;
	}
}

FOnManagerActorRegisted& UManagerActorRegistrySubsystem::GetManagerRegistDelegate(FName ManagerName)
{
	return ManagerRegistDelegates.FindOrAdd(ManagerName);
}
