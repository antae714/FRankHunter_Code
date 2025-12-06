// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/FHSettingRegistry.h"
#include "GameSettingCollection.h"
#include "Player/FHLocalPlayer.h"
#include "Settings/FHDeviceSettings.h"
#include "Settings/FHPlayerProfileSettings.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "Settings/CustomSettings/FHSettingPerformanceStat.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

UFHSettingRegistry::UFHSettingRegistry()
{
}

UFHSettingRegistry* UFHSettingRegistry::Get(UFHLocalPlayer* InLocalPlayer)
{
	UFHSettingRegistry* Registry = FindObject<UFHSettingRegistry>(InLocalPlayer, TEXT("FHSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<UFHSettingRegistry>(InLocalPlayer, TEXT("FHSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

void UFHSettingRegistry::SaveChanges()
{
	Super::SaveChanges();

	if (UFHLocalPlayer* LocalPlayer = Cast<UFHLocalPlayer>(OwningLocalPlayer))
	{
		UFHDeviceSettings::Get()->ApplySettings(false);

		UFHPlayerProfileSettings* PlayerProfileSettings = LocalPlayer->GetPlayerProfileSettings();
		PlayerProfileSettings->ApplySettings();
		PlayerProfileSettings->SaveSettings();
	}
}

void UFHSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	InitializeControlSettings();
	RegisterSetting(ControlSettings);

	InitializeGraphicSettings();
	RegisterSetting(GraphicSettings);

	InitializeSoundSettings();
	RegisterSetting(SoundSettings);

	InitializeGameplaySettings();
	RegisterSetting(GameplaySettings);
}

bool UFHSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (UFHLocalPlayer* LocalPlayer = Cast<UFHLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetPlayerProfileSettings() == nullptr)
			{
				return false;
			}
		}
		
		return true;
	}

	return false;
}

