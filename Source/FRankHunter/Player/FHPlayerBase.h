// Copyright F Rank Hunter. All Rights Reserved.

/**
* ACharacter::Mesh는 3인칭 메쉬 / 이와 별도로 FirstPersonMesh(1인칭 로컬 뷰에서 보는 용도) 선언
* 3인칭 메쉬는 AimOffset 적용하고 다른 플레이어가 볼 때 그대로 그리면 되고 / 1인칭 메쉬는 그냥 Pitch 회전 상속받으면 됨
*/


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GAS/FHGameplayTags.h"
#include "Interfaces\SiInventorySystemInterface.h"
#include "Common/CommonItemEnum.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Core/GameTeamTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Player/FHPlayerDescriptor.h"
#include "GameplayAbilitySpec.h"

#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"

#include "Player/HasCharacterVariation.h"
#include "DungeonGeneration/FHDungeonStructs.h"

#include "GAS/FHGameplayAbilityTargetDatas.h"
#include "FHPlayerBase.generated.h"

class AFHCart;
class IFHInteractable;
class UFHInteractTooltipWidget;
class UInputAction;
class AFHBackpack;
enum class ELicenseRank : uint8;
struct FPlayerSaveData;

USTRUCT()
struct FPlayerEquipment
{
	GENERATED_BODY()

	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> Mesh{ nullptr };
	UPROPERTY()
	EItemHoldingType MeshType{ EItemHoldingType::NoHanded };

	UPROPERTY()
	EItemHoldingSocketDirection TPHoldingDirection{ EItemHoldingSocketDirection::Nowhere };
	UPROPERTY()
	EItemHoldingSocketDirection FPHoldingDirection{ EItemHoldingSocketDirection::Nowhere };

	UPROPERTY()
	FVector TPEquipOffset{};
	UPROPERTY()
	FRotator TPEquipRotation{};

	UPROPERTY()
	FVector FPEquipOffset{};
	UPROPERTY()
	FRotator FPEquipRotation{};
};


UENUM(BlueprintType)
enum class EPlayerActionState : uint8
{
	None 					UMETA(DisplayName = "None"),
	Jump 					UMETA(DisplayName = "Jump"),
	Attack 					UMETA(DisplayName = "Attack"),
	ToggleTerminal 			UMETA(DisplayName = "ToggleTerminal"),
	Dance 					UMETA(DisplayName = "Dance")
};


UCLASS()
class FRANKHUNTER_API AFHPlayerBase : 
	public ACharacter, 
	public IAbilitySystemInterface, 
	public ISiInventorySystemInterface, 
	public IGenericTeamAgentInterface,
	public IHasCharacterVariation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UFHPlayerDescriptor> PlayerDescriptor;
	// =========== Sockets ===========
	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName RightGrabToolSocketName{ "GripPoint.R" };
	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName LeftGrabToolSocketName{ "GripPoint.L" };
	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName KeepToolsBackSocketName{ "KeepBack" };
	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName KeepToolsSideSocketName{ "KeepSide" };
	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName TerminalHangingSocketName{ "TerminalHang" };
	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName ItemDropPointSocketName{ "ItemDropPoint" };


	AFHPlayerBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
	UInputMappingContext* CurrentIMC{ nullptr };

	UFUNCTION(Client, Reliable)
	void AddInputMappingContext();
	void AddInputMappingContext_Implementation();
	UFUNCTION(Client, Reliable)
	void RemoveInputMappingContext();
	void RemoveInputMappingContext_Implementation();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual USiInventoryComponent* GetInventoryComponent() const override;


	// Begin IGenericTeamAgentInterface
public:
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~End IGenericTeamAgentInterface


	// Begin IHasCharacterVariation
public:
	virtual UObject* FindAssetData_Implementation(FName VariationId) const override;
	// ~End IHasCharacterVariation

	
	UFUNCTION(BlueprintCallable)
	void ChangeSuitColor(FLinearColor Color, float Multiplier = 0.1f);

public:
	bool IsDead();
	bool IsObserving();
	bool GetIsSprinting();

