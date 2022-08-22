// Fill out your copyright notice in the Description page of Project Settings.


#include "CSGameModeBase.h"

#include "CSBasePlayerState.h"
#include "Character/BasePlayerController.h"
#include "Character/BaseCharacter.h"
#include "Widgets/BaseGameHUD.h"

#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Items/QuestStartFinishPoint.h"
#include "Items/BaseItem.h"
#include "Items/QuestFinishSpot.h"

DEFINE_LOG_CATEGORY_STATIC(GameModeLog, All, All)

ACSGameModeBase::ACSGameModeBase()
{
	DefaultPawnClass = ABaseCharacter::StaticClass();
	PlayerControllerClass = ABasePlayerController::StaticClass();
	HUDClass = ABaseGameHUD::StaticClass();
	PlayerStateClass = ACSBasePlayerState::StaticClass();
}
void ACSGameModeBase::StartPlay()
{
	Super::StartPlay();
	SetMatchState(CSMatchState::InProgress);
	GetWorldTimerManager().SetTimer(WorldTimer, this, &ACSGameModeBase::UpdateWorldTime, 1.0f, true, 1.0f);
	GenerateQuestsLocations();
	StartQuestsGenerate();
}

void ACSGameModeBase::SetMatchState(CSMatchState State)
{
	if (MatchState != State)
	MatchState = State;
	OnMatchStateChanged.Broadcast(MatchState);
}
void ACSGameModeBase::UpdateWorldTime()
{
	if (WorldTime < 1440.f)
	{
		WorldTime++;
		OnWorldTimeUpdate.Broadcast(WorldTime);
	}
	else
	{
		WorldTime = 0.f;
		OnWorldTimeUpdate.Broadcast(WorldTime);
	}
}
void ACSGameModeBase::PushUpdateWorldTime()
{
	OnWorldTimeUpdate.Broadcast(WorldTime);
}

// QUEST GENERATOR SHOULD BE REPLACED TO DIFFERENT CLASS
void ACSGameModeBase::GenerateQuestsLocations()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestStartFinishPoint::StaticClass(), FoundActors);
	for (int i = 0; i < FoundActors.Num(); i++)
	{
		FoundActorsLocations.Add(FoundActors[i]->GetActorLocation());
		UE_LOG(GameModeLog, Display, TEXT("Location %i %s"), i, *FoundActorsLocations[i].ToString());
	}
}

void ACSGameModeBase::GenerateQuest()
{
	if (QuestsAmount >= MaxQuests)
	{
		StopQuestsGenerate();
		GetWorldTimerManager().SetTimer(QuestsUpdaterHandle, this, &ACSGameModeBase::ReplaceOldQuest, UpdateRate, true, 0.0f);
	}
	else
	{	
		CreateQuestData();
		Quests.Add(TempQuest);
		QuestsAmount++;
		OnQuestsUpdate.Broadcast();
		UE_LOG(GameModeLog, Display, TEXT("Quests Amount %i"), QuestsAmount);
		UE_LOG(GameModeLog, Display, TEXT("Quests Amount %s"), *UEnum::GetDisplayValueAsText(TempQuest.ItemType).ToString());
	}
}
void ACSGameModeBase::ReplaceOldQuest()
{
	auto TempIndex = FMath::RandRange(0, MaxQuests - 1);
	Quests.RemoveAt(TempIndex);
	CreateQuestData();
	Quests.Add(TempQuest);
	OnQuestsUpdate.Broadcast();
	UE_LOG(GameModeLog, Display, TEXT("Quests Amount %i"), TempIndex);
	UE_LOG(GameModeLog, Display, TEXT("Quests Amount %s"), *UEnum::GetDisplayValueAsText(Quests[TempIndex].ItemType).ToString());
}

