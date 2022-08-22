// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowPhoneSignature, bool);

class ACSGameModeBase;
class APawn;
class ABaseCharacter;

UCLASS()
class COURIERSIMULATOR_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void OnMatchStateChanged();

	FOnShowPhoneSignature OnShowPhone;
	void ShowPhone();
	bool ShowedPhone = false;

	void ShowMouse();
	void HideMouse();

	APawn* TemporarySpectatorPawn;
	void CreateAndPossessSpectatorPawn();
	void DestroyTempSpectatorPawn();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	ACSGameModeBase* GameMode;
};