public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_PlayMontage(UAnimMontage* Montage, FName SectionName, float PlayRate, bool ExceptLocalPlayer = true);
	void Server_PlayMontage_Implementation(UAnimMontage* Montage, FName SectionName, float PlayRate, bool ExceptLocalPlayer = true);
protected:
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* Montage, FName SectionName, float PlayRate, bool ExceptLocalPlayer = true);
	void Multicast_PlayMontage_Implementation(UAnimMontage* Montage, FName SectionName, float PlayRate, bool ExceptLocalPlayer = true);

private:
	void AbilityInputPressed(int32 KeyID);
	void AbilityInputReleased(int32 KeyID);
	float GetMovementSpeedScale(const FVector2D& MoveValueNormalized, float MoveForwardScale, float MoveSideScale, float MoveBackwardScale);
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void Attack(int32 InputID);
	void UseSkill();
	void PopUpStatus();

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	void StartJump(int32 InputID);
	void EndJump(int32 InputID);

	void StartSprint(int32 InputID);
	void EndSprint(int32 InputID);

	void StartCrouch(int32 InputID);
	void EndCrouch(int32 InputID);
	


	void ToggleFlashlight();
public:
	bool bIsFlashlightOn{ false };
	bool bCanToggleFlashlight{ true };
	bool IsFlashlightOn();

	UFUNCTION(Server, Reliable)
	void ServerRPC_ToggleFlashlight(bool toggle);
private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ToggleFlashlight(bool toggle);

	UFUNCTION()
	void OnChangedItem(USiInventoryComponent* InventoryComponent, int32 index);



	// ========== Highlight nearby interactable objects ==============
	float ElapsedTimeFromLastCheckInteractables{ 0.0f };
	UPROPERTY()
	TSet<class UFHInteractableComponent*> NearbyInteractables;
	void CheckNearbyInteractableObjects(float DeltaTime);
	






	// Switch Item slot
	int32 CurrentSelectedQuickslotIndex{ 0 };
	void SwitchItemByInput(const FInputActionValue& Value, int32 SlotNum);
	void SwitchItem(int32 Index);
public:
	EItemHoldingType GetCurrentPointedItemHoldingType();
