// Copyright F Rank Hunter. All Rights Reserved.

#include "UI/FHMinimapBase.h"

#include "FRankHunter.h"
#include "Room.h"
#include "DataAsset/FHRoomData.h"

#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "Core/FRankHunterTypes.h"


//void UFHMinimapBase::NativeConstruct()
//{
//	Super::NativeConstruct();
//
//	// 이유는 모르겠으나 중복으로 바인딩됨?
//	//ZoomOutBtn->OnClicked.AddDynamic(this, &UFHMinimapBase::ZoomOut);
//	//ZoomInBtn->OnClicked.AddDynamic(this, &UFHMinimapBase::ZoomIn);
//}

//void UFHMinimapBase::AddRoom(const URoom* Room, const UFHRoomData* RoomData, int32 MinFloor, int32 MaxFloor)
//{
//	 obsolete
//	ensureMsgf(MapContents, TEXT("No map contents widget found."));
//	
//	TArray<TObjectPtr<UImage>> ImageWidgets;
//
//	int32 _CurrentFloor = LowestFloor;
//	for (const TObjectPtr<UTexture2D>& Image : Images)
//	{
//		TObjectPtr<UCanvasPanel> CurrentFloorCanvas = GetFloorCanvas(_CurrentFloor);
//
//		TObjectPtr<UImage> ImageWidget = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
//		ImageWidgets.Add(ImageWidget);
//		CurrentFloorCanvas->AddChildToCanvas(ImageWidget);
//
//		TObjectPtr<UCanvasPanelSlot> panelSlot = Cast<UCanvasPanelSlot>(ImageWidget->Slot);
//
//		// 왜인지는 도저히 모르겠는데, 코드로 이미지 위젯을 생성하는 경우 아래와 같이 회전된 상태를 0도로 취급했을 때의 높이, 너비로 설정해야 이미지가 찌그러지지 않고 제대로 표시된다. 도대체 왜?
//		if (YawAngle == 90.0f || YawAngle == 270.0f)
//		{
//			panelSlot->SetSize({ Extent.Y * 2, Extent.X * 2 });
//		}
//		else if (YawAngle == 0.0f || YawAngle == 180.0f)
//		{
//			panelSlot->SetSize({ Extent.X * 2, Extent.Y * 2 });
//		}
//
//		panelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
//		panelSlot->SetAlignment({ 0.5f, 0.5f });
//
//		FSlateBrush Brush;
//		Brush.SetResourceObject(Image);
//		ImageWidget->SetBrush(Brush);
//
//		FWidgetTransform WidgetTransform = ImageWidget->GetRenderTransform();
//		WidgetTransform.Translation.X = Position.X;
//		WidgetTransform.Translation.Y = Position.Y;
//		WidgetTransform.Angle = YawAngle;
//
//		ImageWidget->SetRenderTransform(WidgetTransform);
//
//		_CurrentFloor++;
//	}
//
//	RoomIDMap.Add(RoomID, ImageWidgets);
//	if (bSetGreyOnAttachRoom == true)
//	{
//		SetRoomTransparent(RoomID, true);
//	}
//}

void UFHMinimapBase::ConstructFloors(int32 _DungeonLowestFloor, int32 _DungeonHighestFloor)
{
	DungeonLowestFloor = _DungeonLowestFloor;
	DungeonHighestFloor = _DungeonHighestFloor;
	DungeonFloors = DungeonHighestFloor - DungeonLowestFloor + 1;

	// 캐싱된 크기를 가져와서 시작할 때 위젯이 없으면 0을 반환한다ㅡㅡ
	// FVector2D MinimapViewportSize = MinimapViewport->GetDesiredSize();
	FVector2D MinimapViewportSize = { 400.0f, 400.0f };

	for (int i = 0; i < DungeonFloors; ++i)
	{
		TObjectPtr<class UCanvasPanel> floorCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
		MapContents->AddChildToCanvas(floorCanvas);

		TObjectPtr<UCanvasPanelSlot> panelSlot = Cast<UCanvasPanelSlot>(floorCanvas->Slot);
		panelSlot->SetSize(MinimapViewportSize);
		panelSlot->SetAnchors(FAnchors(0.5f));
		panelSlot->SetAlignment({ 0.5f, 0.5f });
		panelSlot->SetPosition({ 0, 0 });

		floorCanvas->SetVisibility(ESlateVisibility::Hidden);

		FloorCanvases.Add(floorCanvas);
	}
}

TObjectPtr<class UCanvasPanel> UFHMinimapBase::GetFloorCanvas(int32 AbsoluteFloor)
{
	int32 canvasIdx = AbsoluteFloor - DungeonLowestFloor;
	
	if (canvasIdx < 0 || canvasIdx >= DungeonFloors)
	{
		PRINT_LOG(TEXT("There is no floor %d."), AbsoluteFloor);
		return nullptr;
	}

	return FloorCanvases[canvasIdx];
}

