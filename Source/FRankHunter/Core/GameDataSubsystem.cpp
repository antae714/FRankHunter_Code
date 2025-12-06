// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/GameDataSubsystem.h"
#include "Misc/DefaultValueHelper.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "Core/GameSaveData.h"
#include "FRankHunter.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SimpleSaveKitFunctionLibrary.h"
#include "MultiSaveGame.h"


class FTempVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
	TArray<FString> AllGameID;

	virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
	{
		if (bIsDirectory)
		{
			FString TempGameID{};
			FString DirectoryName(FilenameOrDirectory);
			DirectoryName = FPaths::GetCleanFilename(DirectoryName);
			AllGameID.Add(DirectoryName);

		}
		return true; // 계속 탐색
	}
};


UGameDataSubsystem::UGameDataSubsystem()
{
	SetGameName(TEXT("0"));
}

void UGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

FString UGameDataSubsystem::GetGameID() const
{
	return GameID;
}

FString UGameDataSubsystem::GetGameName() const
{
	return GameSaveData.GameName;
}

void UGameDataSubsystem::SetGameName(FString NewGameID)
{
	GameSaveData.GameName = NewGameID;

	FTCHARToUTF8 UTF8Converter(*NewGameID);
	const uint8* Bytes = reinterpret_cast<const uint8*>(UTF8Converter.Get());
	int32 Size = UTF8Converter.Length();

	GameID = BytesToHex(Bytes, Size);
}

TArray<FString> UGameDataSubsystem::LoadGameIDs()
{
	// SaveGames 디렉토리 경로
	const FString SaveDir = FPaths::ProjectSavedDir() / TEXT("SaveGames");

	// 존재 여부 확인
	if (!IFileManager::Get().DirectoryExists(*SaveDir))
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGames directory does not exist: %s"), *SaveDir);
		return {};
	}

	FTempVisitor TempVisitor;
	IFileManager::Get().IterateDirectory(*SaveDir, TempVisitor);
	

	return TempVisitor.AllGameID;

}

TArray<FString> UGameDataSubsystem::LoadGameNames()
{
	// SaveGames 디렉토리 경로
	const FString SaveDir = FPaths::ProjectSavedDir() / TEXT("SaveGames");

	// 존재 여부 확인
	if (!IFileManager::Get().DirectoryExists(*SaveDir))
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGames directory does not exist: %s"), *SaveDir);
		return {};
	}

	FTempVisitor TempVisitor;
	IFileManager::Get().IterateDirectory(*SaveDir, TempVisitor);
	TArray<FString> ResultArray;
	for (auto& item : TempVisitor.AllGameID)
	{
		FGameSaveData Result;
		UGameSaveDataForLoad* TempGameSaveData = NewObject<UGameSaveDataForLoad>();
		TempGameSaveData->GameSaveData = &Result;

		FString SlotName = item / TEXT("GameState");
		USimpleSaveKitFunctionLibrary::LoadGameFromObjectToSlot(SlotName, TempGameSaveData, false);
		ResultArray.Add(Result.GameName);
	}



	return ResultArray;
}

FString UGameDataSubsystem::GetOwnerNickName() const
{
	return GameSaveData.OwnerNickName;
}

FString UGameDataSubsystem::GetPassword() const
{
	return GameSaveData.Password;
}

bool UGameDataSubsystem::IsLAN() const
{
	return GameSaveData.bIsLAN;
}

bool UGameDataSubsystem::IsPublic() const
{
	return GameSaveData.bIsPublic;
}

bool UGameDataSubsystem::IsFriendOnly() const
{
	return GameSaveData.bIsFriendOnly;
}

bool UGameDataSubsystem::IsMorning() const
{
	return GameSaveData.bIsMorning;
}

void UGameDataSubsystem::SetOwnerNickName(const FString& NewOwnerNickName)
{
	GameSaveData.OwnerNickName = NewOwnerNickName;
}

void UGameDataSubsystem::SetPassword(const FString& NewPassword)
{
	GameSaveData.Password = NewPassword;
}

void UGameDataSubsystem::SetIsLAN(bool bNewIsLAN)
{
	GameSaveData.bIsLAN = bNewIsLAN;
}

void UGameDataSubsystem::SetIsPublic(bool bNewIsPublic)
{
	GameSaveData.bIsPublic = bNewIsPublic;
}

void UGameDataSubsystem::SetIsFriendOnly(bool bNewIsFriendOnly)
{
	GameSaveData.bIsFriendOnly = bNewIsFriendOnly;
}

void UGameDataSubsystem::SetIsMorning(bool bNewIsMorning)
{
	GameSaveData.bIsMorning = bNewIsMorning;
}



void UGameDataSubsystem::InitializeGameSaveData(FString SaveGameID)
{
	if (!SaveGameID.IsEmpty())
	{
		FTCHARToUTF8 UTF8Converter(*SaveGameID);
		const uint8* Bytes = reinterpret_cast<const uint8*>(UTF8Converter.Get());
		int32 Size = UTF8Converter.Length();

		GameID = BytesToHex(Bytes, Size);
	}

	GameSaveData = LoadGameSaveData(SaveGameID);
}

FGameSaveData UGameDataSubsystem::LoadGameSaveData(FString SaveGameID)
{
	if (SaveGameID.IsEmpty())
	{
		ensure(0);
		return {};
	}
	FGameSaveData Result;
	UGameSaveDataForLoad* TempGameSaveData = NewObject<UGameSaveDataForLoad>();
	TempGameSaveData->GameSaveData = &Result;

	if (USimpleSaveKitFunctionLibrary::IsExistSaveFile(SaveGameID, TempGameSaveData))
	{
		USimpleSaveKitFunctionLibrary::LoadGameFromObject(SaveGameID, TempGameSaveData, false);
	}

	return Result;
}

void UGameSaveDataForLoad::SerializeData(FArchive& Ar)
{
	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);
	if (Ar.CustomVer(FFRankHunterCustomVersion::GUID) >= FFRankHunterCustomVersion::FirstVersion)
	{
		GameSaveData->Serialize(Ar);
	}
}
