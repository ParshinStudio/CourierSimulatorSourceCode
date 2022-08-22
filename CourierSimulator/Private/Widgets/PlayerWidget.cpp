// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/PlayerWidget.h"
#include "Character/BaseCharacter.h"
#include "Transport/BaseCar.h"
#include "Character/BasePlayerController.h"
#include "CSGameModeBase.h"
#include "CSBasePlayerState.h"
#include "Kismet/GameplayStatics.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const auto Player = CastChecked<ABaseCharacter>(GetOwningPlayerPawn());
	Player->OnInventoryChange.AddUObject(this, &UPlayerWidget::InitInventory);
	Player->OnInventoryBusy.AddUObject(this, &UPlayerWidget::InventoryBusy);
	Player->OnStaminaChange.AddUObject(this, &UPlayerWidget::ChangeStamina);
	Player->OnSaturationChange.AddUObject(this, &UPlayerWidget::ChangingSaturation);
	Player->OnTirednessChange.AddUObject(this, &UPlayerWidget::ChangingTiredness);
	Player->OnShowFoodOrderWidget.AddUObject(this, &UPlayerWidget::OnShowFoodWidget);
	Player->OnNotEnoughtMoney.AddUObject(this, &UPlayerWidget::OnNotEnoughtMoneyAmount);
	Player->OnMoneySpent.AddUObject(this, &UPlayerWidget::OnSpendMoneyAmount);
	Player->OnShowCarWidget.AddUObject(this, &UPlayerWidget::ShowCarWidget);

	const auto PlayerState = CastChecked<ACSBasePlayerState>(Player->GetPlayerState());
	PlayerState->OnStatsChanged.AddUObject(this, &UPlayerWidget::OnStatsChanged);
	PlayerState->OnGasolineChange.AddUObject(this, &UPlayerWidget::UpdateGasolineWidget);
	PlayerState->OnShowGasStationWidget.AddUObject(this, &UPlayerWidget::ShowGasStationWidget);

	TArray<AActor*> FoundCar;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCar::StaticClass(), FoundCar);
	PlayerCar = Cast<ABaseCar>(FoundCar[0]);
	PlayerCar->OnInventoryOpen.AddUObject(this, &UPlayerWidget::ShowCarInventory);
	PlayerCar->OnInventoryClose.AddUObject(this, &UPlayerWidget::HideCarInventory);

	const auto PlayerController = CastChecked<ABasePlayerController>(Player->GetController());
	PlayerController->OnShowPhone.AddUObject(this, &UPlayerWidget::OnShowPhone);

	const auto GameMode = Cast<ACSGameModeBase>(GetWorld()->GetAuthGameMode());
	GameMode->OnWorldTimeUpdate.AddUObject(this, &UPlayerWidget::WorldTimeUpdate);
	GameMode->OnMatchStateChanged.AddUObject(this, &UPlayerWidget::ShowGamePauseWidget);
	GameMode->OnQuestsUpdate.AddUObject(this, &UPlayerWidget::GlobalQuestsUpdate);
	GameMode->OnTimerUpdate.AddUObject(this, &UPlayerWidget::QuestsTimerUpdate);
	GameMode->OnTimerExpired.AddUObject(this, &UPlayerWidget::QuestsTimerExpired);
	GameMode->OnQuestPassed.AddUObject(this, &UPlayerWidget::QuestPassed);
}