// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHMinimapBase.generated.h"

enum class ELicenseRank : uint8;

UCLASS()
class FRANKHUNTER_API UFHMinimapBase : public UUserWidget
{
	GENERATED_BODY()
    

protected:
    // change scale function is obsolete
    //float TargetScaleMult{ 0.1f };

    // Hide property now be handled by URoom
    //UPROPERTY(EditAnywhere, Category = "Minimap")
    //uint8 bHideNewlyAddedRoom : 1{ true };
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UTextBlock> TimeLimitText;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UTextBlock> LeftCoreText;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UTextBlock> LeftTaxText;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UTextBlock> LeftBatteryText;
	/*UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
	TObjectPtr<class UTextBlock> CurrentRankText;*/
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UProgressBar> BatteryProgressBar;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class USizeBox> MinimapViewport;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UCanvasPanel> MapContents;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UImage> MinimapRenderTargetImage;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UCanvasPanel> Player;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    TObjectPtr<class UCanvasPanel> OtherPlayers;

    
    //UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    //TObjectPtr<class UButton> ZoomOutBtn;
    //UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Minimap")
    //TObjectPtr<class UButton> ZoomInBtn;


protected:
    int32 DungeonLowestFloor{ 0 };
    int32 DungeonHighestFloor{ 0 };
    int32 DungeonFloors{ 0 };

    int32 CurrentFloor{ -9999 };

public:
    float ScaleMult{ 0.1f };
    //virtual void NativeConstruct() override;

    //void AddRoom(const URoom* Room, const class UFHRoomData* RoomData, int32 MinFloor, int32 MaxFloor);

    //void SetRoomTransparent(int64 RoomID, bool bIsTransparent);
    
    void ConstructFloors(int32 DungeonLowestFloor, int32 DungeonHighestFloor);
    TObjectPtr<class UCanvasPanel> GetFloorCanvas(int32 AbsoluteFloor);

    void UpdateMinimapPosition(FVector PlayerPosition, FRotator PlayerRotation, int32 CurrentAbsoluteFloor);

    class UImage* AttachMinimapObjectImage(UTexture2D* Texture, FVector2D ImageSize, FVector WorldLocation, int32 AbsoluteFloor);
    class UImage* AttachOtherPlayerImage(UTexture2D* Texture, FVector2D ImageSize);

    void SetTimeLimitText(FText text, FString LeftTime);
    void SetLeftCoreText(FText text, int32 LeftCore);
    void SetLeftBatteryText(float MaxAmount, float CurrentAmount);
    void SetLeftTaxText(float CurrentPay, float TotalPay);
    //void SetCurrentRankText(ELicenseRank Rank);

    //void UpdateMinimapRenderTargetImage(UTexture2D* MinimapRenderTarget);

    //UFUNCTION()
    //void ZoomOut();
    //UFUNCTION()
    //void ZoomIn();

protected:
    void UpdateFloor(int32 NewFloor);
    //void UpdateFloor(int32 Floor);

    //TObjectPtr<class UCanvasPanel> GetFloorCanvas(int32 floor);

    UPROPERTY()
    TArray<TObjectPtr<class UCanvasPanel>> FloorCanvases;
};