private:
	void ScrollQuickSlot(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void DoSomethingForDebugging(float param);




	// =========== Ragdoll ==============
protected:
	FVector DefaultThirdPersonMeshRelativeLocation;
	FRotator DefaultThirdPersonMeshRelativeRotation;
	UPROPERTY(BlueprintReadOnly)
	uint32 bIsRagdolling : 1{ false };
	UPROPERTY(BlueprintReadOnly)
	uint32 bGetUpFromFront : 1{ true };
public:
	bool IsRagdolling() { return bIsRagdolling; }
	bool IsGetUpFromFront() { return bGetUpFromFront; }

protected:
	UFUNCTION(BlueprintCallable)
	void SetRagdollingState(bool SetRagdolling);
	
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetRagdollingState(bool SetRagdolling);
	void Server_SetRagdollingState_Implementation(bool SetRagdolling);
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetRagdollingState(bool SetRagdolling);
	void Multicast_SetRagdollingState_Implementation(bool SetRagdolling);
	
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ThrowPlayerAway(FVector Direction, float Power = 10000.0f, FName TargetBone = TEXT("Pelvis"), bool bIsDead = false);
	void Server_ThrowPlayerAway_Implementation(FVector Direction, float Power = 10000.0f, FName TargetBone = TEXT("Pelvis"), bool bIsDead = false);
	UFUNCTION(NetMulticast, Reliable)
	void ImpulseToPlayer(FVector Direction, float Power, FName TargetBone = TEXT("Pelvis"));
	void ImpulseToPlayer_Implementation(FVector Direction, float Power, FName TargetBone = TEXT("Pelvis"));

public:
	UFUNCTION(Server, Reliable)
	void Server_RecoverFromRagdolling();
	void Server_RecoverFromRagdolling_Implementation();
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RecoverFromRagdolling(FQuat LastQuat, FVector LastLocation, FRotator LastRotation);
	void Multicast_RecoverFromRagdolling_Implementation(FQuat LastQuat, FVector LastLocation, FRotator LastRotation);
	FTimerHandle RecoverFromRagdollingTimer;


	FTimerHandle DissolveTimerHandle;
	FTimerHandle ObserverTimerHandle;


	// ============ VFX ==============
public:
	FTimerHandle StunEffectTimerHandle;



	UPROPERTY()
	TObjectPtr<ANiagaraActor> StunNiagaraActor;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VFX")
	void ActivateBloodSplashEffect(FVector Location, FRotator Rotation);
	void ActivateBloodSplashEffect_Implementation(FVector Location, FRotator Rotation);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VFX")
	void ActivateStunEffect();
	void ActivateStunEffect_Implementation();
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActivateBloodSplashEffect(FVector Location, FRotator Rotation);
	void Multicast_ActivateBloodSplashEffect_Implementation(FVector Location, FRotator Rotation);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActivateStunEffect();
	void Multicast_ActivateStunEffect_Implementation();





public:
	/// <summary>
	/// 카메라의 시선방향에 있는 첫 번째 액터를 반환합니다. 
	/// </summary>
	AActor* GetActorInfront(const float MaxLength, OUT float& Distance, bool bOnlyDetectInteractable = false);
	AActor* GetActorInfront(const float MaxLength, bool bOnlyDetectInteractable = false);
	// 주기적으로 라인 트레이스 / 앞에 있는 액터에 대한 처리



	// ======== focusing ==========
protected:
	enum class EPlayerFocusingPhase
	{
		None,
		FocusIn,
		FocusOut
	};
protected:
	EPlayerFocusingPhase FocusingPhase{ EPlayerFocusingPhase::None };
	FVector FocusingAtLocation{ 0, 0, 0 };
	float FocusingDefaultFOV{ 0.0f };
	float FocusingTargetFOV{ 0.0f };
	void UpdateFocusingState(float DeltaTime);
public:
	void StartFocusAt(AActor* target);
	void StartFocusAt(FVector target);
	void EndFocusing();





	UPROPERTY()
	TObjectPtr<UFHInteractTooltipWidget> CurrentInteractTooltipUIInstance;

	UPROPERTY()
	TObjectPtr<class UFHInteractableComponent> CurrentLookInteractableComp;
	bool LastLookInteractableCompBlockShowUISetting{ false };

	void AttachWidgetToViewport(TWeakObjectPtr<class UFHInteractableComponent> Interactable);
	void DetachWidgetFromViewportAndInitCurrentLookInteractableComp();
	void UpdateCurrentLookingInteractableActor();

	// 꾹 눌러서 상호작용하기
	// EIC는 어디까지나 누른 시간 기록만 해준다. 같은 액터에 대해서 누르고 있는 건지, 이미 트리거가 발동했는지 등을 판별하려면 수동으로 누적해야 함.
	float ElapsedTimeForHoldingInteract{ 0.0f };
	bool HoldingInteractTriggered{ false };
	UPROPERTY()
	TWeakObjectPtr<class UFHInteractableComponent> HoldingInteractTarget;

	void InitInteractState();

	// Falling
	virtual void Landed(const FHitResult& Hit) override;








	// =========== Interact ==============
protected:
	bool bAllowInteract{ true };
	UFUNCTION(Client, Reliable)
	void RemoveInteractTooltipWidgetOnUnposess();
	void RemoveInteractTooltipWidgetOnUnposess_Implementation();
public:
	void SetAllowInteract(bool allowInteract);
	
	void Interact(const struct FInputActionInstance& InputActionInstance);
	UFUNCTION(Server, Reliable)
	void Server_Interact(class UFHInteractableComponent* InteractActor, const UInputAction* InputAction);
	
	void InteractHold(const struct FInputActionInstance& InputActionInstance);
	UFUNCTION(Server, Reliable)
	void Server_InteractHold(class UFHInteractableComponent* InteractActor, float HoldDuration, const UInputAction* InputAction);
	
	void InteractReleased(const struct FInputActionInstance& InputActionInstance);
	UFUNCTION(Server, Reliable)
	void Server_InteractReleased(class UFHInteractableComponent* InteractActor, float HoldDuration, const UInputAction* InputAction);



public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

	// GameplayAbilitySysetm ========================================
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> abilitySystem;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UPROPERTY()
	TObjectPtr<class UFHAttributeSet_Health> HealthAttributeSet;
	UPROPERTY()
	TObjectPtr<class UFHAttributeSet_Stamina> StaminaAttributeSet;
	UPROPERTY()
	TObjectPtr<class UFHAttributeSet_Movement> MovementAttributeSet;
	UPROPERTY()
	TObjectPtr<class UFHAttributeSet_PlayerStatus> PlayerAttributeSet;

	FGameplayAbilitySpecHandle MoveSpecHandle;

private:
	void InitializeAbilitySystem();

protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitializeVOIPTalker();

public:
	// Get Attribute
	UFUNCTION(BlueprintCallable)
	float GetRunSpeed() const;
	UFUNCTION(BlueprintCallable)
	float GetCurrentStamina() const;
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetVitality() const;
	UFUNCTION(BlueprintCallable)
	float GetStrength() const;
	UFUNCTION(BlueprintCallable)
	float GetEndurance() const;
	UFUNCTION(BlueprintCallable)
	float GetAgility() const;
	UFUNCTION(BlueprintCallable)
	float GetWillpower() const;
	UFUNCTION(BlueprintCallable)
	float GetCarryWeight() const;
	UFUNCTION(BlueprintCallable)
	float GetAttackRange() const;
	UFUNCTION(BlueprintCallable)
	float GetAttackPower() const;
	UFUNCTION(BlueprintCallable)
	float GetMiningPower() const;
	UFUNCTION(BlueprintCallable)
	float GetActionSpeedMult() const;

	void ActivateSkill(FName SkillID);
	UPROPERTY()
	TSubclassOf<UGameplayAbility> SkillSubclass;


	// ============ Be Chased by creature ===============
	UPROPERTY()
	TSet<class AFHCreatureBase*> ChasingCreatures;
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void GetChasedByCreature(class AFHCreatureBase* Creature);
	void GetChasedByCreature_Implementation(class AFHCreatureBase* Creature);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void LooseAttentionFromCreature(class AFHCreatureBase* Creature);
	void LooseAttentionFromCreature_Implementation(class AFHCreatureBase* Creature);
	UFUNCTION(Client, Reliable)
	void PlayChasingSound();
	void PlayChasingSound_Implementation();
	UFUNCTION(Client, Reliable)
	void StopChasingSound();
	void StopChasingSound_Implementation();





	// MovementComponent
	UPROPERTY()
	TObjectPtr<class UFHCharacterMovementComponent> MovementComponent;

	// Regen Stamina
	FGameplayTag IsComfortTag;
	float elapsedTimeFromLastAct{ 0.0f };
	//UPROPERTY(Replicated)
	//float elapsedTimeFromLastSprinting{ 0.0f };
	uint8 IsComfort : 1 { false };
	void UpdateComfortState(float DeltaTime);
	void ResetElapsedTimeFromLastMove();

	// Test
	UFUNCTION(Server, Unreliable)
	void ApplyTestEffect();



protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> GrabMesh;
public:
	class UStaticMeshComponent* GetGrabMesh(){ return GrabMesh; }

public:
	UPROPERTY()
	TSubclassOf<AActor> CurrentCharacterData;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> FaceMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> TorsoMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> LegsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> FeetMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> BackpackMesh;
	UPROPERTY()
	TObjectPtr<class UGroomComponent> EyelashesGroom;
	UPROPERTY()
	TObjectPtr<class UGroomComponent> FuzzGroom;
	UPROPERTY()
	TObjectPtr<class UGroomComponent> EyebrowsGroom;
	UPROPERTY()
	TObjectPtr<class UGroomComponent> HairGroom;
	UPROPERTY()
	TObjectPtr<class UGroomComponent> MustacheGroom;
	UPROPERTY()
	TObjectPtr<class UGroomComponent> BeardGroom;
	UPROPERTY()
	TObjectPtr<class UPhysicsAsset> NormalPhysicsAsset;
	UPROPERTY()
	TObjectPtr<class UPhysicsAsset> RagdollPhysicsAsset;
	UPROPERTY()
	TSoftObjectPtr<class UPhysicsAsset> NormalPhysicsAssetSoft;
	UPROPERTY()
	TSoftObjectPtr<class UPhysicsAsset> RagdollPhysicsAssetSoft;
public:
	UFUNCTION(BlueprintCallable, CallInEditor)
	void ChangeMetahuman(TSubclassOf<AActor> MetahumanActor, TSoftObjectPtr<class UPhysicsAsset> _NormalPhysicsAsset, TSoftObjectPtr<class UPhysicsAsset> _RagdollPhysicsAsset);
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void ChangeMetahumanWithSelectInfo(FName Key);

protected:
	void ChangeGroomComponent(TObjectPtr<class UGroomComponent>& Storage, FString Name, TObjectPtr<UClass>& ComponentClass, TObjectPtr<UActorComponent>& ComponentTemplate);
	// 블루프린트 스태틱함수면 좋을듯
	void ChangeMesh(USkeletalMeshComponent* LeftMesh, USkeletalMeshComponent* RightMesh);

	void OnPlayerStateLoadEnd(FPlayerSaveData PlayerSaveData);

	// Facial Animation ==========================
public:
	// Obsolete. Use anim montage instead.
	UPROPERTY(BlueprintReadWrite)
	float Facial_JawOpen{ 0.0f };
	UPROPERTY(BlueprintReadWrite)
	float Facial_TeethShow{ 0.0f };
public:
	UFUNCTION(BlueprintCallable)
	void PlayTalkingAnimation();
	UFUNCTION(BlueprintCallable)
	void StopTalkingAnimation();
//public:
//	UFUNCTION(Server, Reliable, BlueprintCallable)
//	void Server_PlayTalkingAnimation();
//	void Server_PlayTalkingAnimation_Implementation();
//	UFUNCTION(Server, Reliable, BlueprintCallable)
//	void Server_StopTalkingAnimation();
//	void Server_StopTalkingAnimation_Implementation();
//protected:
//	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
//	void Multicast_PlayTalkingAnimation();
//	void Multicast_PlayTalkingAnimation_Implementation();
//	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
//	void Multicast_StopTalkingAnimation();
//	void Multicast_StopTalkingAnimation_Implementation();







public:
	// Camera ============
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> CameraComponent;

	//Light=================
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpotLightComponent> Flashlight;
	FVector Flashlight_NormalLocation{ 20, 0, 0 };
	FRotator Flashlight_NormalRotation{ 0, 0, 0 };
	FVector Flashlight_RagdollLocation{ 10, 15, 0 };
	FRotator Flashlight_RagdollRotation{ 0, 90, 0 };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UPointLightComponent> Facelight;
	FVector Facelight_NormalLocation{ 35, 0, -50 };
	FRotator Facelight_NormalRotation{ 0, 0, 0 };
	FVector Facelight_RagdollLocation{ 0, 22, 0 };
	FRotator Facelight_RagdollRotation{ 0, 0, 0 };
	//void AttachLightsAtCamera();
	//void AttachLightsAtHead();

	bool FlashlightStateCache{ false };
	void ShowThirdPerson(float ArmLength = 500.0f, FVector TargetOffset = {0, 0, 0});
	void ShowFirstPerson(float ArmLegnth = 0.0f, FVector TargetOffset = { 0, 0, 0 });
public:
	class UInputMappingContext* GetInputMappingContext();
protected:
	float QuickSlotScrollAccumulator = 0.f;
	const float QuickSlotScrollThreshold = 1.0f;

private:
	//Ref =============
	UPROPERTY()
	TObjectPtr<class UFHPlayerAnimInstance> PlayerAnimInstance;
public:
	UFUNCTION(BlueprintCallable)
	class UFHPlayerAnimMontageDataAsset* GetAnimMontages(){ return PlayerDescriptor->AnimMontages; }




	// NameTag =====================================
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="NameTag")
	TObjectPtr<class UWidgetComponent> NameTagWidgetComponent;
