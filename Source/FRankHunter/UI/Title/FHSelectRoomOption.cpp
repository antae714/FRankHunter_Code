// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Title/FHSelectRoomOption.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Components/TextBlock.h"
#include "FRankHunter.h"
#include "Core/GameDataSubsystem.h"

void UFHSelectRoomOption::NativeConstruct()
{
	Super::NativeConstruct();

	IOnlineSessionPtr SessionPtr = Online::GetSessionInterface(GetWorld(), TEXT("Null"));
	if (SessionPtr.IsValid())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UFHSelectRoomOption::FindSessionCompleted);
	}
}

void UFHSelectRoomOption::FindSessionCompleted(bool Result)
{
	if (!Result)
	{
		PRINT_LOG(TEXT("[SelectRoomOption Widget | Find Session Completed.] Unknown Error."));
		return;
	}

	if (SearchOption->SearchResults.IsEmpty())
	{
		PRINT_LOG(TEXT("[SelectRoomOption Widget | Find Session Completed.] No Session Found."));
		RoomName->SetText(FText::FromString(TEXT("None")));
		RoomMasterName->SetText(FText::FromString(TEXT("None")));

		ErrorSessionNotFound();

		return;
	}
	
	for (auto& Results : SearchOption->SearchResults)
	{
		FString SessionId = Results.GetSessionIdStr();
		IOnlineSessionPtr SessionInterfacePtr = Online::GetSessionInterface(GetWorld(), TEXT("Null"));
		FString SessionName;
		Results.Session.SessionSettings.Get(TEXT("Name"), SessionName);
		FString OwnerName = Results.Session.OwningUserName;
		
		RoomName->SetText(FText::FromString(SessionName));
		RoomMasterName->SetText(FText::FromString(OwnerName));
		return;
	}
}

void UFHSelectRoomOption::OnRefreshButtonClicked()
{
	IOnlineSessionPtr SessionPtr = Online::GetSessionInterface(GetWorld(), TEXT("Null"));
	if (SessionPtr.IsValid())
	{
		SearchOption->bIsLanQuery = true;

		SessionPtr->FindSessions(0, SearchOption);
	}
}

void UFHSelectRoomOption::OnJoinButtonClicked()
{
	if (SearchOption->SearchResults.IsEmpty())
	{
		ErrorSessionNotFound();
		return;
	}

	for (auto& Results : SearchOption->SearchResults)
	{
		FString SessionId = Results.GetSessionIdStr();
		IOnlineSessionPtr SessionInterfacePtr = Online::GetSessionInterface(GetWorld(), TEXT("Null"));
		FString SessionName;
		Results.Session.SessionSettings.Get(TEXT("Name"), SessionName);
		SessionInterfacePtr->JoinSession(0, *SessionName, Results);
		return;
	}
}

void UFHSelectRoomOption::OnCreateButtonClicked()
{
	//UGameInstance* GameInstance = GetGameInstance();
	//UGameDataSubsystem* GameDataSubsystem = GameInstance->GetSubsystem<UGameDataSubsystem>();
	//if (GameDataSubsystem)
	//{
	//	int32 RandomID = FMath::RandRange(1, 1000000000);
	//	GameDataSubsystem->GameID = RandomID;
	//	PRINT_LOG(TEXT("GameID Setted. ID: %d"), RandomID);
	//}
}
