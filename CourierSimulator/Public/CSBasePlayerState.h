// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CSBasePlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnStatsChangedSignature, int32, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGasolineChangeSignature, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnShowGasStationWidgetSignature, float, float);

class ACSGameModeBase;

UCLASS()
class COURIERSIMULATOR_API ACSBasePlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void PostInitializeComponents() override;
	ACSGameModeBase* GameMode;

	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	int32 Level = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	float Experience = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	float Reputation = 0.0;

	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	int32 QuestsPassed = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	int32 QuestsFailed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	float MoneyAmount = 1000.0;
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	float MoneyEarned = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	float MoneySpent = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	float Penalties = 0.0;

	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	float GasolineAmount = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	float GasolinePurchased = 0.0;

	void DecreaseMoneyAndExp(float Reward, float Exp, int32 ID);
	void IncreaseMoneyAndExp(float Reward, float Exp, int32 ID);

	void MakeOrder(float Coast);
	void CallMedicalAssistance(float MedicalPrice);

	void UpdateCarGasoline(float GasolineQty, float MoneySpent, float GasolineReceived);

	FOnStatsChangedSignature OnStatsChanged;
	FOnGasolineChangeSignature OnGasolineChange;
	FOnShowGasStationWidgetSignature OnShowGasStationWidget;
};