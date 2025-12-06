// Copyright F Rank Hunter.. All Rights Reserved.


#include "Props/OpenGateSelect.h"
#include "Lobby/FHLobbyGameMode.h"
#include "Core/FHPlayerController.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHInteractableComponent.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHUIManager.h"
#include "UI/Lobby/FH_W_SelectGate.h"

// Sets default values
AOpenGateSelect::AOpenGateSelect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));
}

// Called when the game starts or when spawned
void AOpenGateSelect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOpenGateSelect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOpenGateSelect::Interact_Impl(AFHPlayerBase* Player)
{
	//AFHPlayerController* PC = Cast<AFHPlayerController>(Player->GetController());
	//if (PC)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("PlayerController."));
	//	PC->OpenGateSelectUI();
	//}

	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (UIManager)
	{
		UFH_W_SelectGate* SelectGateWidget = UIManager->GetWidget<UFH_W_SelectGate>(TEXT("SelectGate"));
		if (SelectGateWidget)
		{
			SelectGateWidget->ActivateWidget();
			SelectGateWidget->OnWidgetOpened();
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(SelectGateWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			AFHPlayerController* PC = Cast<AFHPlayerController>(Player->GetController());
			if (PC)
			{
				PC->bIsOtherWidgetOpened = true;
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;
			}
		}
	}
}

