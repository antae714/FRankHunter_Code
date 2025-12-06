// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHUIManager.h"
#include "Blueprint/UserWidget.h"
#include "ManagerActorRegistrySubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFHUIManager::AFHUIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFHUIManager::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* LocalController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!LocalController)
	{
		return;
	}

	for (auto& [Key, Order, WidgetClass, IsAutoColapsed] : WidgetClasses)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(LocalController, WidgetClass, Key);
		if (Widget)
		{
			Widget->AddToViewport(Order);
			if (IsAutoColapsed)
			{

				Widget->SetVisibility(ESlateVisibility::Collapsed);
			}
			UIWidgets.Add({ Key, Widget });
		}
	}
}

// Called every frame
void AFHUIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AFHUIManager::RegistWidget(UUserWidget* Widget, FName WidgetKey)
{
	if (UIWidgets.FindRef(WidgetKey))
	{
		// return false if key is exist.
		return false;
	}
	UIWidgets.Add({ WidgetKey, Widget });
	return true;
}