void ACSGameModeBase::CreateQuestData()
{
	int32 RandomStartInt = FMath::RandRange(0, FoundActorsLocations.Num() - 1);
	int32 RandomEndInt = FMath::RandRange(0, FoundActorsLocations.Num() - 1);
	if (RandomStartInt == RandomEndInt)
	{
		RandomEndInt++;
		if (RandomEndInt > FoundActorsLocations.Num() - 1)
		{
			RandomEndInt = RandomEndInt - 2;
		}
	}
	else
	{
		FVector RandomStartLocation = FoundActorsLocations[RandomStartInt];
		FVector RandomEndLocation = FoundActorsLocations[RandomEndInt];
		float QuestLength = FVector::Dist(RandomStartLocation, RandomEndLocation);
		int32 ItemIndex = FMath::RandRange(0, 7);
		EGameItemTypes ItemType = EGameItemTypes(ItemIndex);
		float QuestReward = QuestLength / 10 * RewardCoeficients[ItemIndex];
		float QuestTimer = QuestLength / 50;
		TempQuest.StartLocation = RandomStartLocation;
		TempQuest.EndLocation = RandomEndLocation;
		TempQuest.Reward = QuestReward;
		TempQuest.Weight = QuestLength / 50;
		TempQuest.Exp = QuestLength / 4;
		TempQuest.TimeToFinish = QuestTimer;
		TempQuest.ItemType = ItemType;
		TempQuest.QuestName = RandomStartLocation.ToString() + " " + RandomEndLocation.ToString();
		TempQuest.QuestID = FMath::RandRange(1, 1000000);

		TArray<int32> IDsArray;
		for (int i = 0; i < Quests.Num(); i++)
		{
			IDsArray.Add(Quests[i].QuestID);
		}
		for (int i = 0; i < CharQuests.Num(); i++)
		{
			IDsArray.Add(CharQuests[i].QuestID);
		}
		while (IDsArray.Contains(TempQuest.QuestID)) { TempQuest.QuestID++; }
	}
}
void ACSGameModeBase::StartQuestsGenerate()
{
	GetWorldTimerManager().SetTimer(QuestsGeneratorHandle, this, &ACSGameModeBase::GenerateQuest, GenerateRate, true, 1.0f);
}
void ACSGameModeBase::StopQuestsGenerate()
{
	GetWorldTimerManager().ClearTimer(QuestsGeneratorHandle);
}
void ACSGameModeBase::UpdateMaxQuests(int32 Level)
{
	if(QuestAmountCoeficients.IsValidIndex(Level))
	MaxQuests = QuestAmountCoeficients[Level];
	if (QuestsUpdaterHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(QuestsUpdaterHandle);
	}
	StartQuestsGenerate();
}
void ACSGameModeBase::TakeQuest(int QuestArrayIndex)
{
	CharQuests.Add(Quests[QuestArrayIndex]);
	TakenQuest = CharQuests.Last();
	GetWorldTimerManager().SetTimer(TimerToFinish, this, &ACSGameModeBase::QuestTimer, 1.0f, true, 1.0f);
	Quests.RemoveAt(QuestArrayIndex);
	CreateQuestData();
	Quests.Add(TempQuest);
	OnQuestsUpdate.Broadcast();
	SpawnItemQuest();
	SpawnFinishSpot();
	OnQuestTakenItemTaken.Broadcast(true, false, TakenQuest.QuestID);
}
void ACSGameModeBase::PassQuest(int QuestId)
{
	for (int i = 0; i < CharQuests.Num(); i++)
	{
		if (CharQuests[i].QuestID == QuestId)
		{
			PassedQuest = CharQuests[i];
			CharQuests.RemoveAt(i);
			TArray<AActor*> ItemsWorldArray;
			TArray<ABaseItem*> CastedItemsWorldArray;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), ItemsWorldArray);
			for (int a = 0; a < ItemsWorldArray.Num(); a++)
			{
				CastedItemsWorldArray.Add(Cast<ABaseItem>(ItemsWorldArray[a]));
				if (QuestId == CastedItemsWorldArray[a]->ItemID)
				{
					CastedItemsWorldArray[a]->Destroy();
					break;
				}
			}
			OnQuestPassed.Broadcast(PassedQuest.Reward, PassedQuest.Exp, PassedQuest.QuestID);
			break;
		}
	}
}
void ACSGameModeBase::OnTakeItem(int32 ID)
{
	OnQuestTakenItemTaken.Broadcast(false, true, ID);
}
void ACSGameModeBase::QuestTimer()
{
	for (int i = 0; i < CharQuests.Num(); i++)
	{
		CharQuests[i].TimeToFinish--;
		if (CharQuests[i].TimeToFinish <= 0.0f)
		{
			FailedQuest = CharQuests[i];
			CharQuests.RemoveAt(i);
			OnTimerExpired.Broadcast(FailedQuest.Reward, FailedQuest.Exp, FailedQuest.QuestID);
			DeleteFailedQuestItem(FailedQuest.QuestID);
			DeleteFailedQuestSpot(FailedQuest.QuestID);
		}
	}
	OnTimerUpdate.Broadcast();
}

void ACSGameModeBase::SpawnItemQuest()
{
	FActorSpawnParameters SpawnParams;
	FTransform Transform;
	Transform.SetLocation(TakenQuest.StartLocation);
	auto SpawnedItem = GetWorld()->SpawnActor<ABaseItem>(SpawnItems[(int)TakenQuest.ItemType], Transform, SpawnParams);
	auto CastedSpawnedItem = Cast<ABaseItem>(SpawnedItem);
	if (CastedSpawnedItem)
	CastedSpawnedItem->ItemID = TakenQuest.QuestID;
	CastedSpawnedItem->ItemType = (int32)TakenQuest.ItemType;
}
void ACSGameModeBase::SpawnFinishSpot()
{
	FActorSpawnParameters SpawnParams;
	FTransform Transform;
	Transform.SetLocation(TakenQuest.EndLocation);
	auto SpawnedSpot = GetWorld()->SpawnActor<AQuestFinishSpot>(FinishSpot, Transform, SpawnParams);
	auto CastedSpawnedSpot = Cast<AQuestFinishSpot>(SpawnedSpot);
	if (CastedSpawnedSpot)
	CastedSpawnedSpot->QuestID = TakenQuest.QuestID;
}
void ACSGameModeBase::DeleteFailedQuestItem(int32 FailedQuestID)
{
	TArray<AActor*> ItemsWorldArray;
	TArray<ABaseItem*> CastedItemsWorldArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), ItemsWorldArray);
	for (int i = 0; i < ItemsWorldArray.Num(); i++)
	{
		CastedItemsWorldArray.Add(Cast<ABaseItem>(ItemsWorldArray[i]));
		if (FailedQuestID == CastedItemsWorldArray[i]->ItemID)
		{
			CastedItemsWorldArray[i]->Destroy();
			break;
		}
	}
}
void ACSGameModeBase::DeleteFailedQuestSpot(int32 FailedQuestID)
{
	TArray<AActor*> SpotsWorldArray;
	TArray<AQuestFinishSpot*> CastedSpotsWorldArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestFinishSpot::StaticClass(), SpotsWorldArray);
	for (int i = 0; i < SpotsWorldArray.Num(); i++)
	{
		CastedSpotsWorldArray.Add(Cast<AQuestFinishSpot>(SpotsWorldArray[i]));
	}
	for (int i = 0; i < CastedSpotsWorldArray.Num(); i++)
	{
		if (FailedQuestID == CastedSpotsWorldArray[i]->QuestID)
		{
			CastedSpotsWorldArray[i]->Destroy();
			break;
		}
	}
}