//void UFHMinimapBase::SetRoomTransparent(int64 RoomID, bool bIsTransparent)
//{
//	if (RoomIDMap.Contains(RoomID) == false)
//	{
//		PRINT_LOG(TEXT("There is floor %d"), RoomID);
//		return;
//	}
//
//	if (bIsTransparent)
//	{
//		TArray<TObjectPtr<UImage>>& images = RoomIDMap[RoomID];
//		for (TObjectPtr<UImage>& image : images)
//		{
//			image->SetColorAndOpacity(FLinearColor{ 1, 1, 1, 0.33f });
//		}
//	}
//	else
//	{
//		TArray<TObjectPtr<UImage>>& images = RoomIDMap[RoomID];
//		for (TObjectPtr<UImage>& image : images)
//		{
//			image->SetColorAndOpacity(FLinearColor{ 1, 1, 1, 1 });
//		}
//	}
//}

void UFHMinimapBase::UpdateMinimapPosition(FVector PlayerPosition, FRotator PlayerRotation, int32 CurrentAbsoluteFloor)
{
	// Update position
	FWidgetTransform MapContentsTransform = MapContents->GetRenderTransform();
	MapContentsTransform.Translation.X = -PlayerPosition.X * ScaleMult;
	MapContentsTransform.Translation.Y = -PlayerPosition.Y * ScaleMult;
	//MapContentsTransform.Scale.X = ScaleMult;
	//MapContentsTransform.Scale.Y = ScaleMult;
	MapContents->SetRenderTransform(MapContentsTransform);

	if (Player == nullptr)
	{
		return;
	}

	FWidgetTransform PlayerTransform = Player->GetRenderTransform();
	PlayerTransform.Angle = PlayerRotation.Yaw + 90.0f;
	Player->SetRenderTransform(PlayerTransform);

	if (CurrentFloor != CurrentAbsoluteFloor)
	{
		UpdateFloor(CurrentAbsoluteFloor);
	}

	// Update floor
	/*double PlayerFootZ = PlayerPosition.Z + epsilon;
	int32 _curFloor = static_cast<int32>(PlayerFootZ / FloorHeight);
	if (PlayerFootZ < 0.0f) _curFloor -= 1;
	if (_curFloor != CurrentFloor)
	{
		UpdateFloor(_curFloor);
	}*/

	// Scale Animation
	/*if (ScaleMult != TargetScaleMult)
	{
		ScaleMult = FMath::FInterpTo(ScaleMult, TargetScaleMult, GetWorld()->GetDeltaSeconds(), 10.0f);
	}*/
}

class UImage* UFHMinimapBase::AttachMinimapObjectImage(UTexture2D* Texture, FVector2D ImageSize, FVector WorldLocation, int32 AbsoluteFloor)
{
	ensureMsgf(MapContents, TEXT("No map contents widget found."));
	
	if (Texture == nullptr) return nullptr;

	TObjectPtr<UImage> image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	TObjectPtr<UCanvasPanel> floorCanvas = GetFloorCanvas(AbsoluteFloor);
	if (floorCanvas == nullptr) return nullptr;

	floorCanvas->AddChildToCanvas(image);

	TObjectPtr<UCanvasPanelSlot> panelSlot = Cast<UCanvasPanelSlot>(image->Slot);
	panelSlot->SetAnchors(FAnchors(0.5f));
	panelSlot->SetAlignment({ 0.5f, 0.5f });
	panelSlot->SetPosition({ WorldLocation.X * ScaleMult, WorldLocation.Y * ScaleMult });
	panelSlot->SetSize(ImageSize);

	FSlateBrush brush;
	brush.SetResourceObject(Texture);
	image->SetBrush(brush);

	// RenderTransform은 일시적인 애니메이션 등에 쓰는 녀석이라 계산이 중첩되어서 이상하게 보인다고 한다??
	// 이거 쓰나 SetPosition 쓰나 똑같은데?
	//FWidgetTransform WidgetTransform = image->GetRenderTransform();
	//WidgetTransform.Translation.X = WorldLocation.X;
	//WidgetTransform.Translation.Y = WorldLocation.Y;

	//image->SetRenderTransform(WidgetTransform);

	return image.Get();
}

class UImage* UFHMinimapBase::AttachOtherPlayerImage(UTexture2D* Texture, FVector2D ImageSize)
{
	ensureMsgf(OtherPlayers, TEXT("No OtherPlayers canvas panel widget found."));

	if (Texture == nullptr) return nullptr;

	TObjectPtr<UImage> image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	OtherPlayers->AddChildToCanvas(image);

	TObjectPtr<UCanvasPanelSlot> panelSlot = Cast<UCanvasPanelSlot>(image->Slot);
	panelSlot->SetAnchors(FAnchors(0.5f));
	panelSlot->SetAlignment({ 0.5f, 0.5f });
	panelSlot->SetSize(ImageSize);

	FSlateBrush brush;
	brush.SetResourceObject(Texture);
	image->SetBrush(brush);


