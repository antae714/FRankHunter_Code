// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHSoundManagerActor.h"
#include "Sound/FHSoundSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Components/AudioComponent.h"

void AFHSoundManagerActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void AFHSoundManagerActor::PlaySoundAtLocationLocallyByTag(FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject)
{
	UFHSoundSystem* System = GetGameInstance()->GetSubsystem<UFHSoundSystem>();
	if (System == nullptr) return;
	USoundBase* Sound = System->GetSoundAssetByTag(SoundTag);
	if (Sound == nullptr) return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, VolumeMult);

	if (HasAuthority() && ReportNoise)
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, Loudness, NoiseSubject);
	}
}

void AFHSoundManagerActor::PlaySoundAtLocationMulticastByTag(FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject)
{
	Server_PlaySoundAtLocationByTag(SoundTag, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::Server_PlaySoundAtLocationByTag_Implementation(FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject)
{
	Multicast_PlaySoundAtLocationByTag(SoundTag, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::Multicast_PlaySoundAtLocationByTag_Implementation(FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject)
{
	PlaySoundAtLocationLocallyByTag(SoundTag, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::PlaySound2DLocallyByTag(FGameplayTag SoundTag, float VolumeMult)
{
	UFHSoundSystem* System = GetGameInstance()->GetSubsystem<UFHSoundSystem>();
	if (System == nullptr) return;
	USoundBase* Sound = System->GetSoundAssetByTag(SoundTag);
	if (Sound == nullptr) return;

	UGameplayStatics::PlaySound2D(GetWorld(), Sound, VolumeMult);
}

void AFHSoundManagerActor::PlaySound2DMulticastByTag(FGameplayTag SoundTag, float VolumeMult)
{
	Server_PlaySound2DByTag(SoundTag, VolumeMult);
}

void AFHSoundManagerActor::Server_PlaySound2DByTag_Implementation(FGameplayTag SoundTag, float VolumeMult)
{
	Multicast_PlaySound2DByTag(SoundTag, VolumeMult);
}

void AFHSoundManagerActor::Multicast_PlaySound2DByTag_Implementation(FGameplayTag SoundTag, float VolumeMult)
{
	PlaySound2DLocallyByTag(SoundTag, VolumeMult);
}





void AFHSoundManagerActor::PlaySoundAtLocationLocallyByName(FName SoundName, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject)
{
	UFHSoundSystem* System = GetGameInstance()->GetSubsystem<UFHSoundSystem>();
	if (System == nullptr) return;
	USoundBase* Sound = System->GetSoundAssetByName(SoundName);
	if (Sound == nullptr) return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, VolumeMult);

	if (HasAuthority() && ReportNoise)
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, Loudness, NoiseSubject);
	}
}

void AFHSoundManagerActor::PlaySoundAtLocationMulticastByName(FName SoundName, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject)
{
	Server_PlaySoundAtLocationByName(SoundName, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::Server_PlaySoundAtLocationByName_Implementation(FName SoundName, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject)
{
	Multicast_PlaySoundAtLocationByName(SoundName, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::Multicast_PlaySoundAtLocationByName_Implementation(FName SoundName, FVector Location, float VolumeMult, bool ReportNoise, float Loudness, AActor* NoiseSubject)
{
	PlaySoundAtLocationLocallyByName(SoundName, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::PlaySound2DLocallyByName(FName SoundName, float VolumeMult)
{
	UFHSoundSystem* System = GetGameInstance()->GetSubsystem<UFHSoundSystem>();
	if (System == nullptr) return;
	USoundBase* Sound = System->GetSoundAssetByName(SoundName);
	if (Sound == nullptr) return;

	UGameplayStatics::PlaySound2D(GetWorld(), Sound, VolumeMult);
}

void AFHSoundManagerActor::PlaySound2DMulticastByName(FName SoundName, float VolumeMult)
{
	Server_PlaySound2DByName(SoundName, VolumeMult);
}

void AFHSoundManagerActor::Server_PlaySound2DByName_Implementation(FName SoundName, float VolumeMult)
{
	Multicast_PlaySound2DByName(SoundName, VolumeMult);
}

void AFHSoundManagerActor::Multicast_PlaySound2DByName_Implementation(FName SoundName, float VolumeMult)
{
	PlaySound2DLocallyByName(SoundName, VolumeMult);
}






void AFHSoundManagerActor::PlayAndManageSound2DLocallyByTag(FName ManageName, FGameplayTag SoundTag, float VolumeMult)
{
	UFHSoundSystem* System = GetGameInstance()->GetSubsystem<UFHSoundSystem>();
	if (System == nullptr) return;
	USoundBase* Sound = System->GetSoundAssetByTag(SoundTag);
	if (Sound == nullptr) return;

	if (ManagedComponents.Find(ManageName))
	{
		return;
	}

	ManagedComponents.Add({ManageName, UGameplayStatics::SpawnSound2D(GetWorld(), Sound, VolumeMult)});
}

void AFHSoundManagerActor::PlayAndManageSound2DMulticastByTag(FName ManageName, FGameplayTag SoundTag, float VolumeMult /*= 1.0f*/)
{
	Server_PlayAndManageSound2DByTag(ManageName, SoundTag, VolumeMult);
}

void AFHSoundManagerActor::Server_PlayAndManageSound2DByTag_Implementation(FName ManageName, FGameplayTag SoundTag, float VolumeMult)
{
	Multicast_PlayAndManageSound2DByTag(ManageName, SoundTag, VolumeMult);
}

void AFHSoundManagerActor::Multicast_PlayAndManageSound2DByTag_Implementation(FName ManageName, FGameplayTag SoundTag, float VolumeMult)
{
	PlayAndManageSound2DLocallyByTag(ManageName, SoundTag, VolumeMult);
}





void AFHSoundManagerActor::PlayAndManageSoundAtLocationLocallyByTag(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult /*= 1.0f*/, bool ReportNoise /*= false*/, float Loudness /*= 1.0f*/, AActor* NoiseSubject /*= nullptr*/)
{
	UFHSoundSystem* System = GetGameInstance()->GetSubsystem<UFHSoundSystem>();
	if (System == nullptr) return;
	USoundBase* Sound = System->GetSoundAssetByTag(SoundTag);
	if (Sound == nullptr) return;

	if (ManagedComponents.Find(ManageName))
	{
		return;
	}

	ManagedComponents.Add({ ManageName, UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, Location, FRotator::ZeroRotator, VolumeMult) });

	if (HasAuthority() && ReportNoise)
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, Loudness, NoiseSubject);
	}
}

void AFHSoundManagerActor::PlayAndManageSoundAtLocationMulticastByTag(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult /*= 1.0f*/, bool ReportNoise /*= false*/, float Loudness /*= 1.0f*/, AActor* NoiseSubject /*= nullptr*/)
{
	Server_PlayAndManageSoundAtLocationByTag(ManageName, SoundTag, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::Server_PlayAndManageSoundAtLocationByTag_Implementation(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise /*= false*/, float Loudness /*= 1.0f*/, AActor* NoiseSubject /*= nullptr*/)
{
	Multicast_PlayAndManageSoundAtLocationByTag(ManageName, SoundTag, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::Multicast_PlayAndManageSoundAtLocationByTag_Implementation(FName ManageName, FGameplayTag SoundTag, FVector Location, float VolumeMult, bool ReportNoise /*= false*/, float Loudness /*= 1.0f*/, AActor* NoiseSubject /*= nullptr*/)
{
	PlayAndManageSoundAtLocationLocallyByTag(ManageName, SoundTag, Location, VolumeMult, ReportNoise, Loudness, NoiseSubject);
}

void AFHSoundManagerActor::DestroyManagedAudioComponentLocally(FName ManageName, bool TurnOffBlending)
{
	if (ManagedComponents.Contains(ManageName))
	{
		if (IsValid(ManagedComponents[ManageName]) == false)
		{
			ManagedComponents.Remove(ManageName);
			return;
		}
	}
	else
	{
		return;
	}

	if (TurnOffBlending)
	{
		ManagedComponents[ManageName].Get()->FadeOut(1.0f, 0.0f);

		if (FTimerHandle* ExistingTimerHandle = ManagedComponentsBlendingTimers.Find(ManageName))
		{
			GetWorld()->GetTimerManager().ClearTimer(*ExistingTimerHandle);
			ManagedComponentsBlendingTimers.Remove(ManageName);
		}

		FTimerHandle NewTimerHandle;
		TWeakObjectPtr<AFHSoundManagerActor> WeakThis{ this };

		GetWorld()->GetTimerManager().SetTimer(NewTimerHandle,
			FTimerDelegate::CreateLambda([WeakThis, ManageName]()
		{
			if (!WeakThis.IsValid())
			{
				return;
			}

			AFHSoundManagerActor* StrongThis = WeakThis.Get();

			if (StrongThis->ManagedComponents.Contains(ManageName))
			{
				UAudioComponent* Component = StrongThis->ManagedComponents[ManageName].Get();
				if (Component)
				{
					Component->Stop();
				}
				StrongThis->ManagedComponents.Remove(ManageName);
			}

			StrongThis->ManagedComponentsBlendingTimers.Remove(ManageName);
		}),
		1.0f, false);

		ManagedComponentsBlendingTimers.Add(ManageName, NewTimerHandle);
	}
	else
	{
		ManagedComponents[ManageName].Get()->Stop();
		//ManagedComponents[ManageName].Get()->DestroyComponent();
		ManagedComponents.Remove(ManageName);
	}
}

void AFHSoundManagerActor::DestroyManagedAudioComponentMulticast(FName ManageName, bool TurnOffBlending)
{
	Server_DestroyManagedAudioComponent(ManageName, TurnOffBlending);
}

void AFHSoundManagerActor::Server_DestroyManagedAudioComponent_Implementation(FName ManageName, bool TurnOffBlending)
{
	Multicast_DestroyManagedAudioComponent(ManageName, TurnOffBlending);
}

void AFHSoundManagerActor::Multicast_DestroyManagedAudioComponent_Implementation(FName ManageName, bool TurnOffBlending)
{
	DestroyManagedAudioComponentLocally(ManageName, TurnOffBlending);
}
