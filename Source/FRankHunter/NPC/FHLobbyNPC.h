// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FHLobbyNPC.generated.h"

class UInputAction;


UCLASS()
class FRANKHUNTER_API AFHLobbyNPC : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UFHLobbyNPCWidget> WidgetClass;
	UPROPERTY()
	TWeakObjectPtr<class UFHLobbyNPCWidget> WidgetInstance;
	
	UPROPERTY()
	class AFHPlayerBase* InteractPlayer{ nullptr };

	UPROPERTY()
	class AFHPlayerController* InteractPlayerController{ nullptr };


public:
	AFHLobbyNPC();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnInteract(class AFHPlayerBase* Player, const UInputAction* InputAction);
public:
	void OnFocusIn();
	void OnFocusOut();
};
