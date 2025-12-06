// Copyright F Rank Hunter. All Rights Reserved.


#include "Creature/Sense/AISenseConfig_SeenBy.h"
#include "Creature/Sense/AISense_SeenBy.h"


UAISenseConfig_SeenBy::UAISenseConfig_SeenBy()
{
    Implementation = UAISense_SeenBy::StaticClass();
    SightRadius = 21.f;
    SightRange = 2000.0f;
    SightStartOffset = FVector(0.f, 0.f, 0.f);
}

TSubclassOf<UAISense> UAISenseConfig_SeenBy::GetSenseImplementation() const
{
    return Implementation;
}