protected:
	void UpdateNameTagRotation();
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerNickname)
	FString PlayerNickname{ "Player" };
public:
	void SetNameTag(ELicenseRank Rank, FString Name);
	void SetNameTag_LicenseOnly(ELicenseRank Rank);
protected:
	void ChangeNameTag(ELicenseRank Rank, FString Name);

	UFUNCTION()
	void OnRep_PlayerNickname();




	// Replicate
protected:
	// Controller Rotation Pitch값은 자동 리플리케이트 안 됨
	UPROPERTY(Replicated)
	float AimPitch;
	UPROPERTY(Replicated)
	uint8 bIsAiming : 1{ false }; // 한 손 무기 에임 상태
	UPROPERTY(Replicated)
	uint8 bIsClicked : 1{ false };
	UPROPERTY(Replicated)
	FRotator ControlRotation;

	UFUNCTION(Server, Reliable)
	void Server_SetIsClicked(bool isClicked);
	void Server_SetIsClicked_Implementation(bool isClicked);

	void UpdateAimPitch();
public:
	float GetAimPitch(){ return AimPitch; }
	bool GetIsAiming(){ return bIsAiming; }
	bool GetIsClicked(){ return bIsClicked; }
	UFUNCTION(BlueprintCallable)
	void SetIsAiming(bool isAiming){ bIsAiming = isAiming; }
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


	// 사망과 관련된 로직이 Death 이벤트에 의해 트리거되는 GA와 아래 함수로 이분되어있음

	// FHPlayerController::EnterObserverMode에서 호출, 관전용
	UFUNCTION(NetMulticast, Reliable)
	void SetObservingFlag();
	void SetObservingFlag_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Die();
	void Die_Implementation();
