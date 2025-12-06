// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputConfigData.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UInputConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputLook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputClick;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputInteract1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputInteract2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputDropBackpack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputToggleFlashlight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputRun;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputUseSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputCloaking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputPopupStatus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInputAction* InputSwitchSlot1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInputAction* InputSwitchSlot2;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInputAction* InputSwitchSlot3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInputAction* InputSwitchSlot4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInputAction* InputScrollQuickSlot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInputAction* InputDropItem;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInputAction* InputToggleTerminal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Observer")
	class UInputAction* InputObserveNext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Observer")
	class UInputAction* InputObservePrev;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Observer")
	class UInputAction* InputObserveTurn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Observer")
	class UInputAction* InputObserveLookUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Observer")
	class UInputAction* InputObserveZoom;


	// ========= Emote =========
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emote")
	class UInputAction* InputEmote1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emote")
	class UInputAction* InputEmote2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emote")
	class UInputAction* InputEmote3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emote")
	class UInputAction* InputEmote4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emote")
	class UInputAction* InputEmote5;




	// ========= Debug =========
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> InputForDebugging;
};
