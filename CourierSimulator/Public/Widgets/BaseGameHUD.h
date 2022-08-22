// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseGameHUD.generated.h"

class UPlayerWidget;

UCLASS()
class COURIERSIMULATOR_API ABaseGameHUD : public AHUD
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (BlueprintProtected = "true"))
	TSubclassOf<UPlayerWidget> PlayerWidgetToCreate;

	virtual void BeginPlay() override;
};