protected:
	UFUNCTION(Client, Reliable)
	void Client_Die();
	void Client_Die_Implementation();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Die();
	void Multicast_Die_Implementation();
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "After Dead"))
	void K2_AfterDead();

	// Multicast_Die에서 알아서 갱신하므로 Replicated 플래그 불필요
	UPROPERTY()
	uint32 bIsDie : 1{ false };
	UPROPERTY()
	uint32 bIsObserving : 1{ false };
protected:

	// voice
public:
	float LastCachedVolume;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Voice")
	//TObjectPtr<class UVOIPTalker> VOIPComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item")
	TObjectPtr<class UFHItemDropPointSocketComponent> ItemDropPointComp;






	// =============== Cart =================
public:
	void SetCart(AFHCart* NewCart);
	
private:
	UPROPERTY(Transient, VisibleAnywhere, Category = "Cart")
	TObjectPtr<AFHCart> Cart;
	




protected:
	// ================= Terminal ===================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terminal")
	TObjectPtr<class UChildActorComponent> TerminalChildActor;
public:
	class AFHTerminalBase* GetTerminalActor();
	void ToggleTerminal(const FInputActionValue& Value);
	void ToggleTerminal_Impl(uint8 bIsOpen, uint8 SwapAnimation = true);
	void ConstructMinimapWidget(FVector RoomUnit, const TArray<FMinimapRoomSpec>& RoomSpecs, int32 DungeonLowestFloor, int32 DungeonHighestFloor, float RoomHeight, float GeneratorPosZ);
