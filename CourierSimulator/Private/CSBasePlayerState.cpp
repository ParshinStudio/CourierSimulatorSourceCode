// Fill out your copyright notice in the Description page of Project Settings.

#include "CSBasePlayerState.h"
#include "CSGameModeBase.h"

void ACSBasePlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GameMode = GetWorld()->GetAuthGameMode<ACSGameModeBase>();
	GameMode->OnTimerExpired.AddUObject(this, &ACSBasePlayerState::DecreaseMoneyAndExp);
	GameMode->OnQuestPassed.AddUObject(this, &ACSBasePlayerState::IncreaseMoneyAndExp);
	OnStatsChanged.Broadcast(Level, Reputation, MoneyAmount);
}

void ACSBasePlayerState::DecreaseMoneyAndExp(float Reward, float Exp, int32 ID)
{
	MoneyAmount = MoneyAmount - Reward;
	Reputation = Reputation - Exp;

	QuestsFailed++;
	Penalties = Penalties + Reward;
	Level = FMath::Floor(Experience / 1000.0f);
	OnStatsChanged.Broadcast(Level, Reputation, MoneyAmount);
	GameMode->UpdateMaxQuests(Level);
}
void ACSBasePlayerState::IncreaseMoneyAndExp(float Reward, float Exp, int32 ID)
{
	MoneyAmount = MoneyAmount + Reward;
	Experience = Experience + Exp;
	Reputation = Reputation + Exp;

	QuestsPassed++;
	MoneyEarned = MoneyEarned + Reward;
	Level = FMath::Floor(Experience / 1000.0f);
	OnStatsChanged.Broadcast(Level, Reputation, MoneyAmount);
	GameMode->UpdateMaxQuests(Level);
}

void ACSBasePlayerState::MakeOrder(float Coast)
{
	MoneyAmount = MoneyAmount - Coast;
	MoneySpent = MoneySpent + Coast;
	OnStatsChanged.Broadcast(Level, Reputation, MoneyAmount);
}
void ACSBasePlayerState::CallMedicalAssistance(float MedicalPrice)
{
	MoneyAmount = MoneyAmount - MedicalPrice;
	MoneySpent = MoneySpent + MedicalPrice;
	OnStatsChanged.Broadcast(Level, Reputation, MoneyAmount);
}

void ACSBasePlayerState::UpdateCarGasoline(float GasolineQty, float GasolinePrice, float GasolineReceived)
{
	GasolineAmount = GasolineQty;
	float WidgetScalarParameter = FMath::Lerp(0.0f, 0.0988f, GasolineAmount / 80.0f);
	OnGasolineChange.Broadcast(WidgetScalarParameter);
	if (GasolinePrice == 0.0f) return;
	MoneyAmount = MoneyAmount - GasolinePrice;
	MoneySpent = MoneySpent + GasolinePrice;
	GasolinePurchased = GasolinePurchased + GasolineReceived;
	OnStatsChanged.Broadcast(Level, Reputation, MoneyAmount);
	OnShowGasStationWidget.Broadcast(GasolinePrice, GasolineReceived);
}
