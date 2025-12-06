// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameSavable.generated.h"

class USaveGame;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameSavable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SIMPLESAVEKIT_API IGameSavable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual FString GetSaveSlot() const;
	virtual bool IsGlobal() const;
	virtual void SerializeData(FArchive& Ar);
};
