// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/FHRPCComponent.h"
#include "FHSelectCharacterRPC.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHSelectCharacterRPC : public UFHRPCComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void PawnSelected(FName PawnKey);
	void PawnSelected_Implementation(FName PawnKey);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void IconSlotReleased(int32 Index);
	void IconSlotReleased_Implementation(int32 Index);

	UFUNCTION(Client, Reliable)
	void IconStateReplicated(int32 Index, bool State);
	void IconStateReplicated_Implementation(int32 Index, bool State);

	UFUNCTION(Client, Reliable)
	void InitPawnState(const TArray<FName>& StateArray);
	void InitPawnState_Implementation(const TArray<FName>& StateArray);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void CharacterSelectComplete(AActor* MeshPreviewActor, const FName& ActorKey, const FName& SkillID);
	void CharacterSelectComplete_Implementation(AActor* MeshPreviewActor, const FName& ActorKey, const FName& SkillID);

	UFUNCTION(Client, Reliable)
	void SelectError();
	void SelectError_Implementation();
};
