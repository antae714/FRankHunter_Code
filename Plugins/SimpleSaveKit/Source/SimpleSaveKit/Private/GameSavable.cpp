// Copyright F Rank Hunter. All Rights Reserved.


#include "GameSavable.h"

// Add default functionality here for any IGameSavable functions that are not pure virtual.

FString IGameSavable::GetSaveSlot() const
{
    return TEXT("");
}

bool IGameSavable::IsGlobal() const
{
    return false;
}

void IGameSavable::SerializeData(FArchive& Ar)
{

}
