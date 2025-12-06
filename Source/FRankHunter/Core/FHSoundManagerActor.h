// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/ManagerActor.h"
#include "GameplayTagContainer.h"
#include "FHSoundManagerActor.generated.h"

UCLASS()
class FRANKHUNTER_API AFHSoundManagerActor : public AManagerActor
{
	GENERATED_BODY()

public:
	virtual FName GetManagerName() const { return TEXT("SoundManager"); }

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;


public:
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlaySoundAtLocationLocallyByTag(FGameplayTag SoundTag, FVector Location, float VolumeMult = 1.0f, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlaySoundAtLocationMulticastByTag(FGameplayTag SoundTag, FVector Location, float VolumeMult = 1.0f, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
protected:
	UFUNCTION(Server, Reliable)
	void Server_PlaySoundAtLocationByTag(FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject);
	void Server_PlaySoundAtLocationByTag_Implementation(FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySoundAtLocationByTag(FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject);
	void Multicast_PlaySoundAtLocationByTag_Implementation(FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject);


public:
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlaySound2DLocallyByTag(FGameplayTag SoundTag, float VolumeMult = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlaySound2DMulticastByTag(FGameplayTag SoundTag, float VolumeMult = 1.0f);
protected:
	UFUNCTION(Server, Reliable)
	void Server_PlaySound2DByTag(FGameplayTag SoundTag, float VolumeMult);
	void Server_PlaySound2DByTag_Implementation(FGameplayTag SoundTag, float VolumeMult);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySound2DByTag(FGameplayTag SoundTag, float VolumeMult);
	void Multicast_PlaySound2DByTag_Implementation(FGameplayTag SoundTag, float VolumeMult);


public:
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlaySoundAtLocationLocallyByName(FName SoundName, FVector Location, float VolumeMult = 1.0f, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlaySoundAtLocationMulticastByName(FName SoundName, FVector Location, float VolumeMult = 1.0f, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
protected:
	UFUNCTION(Server, Reliable)
	void Server_PlaySoundAtLocationByName(FName SoundName, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject);
	void Server_PlaySoundAtLocationByName_Implementation(FName SoundName, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySoundAtLocationByName(FName SoundName, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject);
	void Multicast_PlaySoundAtLocationByName_Implementation(FName SoundName, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject);


public:
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlaySound2DLocallyByName(FName SoundName, float VolumeMult = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlaySound2DMulticastByName(FName SoundName, float VolumeMult = 1.0f);
protected:
	UFUNCTION(Server, Reliable)
	void Server_PlaySound2DByName(FName SoundName, float VolumeMult);
	void Server_PlaySound2DByName_Implementation(FName SoundName, float VolumeMult);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySound2DByName(FName SoundName, float VolumeMult);
	void Multicast_PlaySound2DByName_Implementation(FName SoundName, float VolumeMult);


	






public:
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlayAndManageSound2DLocallyByTag(FName ManageName, FGameplayTag SoundTag, float VolumeMult = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlayAndManageSound2DMulticastByTag(FName ManageName, FGameplayTag SoundTag, float VolumeMult = 1.0f);
protected:
	UFUNCTION(Server, Reliable)
	void Server_PlayAndManageSound2DByTag(FName ManageName, FGameplayTag SoundTag, float VolumeMult);
	void Server_PlayAndManageSound2DByTag_Implementation(FName ManageName, FGameplayTag SoundTag, float VolumeMult);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAndManageSound2DByTag(FName ManageName, FGameplayTag SoundTag, float VolumeMult);
	void Multicast_PlayAndManageSound2DByTag_Implementation(FName ManageName, FGameplayTag SoundTag, float VolumeMult);

public:
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlayAndManageSoundAtLocationLocallyByTag(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult = 1.0f, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void PlayAndManageSoundAtLocationMulticastByTag(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult = 1.0f, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
protected:
	UFUNCTION(Server, Reliable)
	void Server_PlayAndManageSoundAtLocationByTag(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
	void Server_PlayAndManageSoundAtLocationByTag_Implementation(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAndManageSoundAtLocationByTag(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);
	void Multicast_PlayAndManageSoundAtLocationByTag_Implementation(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise = false, float Loudness = 1.0f, AActor* NoiseSubject = nullptr);

public:
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void DestroyManagedAudioComponentLocally(FName ManageName, bool TurnOffBlending = true);
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void DestroyManagedAudioComponentMulticast(FName ManageName, bool TurnOffBlending = true);
protected:
	UFUNCTION(Server, Reliable)
	void Server_DestroyManagedAudioComponent(FName ManageName, bool TurnOffBlending);
	void Server_DestroyManagedAudioComponent_Implementation(FName ManageName, bool TurnOffBlending);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DestroyManagedAudioComponent(FName ManageName, bool TurnOffBlending);
	void Multicast_DestroyManagedAudioComponent_Implementation(FName ManageName, bool TurnOffBlending);

public:
	UPROPERTY()
	TMap<FName, TObjectPtr<class UAudioComponent>> ManagedComponents;
	UPROPERTY()
	TMap<FName, FTimerHandle> ManagedComponentsBlendingTimers;
};
