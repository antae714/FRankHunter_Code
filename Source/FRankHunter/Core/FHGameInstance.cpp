// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHGameInstance.h"
#include "UI/Common/FHFadeInOut.h"
#include "UI/Gate/FHLoading.h"
#include "UObject/UObjectGlobals.h"
#include "FRankHunter.h"
#include "Kismet/GameplayStatics.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "DataAsset/FHLevelAsset.h"
#include "Core/GameDataSubsystem.h"

UFHGameInstance::UFHGameInstance()
{
	SetGateRank(EGateRank::Invalid);
}

void UFHGameInstance::SetGateRank(EGateRank Rank)
{
	const UEnum* EnumPtr = StaticEnum<EGateRank>();
	GateRank = EnumPtr->GetNameStringByValue(static_cast<int64>(Rank));
}

EGateRank UFHGameInstance::GetGateRank()
{
	const UEnum* EnumPtr = StaticEnum<EGateRank>();
	return static_cast<EGateRank>(EnumPtr->GetValueByName(FName(*GateRank)));
}

FString UFHGameInstance::GetGateRankString()
{
	return GateRank;
}

void UFHGameInstance::SetLobbyState(bool State)
{
	UGameDataSubsystem* GameData = GetSubsystem<UGameDataSubsystem>();
	check(GameData);

	GameData->SetIsMorning(State);
	bIsMorning = State;
}

void UFHGameInstance::Init()
{
	Super::Init();

	// if assert this, add fadeinoutwidget in bp_gameinstance
	check(FadeInOutWidgetClass);
	// if assert this, add LoadingUI in bp_gameinstance
	check(LoadingWidgetClass);
	FadeInOutInstance = CreateWidget<UFHFadeInOut>(this, FadeInOutWidgetClass);
	FadeInOutInstance->SetVisibility(ESlateVisibility::Collapsed);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UFHGameInstance::ShowFadeWidget);

//#ifndef WITH_EDITOR
//	FCoreUObjectDelegates::PreLoadMap.AddLambda([&](const FString& MapName) {
//		PRINT_LOG(TEXT("Start PreLoadMap. LoadingMap: %s"), *MapName);
//		if (!FadeInOutInstance)
//		{
//			return;
//		}
//		FadeInOutInstance->SetVisibility(ESlateVisibility::Visible);
//		});
//	FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda([&](UWorld* World) {
//		PRINT_LOG(TEXT("PostLoadMap."));
//		if (!FadeInOutInstance)
//		{
//			return;
//		}
//		FadeInOutInstance->SetVisibility(ESlateVisibility::Visible);
//		});
//#endif

	// create only. call Init() when GateGameState is BeginPlay.
	LoadingInstance = CreateWidget<UFHLoading>(this, LoadingWidgetClass);
	LoadingInstance->SetVisibility(ESlateVisibility::Collapsed);




	// 네트워크/트래블 실패 델리게이트 바인딩
	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UFHGameInstance::HandleNetworkFailure);
		GEngine->OnTravelFailure().AddUObject(this, &UFHGameInstance::HandleTravelFailure);
	}
}

void UFHGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver,
										   ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	// 로그 남기고 타이틀로 복귀
	UE_LOG(LogTemp, Warning, TEXT("[NetworkFailure] %d : %s"), (int32)FailureType, *ErrorString);

	ENetMode NetMode = World ? World->GetNetMode() : NM_Standalone;

	// 클라이언트 측에서만 처리
	if (NetMode == NM_ListenServer)
	{
		return;
	}

	if (NetDriver->NetDriverName != TEXT("GameNetDriver"))
	{
		return;
	}


	bHasNetworkError = true;
	if (FailureType == ENetworkFailure::ConnectionLost)
	{
		ErrorMsg = TEXT("HandleNetworkFailure");
	}
	else
	{
		ErrorMsg = ErrorString;
	}

	ReturnToTitle();
}

void UFHGameInstance::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	if (!World || World->GetNetMode() == NM_ListenServer)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[TravelFailure] %d : %s"), (int32)FailureType, *ErrorString);

	ReturnToTitle();
}

void UFHGameInstance::ReturnToTitle()
{
	// (선택) 세션 정리: Steam/EOS/Null 공통
	if (IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr Session = OSS->GetSessionInterface())
		{
			// 보통 "GameSession" 같은 이름 사용
			const FName SessionName = NAME_GameSession;
			if (Session->GetNamedSession(SessionName))
			{
				Session->DestroySession(SessionName);
			}
		}
	}

	UFHLevelAsset* LevelAssetDefault = UFHBlueprintFunctionLibrary::GetLevelAsset();
	check(LevelAssetDefault);

	bIsReturningToTitle = true;
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LevelAssetDefault->GetTitleMap(), true, "closed");
	// 또는 첫 번째 로컬 플레이어컨트롤러 기준 ClientTravel 사용:
	// if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	// {
	//     PC->ClientTravel(TEXT("/Game/Maps/TitleMap"), TRAVEL_Absolute);
	// }
}

void UFHGameInstance::PlayFadeIn(FOnFadeEnd FadeEndDelegate)
{
	if (!FadeInOutInstance->IsInViewport())
	{
		FadeInOutInstance->AddToViewport(200);
	}
	FadeInOutInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
	FadeInOutInstance->OnFadeEndDelegate = FadeEndDelegate;
	FadeInOutInstance->PlayFadeIn();
}

void UFHGameInstance::PlayFadeOut(FOnFadeEnd FadeEndDelegate)
{
	if (!FadeInOutInstance->IsInViewport())
	{
		FadeInOutInstance->AddToViewport(200);
	}
	FadeInOutInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
	FadeInOutInstance->OnFadeEndDelegate = FadeEndDelegate;
	FadeInOutInstance->PlayFadeOut();
}

void UFHGameInstance::InitLoadingWidget()
{
	LoadingInstance->Init();
}

void UFHGameInstance::ShowLoadingWidget()
{
	if (!LoadingInstance->IsInViewport())
	{
		LoadingInstance->AddToViewport(201);
	}
	LoadingInstance->SetVisibility(ESlateVisibility::Visible);
}

void UFHGameInstance::HideLoadingWidget()
{
	LoadingInstance->SetVisibility(ESlateVisibility::Collapsed);
}

void UFHGameInstance::ShowFadeWidget(UWorld* World)
{
	if (!FadeInOutInstance)
	{
		return;
	}
	if (!FadeInOutInstance->IsInViewport())
	{
		FadeInOutInstance->AddToViewport(200);
	}
	FadeInOutInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UFHGameInstance::HideFadeWidget()
{
	//if (!FadeInOutInstance)
	//{
	//	return;
	//}
	//FadeInOutInstance->SetVisibility(ESlateVisibility::Collapsed);
	FOnFadeEnd OnFadeEndDelegate;
	PlayFadeOut(OnFadeEndDelegate);
}
