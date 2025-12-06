// Copyright F Rank Hunter. All Rights Reserved.


#include "NPC/FHLobbyNPC.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHInteractableComponent.h"
#include "Core/FHPlayerController.h"
#include "UI/FHLobbyNPCWidget.h"
#include "Kismet/GameplayStatics.h"

#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"

AFHLobbyNPC::AFHLobbyNPC()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractableComponent = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComponent"));
}

void AFHLobbyNPC::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->OnInteractClient.AddDynamic(this, &AFHLobbyNPC::OnInteract);
}

void AFHLobbyNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHLobbyNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFHLobbyNPC::OnInteract(AFHPlayerBase* Player, const UInputAction* InputAction)
{
	if (Player->IsLocallyControlled() == false)
	{
		return;
	}

	InteractPlayer = Player;
	InteractPlayerController = Cast<AFHPlayerController>(Player->GetController());

	AFHPlayerController* pc = Cast<AFHPlayerController>(Player->GetController());
	if (!WidgetInstance.IsValid())
	{
		WidgetInstance = CreateWidget<UFHLobbyNPCWidget>(pc, WidgetClass, TEXT("LobbyNPCWidget"));
		WidgetInstance->AddToViewport(0);
		WidgetInstance->WidgetMaster = this;
		WidgetInstance->InputMappingContext = Player->GetInputMappingContext();
	}
	else
	{
		WidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}

	// ======== Play Sound =========
	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		SoundManager->PlaySound2DLocallyByTag(GET_GAMEPLAY_TAG("Sound.Interact.Normal"));
	}

	//OnFocusIn();
}

void AFHLobbyNPC::OnFocusIn()
{
	InteractPlayer->SetAllowInteract(false);

	FInputModeUIOnly im;
	im.SetWidgetToFocus(WidgetInstance->TakeWidget());
	im.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InteractPlayerController->SetInputMode(im);
	InteractPlayerController->SetShowMouseCursor(true);

	FVector FocusPosition = GetActorLocation() + FVector{0, 0, 50};
	InteractPlayer->StartFocusAt(FocusPosition);
}

void AFHLobbyNPC::OnFocusOut()
{
	if (InteractPlayer)
	{
		InteractPlayer->SetAllowInteract(true);
		InteractPlayer->EndFocusing();
	}
	if (InteractPlayerController)
	{
		FInputModeGameOnly im;
		InteractPlayerController->SetInputMode(im);
		InteractPlayerController->SetShowMouseCursor(false);

		// 이 함수는 위젯을 Pending kill 상태로 만든다고 한다. 아직 참조가 유지되고 있어도 죽여버리는 듯.
		//RemoveFromParent();
		WidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}
}