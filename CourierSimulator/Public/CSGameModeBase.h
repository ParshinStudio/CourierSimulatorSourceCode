// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CSGameModeBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWorldTimeUpdateSignature, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchStateChangedSignature, CSMatchState);
DECLARE_MULTICAST_DELEGATE(FOnQuestsUpdateSignature);
DECLARE_MULTICAST_DELEGATE(FOnQuestTimerUpdateSignature);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnQuestTimerExpiredSignature, float, float, int32);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnQuestPassedSignature, float, float, int32);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnQuestTakenItemTakenSignature, bool, bool, int32);

class AQuestStartFinishPoint;
class ABaseItem;
class AQuestFinishSpot;

UENUM(BlueprintType)
enum class EGameItemTypes : uint8 {

    Mail = 0	UMETA(DisplayName = "Mail"),
    ValuableMail = 1 	UMETA(DisplayName = "ValuableMail"),

    Parcel = 2	UMETA(DisplayName = "Parcel"),
    ValuableParcel = 3	UMETA(DisplayName = "ValuableParcel"),

    SmallPackage = 4 	UMETA(DisplayName = "SmallPackage"),
    SmallValuablePackage = 5	UMETA(DisplayName = "SmallValuablePackage"),

    Package = 6 	UMETA(DisplayName = "Package"),
    ValuablePackage = 7 	UMETA(DisplayName = "ValuablePackage")
};

USTRUCT(BlueprintType)
struct FQuestData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    FVector StartLocation;
    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    FVector EndLocation;
    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    float Reward;
    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    float TimeToFinish;
    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    EGameItemTypes ItemType;
    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    float Weight;
    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    float Exp;
    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    FString QuestName;
    UPROPERTY(BlueprintReadOnly, Category = "QuestData")
    int32 QuestID;
};

UENUM(BlueprintType)
enum class CSMatchState : uint8
{
	InProgress = 0,
	Pause = 1
};

UCLASS()
class COURIERSIMULATOR_API ACSGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACSGameModeBase();
    virtual void StartPlay() override;
    void SetPlayerReference(APawn* PlayerPawn) { PlayerReference = PlayerPawn; };
    APawn* PlayerReference;

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Quests")
    TArray<FQuestData> Quests;
    UPROPERTY(BlueprintReadOnly, Category = "Quests")
    TArray<FQuestData> CharQuests;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quests")
    TArray<TSubclassOf<ABaseItem>> SpawnItems;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quests")
    TSubclassOf<AQuestFinishSpot> FinishSpot;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quests")
    int MaxQuests = 3;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quests")
    float GenerateRate = 1.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quests")
    float UpdateRate = 1.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quests")
    TArray<float> RewardCoeficients;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quests")
    TArray<int32> QuestAmountCoeficients;


public:
    FOnMatchStateChangedSignature OnMatchStateChanged;
    CSMatchState MatchState = CSMatchState::InProgress;
    void SetMatchState(CSMatchState State);

    void GenerateQuestsLocations();
    TArray<AActor*> FoundActors;
    TArray<FVector> FoundActorsLocations;
    
    void GenerateQuest();
    void StartQuestsGenerate();
    void StopQuestsGenerate();

    void UpdateMaxQuests(int32 Level);

    void ReplaceOldQuest();
    void CreateQuestData();
    void QuestTimer();

    void SpawnItemQuest();
    void DeleteFailedQuestItem(int32 FailedQuestID);
    void SpawnFinishSpot();
    void DeleteFailedQuestSpot(int32 FailedQuestID);

    FQuestData TempQuest;
    FQuestData FailedQuest;
    FQuestData PassedQuest;
    FQuestData TakenQuest;

    FOnWorldTimeUpdateSignature OnWorldTimeUpdate;
    FTimerHandle WorldTimer;
    void UpdateWorldTime();
    void PushUpdateWorldTime();
    float WorldTime = 720.f;

    FOnQuestsUpdateSignature OnQuestsUpdate;
    FOnQuestTimerUpdateSignature OnTimerUpdate;
    FOnQuestTimerExpiredSignature OnTimerExpired;
    FOnQuestPassedSignature OnQuestPassed;

    FOnQuestTakenItemTakenSignature OnQuestTakenItemTaken;

    FTimerHandle QuestsGeneratorHandle;
    FTimerHandle QuestsUpdaterHandle;
    FTimerHandle TimerToFinish;

    UFUNCTION(BlueprintCallable, Category = "Quests")
    void TakeQuest(int QuestArrayIndex);
    void PassQuest(int QuestId);
    void OnTakeItem(int32 ID);

private:
    int QuestsAmount = 0;
};