void UFHSettingRegistry::AddPerformanceStatPage(UGameSettingCollection* PerfStatsOuterCategory)
{
	UGameSettingCollectionPage* StatsPage = NewObject<UGameSettingCollectionPage>();
	StatsPage->SetDevName(TEXT("PerfStatsPage"));
	StatsPage->SetDisplayName(LOCTEXT("PerfStatsPage_Name", "PerformanceStats"));
	StatsPage->SetDescriptionRichText(LOCTEXT("PerfStatsPage_Description", "Configure the display of performance statistics."));
	StatsPage->SetNavigationText(LOCTEXT("PerfStatsPage_Navigation", "Edit"));

	StatsPage->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

	PerfStatsOuterCategory->AddSetting(StatsPage);

	/** Performance stats */
	{
		UGameSettingCollection* StatCategory_Performance = NewObject<UGameSettingCollection>();
		StatCategory_Performance->SetDevName(TEXT("StatCategory_Performance"));
		StatCategory_Performance->SetDisplayName(LOCTEXT("StatCategory_Performance_Name", "Performance"));
		StatsPage->AddSetting(StatCategory_Performance);

		/** ClientFPS */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::ClientFPS);
			Setting->SetDisplayName(LOCTEXT("PerfStat_ClientFPS", "ClientFPS"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_ClientFPS", "Client frame rate (higher is better)"));
			StatCategory_Performance->AddSetting(Setting);
		}

		/** ServerFPS */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::ServerFPS);
			Setting->SetDisplayName(LOCTEXT("PerfStat_ServerFPS", "ServerFPS"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_ServerFPS", "Server frame rate"));
			StatCategory_Performance->AddSetting(Setting);
		}

		/** FrameTime */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::FrameTime);
			Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime", "FrameTime"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime", "The total frame time."));
			StatCategory_Performance->AddSetting(Setting);
		}

		/** IdleTime */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::IdleTime);
			Setting->SetDisplayName(LOCTEXT("PerfStat_IdleTime", "IdleTime"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_IdleTime", "The amount of time spent waiting idle for frame pacing."));
			StatCategory_Performance->AddSetting(Setting);
		}

		/** CPUGameTime */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::FrameTime_GameThread);
			Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime_GameThread", "CPUGameTime"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime_GameThread", "The amount of time spent on the main game thread."));
			StatCategory_Performance->AddSetting(Setting);
		}

		/** CPURenderTime */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::FrameTime_RenderThread);
			Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime_RenderThread", "CPURenderTime"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime_RenderThread", "The amount of time spent on the rendering thread."));
			StatCategory_Performance->AddSetting(Setting);
		}

		/** CPURHITime */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::FrameTime_RHIThread);
			Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime_RHIThread", "CPURHITime"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime_RHIThread", "The amount of time spent on the Render Hardware Interface thread."));
			StatCategory_Performance->AddSetting(Setting);
		}

		/** GPURenderTime */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::FrameTime_GPU);
			Setting->SetDisplayName(LOCTEXT("PerfStat_FrameTime_GPU", "GPURenderTime"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_FrameTime_GPU", "The amount of time spent on the GPU."));
			StatCategory_Performance->AddSetting(Setting);
		}
	}

	/** Network stats */
	{
		UGameSettingCollection* StatCategory_Network = NewObject<UGameSettingCollection>();
		StatCategory_Network->SetDevName(TEXT("StatCategory_Network"));
		StatCategory_Network->SetDisplayName(LOCTEXT("StatCategory_Network_Name", "Network"));
		StatsPage->AddSetting(StatCategory_Network);

		/** Ping */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::Ping);
			Setting->SetDisplayName(LOCTEXT("PerfStat_Ping", "Ping"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_Ping", "The roundtrip latency of your connection to the server."));
			StatCategory_Network->AddSetting(Setting);
		}

		/** IncomingPacketLoss */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::PacketLoss_Incoming);
			Setting->SetDisplayName(LOCTEXT("PerfStat_PacketLoss_Incoming", "IncomingPacketLoss"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketLoss_Incoming", "The percentage of incoming packets lost."));
			StatCategory_Network->AddSetting(Setting);
		}

		/** OutgoingPacketLoss */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::PacketLoss_Outgoing);
			Setting->SetDisplayName(LOCTEXT("PerfStat_PacketLoss_Outgoing", "OutgoingPacketLoss"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketLoss_Outgoing", "The percentage of outgoing packets lost."));
			StatCategory_Network->AddSetting(Setting);
		}

		/** IncomingPacketRate */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::PacketRate_Incoming);
			Setting->SetDisplayName(LOCTEXT("PerfStat_PacketRate_Incoming", "IncomingPacketRate"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketRate_Incoming", "Rate of incoming packets (per second)"));
			StatCategory_Network->AddSetting(Setting);
		}

		/** OutgoingPacketRate */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::PacketRate_Outgoing);
			Setting->SetDisplayName(LOCTEXT("PerfStat_PacketRate_Outgoing", "OutgoingPacketRate"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketRate_Outgoing", "Rate of outgoing packets (per second)"));
			StatCategory_Network->AddSetting(Setting);
		}

		/** IncomingPacketSize */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::PacketSize_Incoming);
			Setting->SetDisplayName(LOCTEXT("PerfStat_PacketSize_Incoming", "IncomingPacketSize"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketSize_Incoming", "The average size (in bytes) of packets recieved in the last second."));
			StatCategory_Network->AddSetting(Setting);
		}

		/** OutgoingPacketSize */
		{
			UFHSettingPerformanceStat* Setting = NewObject<UFHSettingPerformanceStat>();
			Setting->SetStat(EFHDisplayablePerformanceStat::PacketSize_Outgoing);
			Setting->SetDisplayName(LOCTEXT("PerfStat_PacketSize_Outgoing", "OutgoingPacketSize"));
			Setting->SetDescriptionRichText(LOCTEXT("PerfStatDescription_PacketSize_Outgoing", "The average size (in bytes) of packets sent in the last second."));
			StatCategory_Network->AddSetting(Setting);
		}
	}
}

#undef LOCTEXT_NAMESPACE