protected:
	UFUNCTION(Server, Reliable)
	void Server_ToggleTerminal(uint8 bIsOpen);
	void Server_ToggleTerminal_Implementation(uint8 bIsOpen);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleTerminal(uint8 bIsOpen);
	void Multicast_ToggleTerminal_Implementation(uint8 bIsOpen);




protected:
	// ==================== Emote ======================
	//UFUNCTION(Server, Reliable)
	void PlayEmote(EPlayerEmoteType EmoteType);
	//void PlayEmote_Implementation(EPlayerEmoteType EmoteType);
public:
	UFUNCTION(Server, Reliable)
	void Server_HideGrabMeshTemporarily();
	void Server_HideGrabMeshTemporarily_Implementation();
	UFUNCTION(Server, Reliable)
	void Server_ShowGrabMeshBack();
	void Server_ShowGrabMeshBack_Implementation();
protected:
	UPROPERTY()
	TObjectPtr<class UStaticMesh> GrabMeshCache{ nullptr };
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HideGrabMeshTemporarily();
	void Multicast_HideGrabMeshTemporarily_Implementation();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowGrabMeshBack();
	void Multicast_ShowGrabMeshBack_Implementation();

	
	




	// ======================= Equipment / Swap ============================
	// 현재 Equip 몽타쥬 첫 프레임에 SetGrabMeshEmptyTemp 함수를 호출하는 노티파이를 통해 수동으로 비움 -> 네트워크 지연이 심할 경우 PendingMesh가 리플리케이트 되기 전 몽타쥬가 재생될 우려 / 개선 바람
