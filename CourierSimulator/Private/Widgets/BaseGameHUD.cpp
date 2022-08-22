// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/BaseGameHUD.h"
#include "Widgets/PlayerWidget.h"
#include "Blueprint/UserWidget.h"

void ABaseGameHUD::BeginPlay()
{
	Super::BeginPlay();
	auto CreatedWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetToCreate);
	CreatedWidget->AddToViewport();
}