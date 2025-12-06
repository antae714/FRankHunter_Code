// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGeneration/FHDungeonGenerator.h"
#include "FHTerminalBase.generated.h"

UENUM(BlueprintType)
enum class EMinimapObjectType : uint8
{
    None 		        UMETA(DisplayName = "None"),
    Player 		        UMETA(DisplayName = "Player"),
	OtherPlayer			UMETA(DisplayName = "OtherPlayer"),
	RoomIcon			UMETA(DisplayName = "Stair"),
	DungeonCore			UMETA(DisplayName = "DungeonCore")
};

USTRUCT()
struct FRANKHUNTER_API FFHMinimapTrackingIconDescriptor
{
    GENERATED_BODY()
	
	UPROPERTY()
	uint8 bEnabled : 1 { true };
    UPROPERTY()
    EMinimapObjectType Type{ EMinimapObjectType::None };
    UPROPERTY()
    TWeakObjectPtr<AActor> TrackingActor;
	UPROPERTY()
	TObjectPtr<class UImage> ImageWidget;
    UPROPERTY()
    uint8 bInheritRotation : 1{ true };
};

USTRUCT()
struct FRANKHUNTER_API FFHMinimapRoomDescriptor
{
    GENERATED_BODY()

	UPROPERTY()
    const class URoom* Room{ nullptr };

	UPROPERTY()
	const class UFHRoomData* RoomData{ nullptr };

    int32 MinFloor{ 0 };
    int32 MaxFloor{ 0 };

	FVector Location{};
	FVector Extension{};
    float YawAngle{ 0.0f };
};


USTRUCT()
struct FFHMinimapObjectBindingStructure
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<class UImage>> Images;
};


UCLASS()
class FRANKHUNTER_API AFHTerminalBase : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TWeakObjectPtr<class AFHPlayerBase> OwnerPlayer;
	


	// ========= 배터리 ==========
	UPROPERTY(EditAnywhere, Category = "TerminalBase")
	float InitialBattery{ 100.0f };
protected:
	UPROPERTY(VisibleAnywhere, Category = "TerminalBase")
	float CurrentBattery{ 100.0f };
public:
	UPROPERTY(EditAnywhere, Category = "TerminalBase")
	float BatteryReduceIntervalSecond{ 3.0f };
	UPROPERTY(EditAnywhere, Category = "TerminalBase")
	float BatteryReduceAmountForEachCycle{ 1.0f };

	float NextBatteryReduceIntervalSecond{ 3.0f };
protected:
	float ElapsedTimeFromLastBatteryUpdate{ 0.0f };


