// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"


UCLASS()
class COURIERSIMULATOR_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void WorldTimeUpdate(float WorldTime);
	UFUNCTION(BlueprintImplementableEvent)
	void InitInventory();
	UFUNCTION(BlueprintImplementableEvent)
	void ShowCarInventory();
	UFUNCTION(BlueprintImplementableEvent)
	void HideCarInventory();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateGasolineWidget(float WidgetScalarParameter);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowGasStationWidget(float GasolinePrice, float GasolineReceived);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowCarWidget(bool Show);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowGamePauseWidget(CSMatchState State);
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeStamina(float Var);
	UFUNCTION(BlueprintImplementableEvent)
	void ChangingSaturation(float Sat);
	UFUNCTION(BlueprintImplementableEvent)
	void ChangingTiredness(float Tir);
	UFUNCTION(BlueprintImplementableEvent)
	void InventoryBusy();
	UFUNCTION(BlueprintImplementableEvent)
	void GlobalQuestsUpdate();
	UFUNCTION(BlueprintImplementableEvent)
	void QuestsTimerUpdate();
	UFUNCTION(BlueprintImplementableEvent)
	void QuestsTimerExpired(float Reward, float Exp, int32 ID);
	UFUNCTION(BlueprintImplementableEvent)
	void QuestPassed(float Reward, float Exp, int32 ID);
	UFUNCTION(BlueprintImplementableEvent)
	void OnShowPhone(bool ShowedPhone);
	UFUNCTION(BlueprintImplementableEvent)
	void OnShowFoodWidget(bool ShowFoodWidget);
	UFUNCTION(BlueprintImplementableEvent)
	void OnNotEnoughtMoneyAmount();
	UFUNCTION(BlueprintImplementableEvent)
	void OnSpendMoneyAmount(int32 OrderItem, float Cost);
	UFUNCTION(BlueprintImplementableEvent)
	void OnStatsChanged(int32 Level, float Reputation, float MoneyAmount);
	UPROPERTY(BlueprintReadWrite, Category = "CarOfPlayer")
	class ABaseCar* PlayerCar;

protected:
	virtual void NativeConstruct() override;
};