protected:
	// 인벤토리를 초기화하면서 SetIndex(0)를 1회 수행 후 실제 아이템을 로드하며 다시 실행
	uint32 bEquipmentInitialized : 1{ false };

	FTimerHandle InitEquipmentStateTimer;
	UFUNCTION(Server, Reliable)
	void InitializeEquipmentState();
	void InitializeEquipmentState_Implementation();

	// 사망시
	UFUNCTION(Server, Reliable)
	void Server_EmptyGrabMeshForcefully();
	void Server_EmptyGrabMeshForcefully_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetAnimHoldingTypeNoHanded();
	void Multicast_SetAnimHoldingTypeNoHanded_Implementation();
		

public:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentEquipment)
	FPlayerEquipment CurrentEquipment;
    UFUNCTION()
    void OnRep_CurrentEquipment(FPlayerEquipment OldEquipment);

	UPROPERTY()
	class UFHItemBase* PrevHoldingItem{ nullptr };
	
    UPROPERTY(Replicated)
	TSoftObjectPtr<UStaticMesh> PendingMesh{ nullptr };
    UPROPERTY(Replicated)
	EItemHoldingType PendingType{ EItemHoldingType::NoHanded };

	void OnChangeHoldingItemType(class UFHItemBase* NewItem);
	
	EItemHoldingType AnimHoldingType{ EItemHoldingType::NoHanded };

	UFUNCTION(BlueprintCallable)
	EItemHoldingType GetCurrentEquipmentHoldingType();

	// 다른 착용 몽타쥬 재생 중 슬롯을 비울 때, GA::EndAbility에서 ForceUpdateCurrentEquipmentAndGrabMesh를 바로 호출할 경우 집어넣는 모션이 재생 중에 PendingMesh(nullptr)로 업데이트 되는 문제

public:
	UFUNCTION(BlueprintCallable)
	void UsePendingEquipment();
	UFUNCTION(BlueprintCallable)
	void SetGrabMeshFromCurrentEquipment();
	UFUNCTION(BlueprintCallable)
	void SetGrabMeshEmptyTemporarily();

	// 아이템을 소모했을 경우
	UFUNCTION(NetMulticast, Reliable)
	void ForceEmptyCurrentEquipmentAndGrabMesh();
	void ForceEmptyCurrentEquipmentAndGrabMesh_Implementation();

	// GA가 정상 절차를 밟지 않고 의도치 않게 종료될 경우
	UFUNCTION(NetMulticast, Reliable)
	void ForceUpdateCurrentEquipmentAndGrabMesh();
	void ForceUpdateCurrentEquipmentAndGrabMesh_Implementation();
protected:
	UFUNCTION()
	void OnGrabMeshLoaded();

	// 바로 이전에 아이템을 사용했을 경우, 아이템 GA에서 해당 플래그를 켜고, 이후 호출되는 OnChangeHoldingItemType에서 이를 처리함.
	uint8 bEquippedItemDisappeardJustBefore : 1{ false };
	// 아이템 사용 모션을 재생하는 동안 메쉬 유지
	uint8 bUseMotionItemJustBefore : 1{ false };
	int tempCnt = 0;
public:
	UFUNCTION(BlueprintCallable)
	void InformPlayerEquippedItemDIsappeard();
	UFUNCTION(BlueprintCallable)
	void InformPlayerItemWhichHasUsingMotionUsed();







	// ============= Backpack =================
public:
	void PickUpBackpack(AActor* BackpackActor);

	UFUNCTION(Server, Reliable)
	void ServerPickUpBackpack(AActor* BackpackActor);

	//UFUNCTION(Client, Reliable)
	//void ClentCancelPickUpBackpack(AActor* BackpackActor);

	UFUNCTION()
	void OnRep_CurrentBackpackActor();

	void DropBackpackStart(const FInputActionInstance& InputActionInstance);
	void DropBackpack(const FInputActionInstance& InputActionInstance);
	void DropBackpackEnd(const FInputActionInstance& InputActionInstance);

	void DropBackpack_Internal();
	UFUNCTION(Server, Reliable)
	void ServerDropBackpack();

	AFHBackpack* GetCurrentBackpackActor() const;