public:
	// 전체 디스플레이
	UPROPERTY(EditAnywhere, Category = "Minimap")
	TObjectPtr<UMaterialInterface> GateTerminalMaterial;
	UPROPERTY(EditAnywhere, Category = "Minimap")
	TObjectPtr<UMaterialInterface> LobbyTerminalMaterial;

	UPROPERTY(EditAnywhere, Category = "Minimap")
	FName DisplayMaterialRenderTargetTextureParameterName{ TEXT("TerminalDisplay") };

	// 디스플레이 내 미니맵이 그려질 이미지 텍스쳐
	UPROPERTY(EditAnywhere, Category = "Minimap")
	TObjectPtr<UTextureRenderTarget2D> MinimapRenderTarget;
	UPROPERTY(EditAnywhere, Category = "Minimap")
	TObjectPtr<UMaterialInterface> MinimapOutlineMaterial;
	//UPROPERTY(EditAnywhere, Category = "Minimap")
	//TObjectPtr<UTextureRenderTarget2D> MinimapRenderTargetFinal;
	
    UPROPERTY(EditAnywhere, Category = "Minimap")
	FVector2D RoomIconSize{ 100.0, 100.0 };

    UPROPERTY(EditAnywhere, Category = "Minimap")
    float MinimapUpdateInterval{ 0.1f };

    UPROPERTY(EditAnywhere, Category = "Minimap")
    FLinearColor MinimapTextureClearColor{ 0, 0, 0, 1 };

    UPROPERTY(EditAnywhere, Category = "Minimap")
    float MinimapScaleVersusRealWorld{ 0.1f };

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> BodyMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> DisplayMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UWidgetComponent> DisplayWidget;

	// Voive Recv
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Voice")
	//TObjectPtr<class UVOIPTalker> VOIPComponent;
	float LastCachedVolume;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicDisplayMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ShutOffMaterial;



	bool bIsWidgetAttached{ false };

	UPROPERTY(EditAnywhere)
	uint8 bCountTimeLimit : 1{ true };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> LobbyWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UUserWidget> LobbyWidgetInstance;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	TObjectPtr<UTexture2D> MinimapDoorTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	TObjectPtr<UTexture2D> OtherPlayerIconTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	FVector2D OtherPlayerIconSize{ 50.0, 50.0 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	TSubclassOf<class UFHMinimapBase> MinimapWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<class UFHMinimapBase> MinimapWidgetInstance;

	uint32 bIsInGate : 1{ false };
	class AFHGateGameStateBase* GateState{ nullptr };
	float ElapsedTimeForLastMinimapUpdate{ 0.0f };


	// ========= Minimap ingredients ==========
protected:
	UPROPERTY()
    TArray<FFHMinimapTrackingIconDescriptor> TrackingIcons;

    int32 CurrentFloor = 0;
    double epsilon = 10.0;

    TMap<int64, FFHMinimapRoomDescriptor> RoomDescs;
    double GeneratorPosZ = 0;
    double FloorHeight = 0;

	// Room - MinimapObjects binding
	UPROPERTY()
	TSet<int64> DiscloseTriggeredRooms;
	UPROPERTY()
	TMap<int64, FFHMinimapObjectBindingStructure> RoomMinimapObjectMap;



public:
	AFHTerminalBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	FTimerHandle InitializationTimer;
	void InitializeTerminal();

	virtual void Tick(float DeltaTime) override;

	void StartTalk();
	void StopTalk();
	void VoiceChannelOn();
	void VoiceChannelOff();
	void OnPlayerTalking(TSharedRef<const FUniqueNetId> UniqueNetIdRef, bool IsTalking);
	void SetVOIPTalker(APlayerState* PlayerState);
	void OnOffMute();
	UFUNCTION(BlueprintCallable)
	class UVOIPTalker* GetVOIPTalker(APlayerState* PlayerState);

	uint32 bUpdateMinimap : 1{ true };
	uint32 bUpdateBattery : 1{ false };

	UPROPERTY()
	uint32 bIsRemoteTalking : 1 {false};

	UPROPERTY()
	uint32 bIsLocalTalking : 1 {false};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VOIPTalker")
	TArray<TObjectPtr<class UVOIPTalker>> RemotePlayerTalkers;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Voice")
	TArray<class UVOIPTalker*> Remotes;
protected:


	UPROPERTY()
	uint32 bIsMute : 1 {false};

	FVector RoomUnit{ 512, 512, 600 };

public:
	void ConstructMinimapWidget(FVector RoomUnit, const TArray<FMinimapRoomSpec>& RoomSpecs, int32 DungeonLowestFloor, int32 DungeonHighestFloor, float RoomHeight, float GeneratorPosZ);
	void UpdateMinimapWidget();
	UFUNCTION()
	void UpdateLeftTimeText(int32 LeftTime);
	UFUNCTION()
	void UpdateLeftCoreText(int32 LeftCore);
protected:
	void AttachOtherPlayerIcon();
	void UpdateTrackingIcons(FVector& playerPos);
	// 이미 진입한 방의 테두리는 그대로 그리기 위해 진입한 방들은 R 값에 작은 오차를 주어 그리고, Distance가 0 이상일 경우 테두리를 그리게 함.
	// R 채널은 룸 / G 채널은 문으로 사용
	void DrawRoom(class UCanvas* canvas, UTexture* texture, FVector2D& canvasSize, struct FFHMinimapRoomDescriptor& desc, FVector& playerPos, float CurrentR);

	// 문 캐싱
	UPROPERTY(EditAnywhere, Category = "Minimap")
	float DoorTextureWidthPerRoomUnit = 0.8f;
	UPROPERTY(EditAnywhere, Category = "Minimap")
	float DoorTextureHeightPerRoomUnit = 0.4f;
	UPROPERTY()
	TSet<class ADoor*> Doors;
	void DrawDoors(float RoomUnitX, class UCanvas* canvas, FVector2D& canvasSize, FVector& playerPos);


	void OnDisclose(int64 RoomID);
public:
	//virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void ShutOffTerminal();

protected:
	UFUNCTION(Server, Reliable)
	void Server_TurnOffTerminalDisplay();
	void Server_TurnOffTerminalDisplay_Implementation();
	UFUNCTION(Server, Reliable)
	void Multicast_TurnOffTerminalDisplay();
	void Multicast_TurnOffTerminalDisplay_Implementation();


// utils
protected:
	// only use after minimap constructed
	int32 GetPlayerStandsFloor(class AFHPlayerBase* player);




	// 배터리
protected:
	void UpdateBattery();
};
