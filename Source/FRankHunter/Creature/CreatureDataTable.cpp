// Copyright F Rank Hunter. All Rights Reserved.


#include "Creature/CreatureDataTable.h"

void FFHCreatuerData::OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems)
{
	OnChangeItem(InRowName);
}

void FFHCreatuerData::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	OnChangeItem(InRowName);
}

void FFHCreatuerData::OnChangeItem(const FName InRowName)
{
	CreatureID = InRowName;
}
