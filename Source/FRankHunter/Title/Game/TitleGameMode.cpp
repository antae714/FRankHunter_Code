// Copyright F Rank Hunter.. All Rights Reserved.


#include "Title/Game/TitleGameMode.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Title/Controller/TitleController.h"
#include "Core/FHGameInstance.h"



void ATitleGameMode::BeginPlay()
{
	Super::BeginPlay();

	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
		if (Session)
		{
			SessionInterface->DestroySession(NAME_GameSession);
		}
	}
}

FString ATitleGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	UFHGameInstance* GI = GetGameInstance<UFHGameInstance>();

	if (GI && GI->HasNetworkError())
	{
		ATitleController* TitleController = Cast<ATitleController>(NewPlayerController);
		TitleController->PopUpMessageBoxByKey(FName(*GI->GetErrorMsg()));
		GI->ErrorNotified();
		GI->bIsReturningToTitle = false;
	}

	//if (GI && GI->bIsReturningToTitle)
	//{
	//	ATitleController* TitleController = Cast<ATitleController>(NewPlayerController);
	//	TitleController->PopUpMessageBoxByKey(FName(*GI->GetErrorMsg()));
	//	
	//}
	return Result;
}