private:

	UPROPERTY(ReplicatedUsing = OnRep_CurrentBackpackActor)
	TObjectPtr<AFHBackpack> CurrentBackpackActor;

	UPROPERTY()
	TObjectPtr<AFHBackpack> CurrentBackpackActorCache;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Backpack, meta = (AllowPrivateAccess = "true"))
	FName BackPackSocketName{ TEXT("BackPackSocket") };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Backpack, meta = (AllowPrivateAccess = "true"))
	FName BackPackDropSocketName{ TEXT("ItemDropPoint") };

	uint32 bIsBackPackDrop : 1 { false };

	float BackPackDropElapsedTime{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Backpack, meta = (AllowPrivateAccess = "true"))
	float BackPackDropTriggerTime{ 1.0f };

	// ~End Backpack











	// ===================== Dead Effect =============================
public:
	FName DissolveParameterName{ TEXT("Dissolve") };
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestToDissolvePlayer();
	void Server_RequestToDissolvePlayer_Implementation();
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartDissolvePlayer();
	void Multicast_StartDissolvePlayer_Implementation();

	UPROPERTY()
	TArray<TObjectPtr<class UMaterialInstanceDynamic>> DynamicMaterials;

	bool bIsDissolving{ false };
	bool bIsDissolvingAndRemoveGrooms{ false };
	float DissolvingRomveGroomThreshold{ 0.5f };
	FTimeline DissolveTimeLine;
	UPROPERTY(EditAnywhere, Category="Dissolve")
	TObjectPtr<UCurveFloat> DissolveFloatCurve;
	UFUNCTION()
	void UpdateDissolveScaleOfDynamicMaterials(float Value);
	UFUNCTION()
	void OnPlayerDissolvedCompletely();





public:
	UPROPERTY()
	uint32 bIsTerminalOpen : 1 {false};




	// ================= Observing Mode ======================
public:
	UFUNCTION(Client, Reliable)
	void SetObserverInputMode(bool bIsObserverMode);
	void SetObserverInputMode_Implementation(bool bIsObserverMode);


	// ================= Skill ======================
public:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAlchemistSkillEffect(FVector SpawnLocation);
	void Multicast_PlayAlchemistSkillEffect_Implementation(FVector SpawnLocation);

	UFUNCTION(Client, Reliable)
	void Client_PlayStunVignette(float Duration);
	void Client_PlayStunVignette_Implementation(float Duration);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayStopperSkillEffect(FVector SpawnLocation, float Radius);
	void Multicast_PlayStopperSkillEffect_Implementation(FVector SpawnLocation, float Radius);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySeekerSkillEffect(FVector SpawnLocation, float Radius);
	void Multicast_PlaySeekerSkillEffect_Implementation(FVector SpawnLocation, float Radius);

	void SetTwoPassMeshesVisible(bool IsVisible);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Effects", meta = (DisplayName = "On Play Alchemist Skill Effect"))
	void K2_PlayAlchemistSkillEffect(FVector SpawnLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Effects", meta = (DisplayName = "On Play Stopper Skill Effect"))
	void K2_PlayStopperSkillEffect(FVector SpawnLocation, float Radius);

	UFUNCTION(BlueprintImplementableEvent, Category = "Effects", meta = (DisplayName = "On Play Seeker Skill Effect"))
	void K2_PlaySeekerSkillEffect(FVector SpawnLocation, float Radius);

	void OnStunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);		

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UFHCloakingComponent> CloakingComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Cloaking")
	TObjectPtr<UMaterialInterface> RefractionMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TwoPassMesh_Torso;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TwoPassMesh_Face;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TwoPassMesh_Legs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TwoPassMesh_Feet;

	void OnCloakTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BeginCloaking();
	void Multicast_BeginCloaking_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndCloaking();
	void Multicast_EndCloaking_Implementation();

};