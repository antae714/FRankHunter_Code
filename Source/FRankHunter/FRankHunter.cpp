// Copyright F Rank Hunter. All Rights Reserved.

#include "FRankHunter.h"
#include "Modules/ModuleManager.h"
#include "DungeonGeneration/FHSpawningPointDescriptor.h"

const FGuid FFRankHunterCustomVersion::GUID(0xD764C013, 0x31C9402A, 0x8AB324B8, 0x9AF1A433);
FCustomVersionRegistration GRegisterFRankHunterCustomVersion(FFRankHunterCustomVersion::GUID, FFRankHunterCustomVersion::LatestVersion, TEXT("FRankHunter Version"));


DEFINE_LOG_CATEGORY(FH);
DEFINE_LOG_CATEGORY(NetDebug);



void FRankHunterModule::StartupModule()
{

}

void FRankHunterModule::ShutdownModule()
{

}


IMPLEMENT_PRIMARY_GAME_MODULE(FRankHunterModule, FRankHunter, "FRankHunter");
//IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, FRankHunter, "FRankHunter");