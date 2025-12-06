// Copyright I Love Unity, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SiInventorySystemInterface.generated.h"

class ASiInventory;
class USiInventoryComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class USiInventorySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SIMPLEINVENTORY_API ISiInventorySystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual USiInventoryComponent* GetInventoryComponent() const;
};