	//panelSlot->SetPosition({ WorldLocation.X * ScaleMult, WorldLocation.Y * ScaleMult });
	
	// RenderTransform은 일시적인 애니메이션 등에 쓰는 녀석이라 계산이 중첩되어서 이상하게 보인다고 한다??
	// 이거 쓰나 SetPosition 쓰나 똑같은데?
	//FWidgetTransform WidgetTransform = image->GetRenderTransform();
	//WidgetTransform.Translation.X = WorldLocation.X;
	//WidgetTransform.Translation.Y = WorldLocation.Y;

	//image->SetRenderTransform(WidgetTransform);

	return image.Get();
}

void UFHMinimapBase::SetTimeLimitText(FText text, FString LeftTime)
{
	FText NewText = FText::Format(text, FText::FromString(LeftTime));
	TimeLimitText->SetText(NewText);
}

void UFHMinimapBase::SetLeftCoreText(FText text, int32 LeftCore)
{
	FText NewText = FText::Format(text, LeftCore);
	LeftCoreText->SetText(NewText);
}

void UFHMinimapBase::SetLeftBatteryText(float MaxAmount, float CurrentAmount)
{
	BatteryProgressBar->SetPercent(CurrentAmount / MaxAmount);

	FText NewText = FText::FromString(FString::Printf(TEXT("%d"), static_cast<int32>(CurrentAmount)));
	LeftBatteryText->SetText(NewText);
}

void UFHMinimapBase::SetLeftTaxText(float CurrentPay, float TotalPay)
{
	FText NewText = FText::FromString(FString::Printf(TEXT("%d / %d"), static_cast<int32>(CurrentPay), static_cast<int32>(TotalPay)));
	LeftTaxText->SetText(NewText);
}

//void UFHMinimapBase::SetCurrentRankText(ELicenseRank Rank)
//{
//	FString RankString = StaticEnum<ELicenseRank>()->GetValueAsString(Rank);
//	FText NewText = FText::FromString(RankString);
//
//	CurrentRankText->SetText(NewText);
//}

void UFHMinimapBase::UpdateFloor(int32 NewFloor)
{
	TObjectPtr<UCanvasPanel> PrevCanvas = GetFloorCanvas(CurrentFloor);
	TObjectPtr<UCanvasPanel> CurCanvas = GetFloorCanvas(NewFloor);

	CurrentFloor = NewFloor;
	if (PrevCanvas)
	{
		PrevCanvas->SetVisibility(ESlateVisibility::Hidden);
	}
	if (CurCanvas)
	{
		CurCanvas->SetVisibility(ESlateVisibility::Visible);
	}
}

//void UFHMinimapBase::UpdateFloor(int32 Floor)
//{
//	if (!FloorCanvasMap.Contains(Floor))
//	{
//		PRINT_LOG(TEXT("Something goes wrong! Floor %d not found."), Floor);
//		return;
//	}
//
//	if (FloorCanvasMap.Contains(CurrentFloor))
//	{
//		TObjectPtr<UCanvasPanel> PrevFloorCanvas = FloorCanvasMap[CurrentFloor];
//		PrevFloorCanvas->SetRenderOpacity(0.0f);
//	}
//
//	TObjectPtr<UCanvasPanel> CurFloorCanvas = FloorCanvasMap[Floor];
//	CurFloorCanvas->SetRenderOpacity(1.0f);
//
//	CurrentFloor = Floor;
//}

//void UFHMinimapBase::ZoomOut()
//{
//	TargetScaleMult *= 0.8f;
//}
//
//void UFHMinimapBase::ZoomIn()
//{
//	TargetScaleMult *= 1.2f;
//}

//TObjectPtr<UCanvasPanel> UFHMinimapBase::GetFloorCanvas(int32 floor)
//{
//	if (FloorCanvasMap.Contains(floor) == false)
//	{
//		ensureMsgf(MapContents, TEXT("No map contents widget found."));
//		ensureMsgf(MinimapViewport, TEXT("No minimap viewport widget found."));
//		TObjectPtr<UCanvasPanel> floorCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
//		MapContents->AddChildToCanvas(floorCanvas);
//
//		TObjectPtr<UCanvasPanelSlot> panelSlot = Cast<UCanvasPanelSlot>(floorCanvas->Slot);
//		panelSlot->SetSize(MinimapViewport->GetDesiredSize());
//
//		FloorCanvasMap.Add(floor, floorCanvas);
//
//		if (floor == CurrentFloor)
//		{
//
//		}
//		else
//		{
//			floorCanvas->SetRenderOpacity(0.0f);
//		}
//
//		return floorCanvas;
//	}
//	else
//	{
//		return FloorCanvasMap[floor];
//	}
//}

//void UFHMinimapBase::UpdateMinimapRenderTargetImage(UTexture2D* MinimapRenderTarget)
//{
//	if (MinimapRenderTargetImage)
//	{
//		MinimapRenderTargetImage->SetBrushFromTexture(MinimapRenderTarget);
//	}
//}
