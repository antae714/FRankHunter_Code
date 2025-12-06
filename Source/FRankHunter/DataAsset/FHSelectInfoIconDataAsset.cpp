// Copyright F Rank Hunter. All Rights Reserved.


#include "DataAsset/FHSelectInfoIconDataAsset.h"

TSoftObjectPtr<UTexture2D> UFHSelectInfoIconDataAsset::GetIconTexture(FName Key)
{
	return TSoftObjectPtr<UTexture2D>(IconMap.FindRef(Key).ToSoftObjectPath());
}

TSoftObjectPtr<UTexture2D> UFHSelectInfoIconDataAsset::GetSkillIconTexture(FName Key)
{
	return TSoftObjectPtr<UTexture2D>(SkillIconMap.FindRef(Key).ToSoftObjectPath());
}

TSubclassOf<AActor> UFHSelectInfoIconDataAsset::GetCharacterBP(FName Key)
{
	return CharacterBPMap.FindRef(Key);
}

TSoftObjectPtr<UPhysicsAsset> UFHSelectInfoIconDataAsset::GetNormalCharacterPhysicsAsset(FName Key)
{
	return NormalCharacterPhysicsAsset;
}

TSoftObjectPtr<UPhysicsAsset> UFHSelectInfoIconDataAsset::GetRagdollCharacterPhysicsAsset(FName Key)
{
	return RagdollCharacterPhysicsAsset;
}
