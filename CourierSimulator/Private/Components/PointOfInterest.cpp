// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/PointOfInterest.h"
#include "CSGameModeBase.h"

UPointOfInterest::UPointOfInterest()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UPointOfInterest::BeginPlay()
{
	const auto GameMode = Cast<ACSGameModeBase>(GetWorld()->GetAuthGameMode());
	GameMode->OnQuestTakenItemTaken.AddUObject(this, &UPointOfInterest::TakeItem);
	Super::BeginPlay();
}