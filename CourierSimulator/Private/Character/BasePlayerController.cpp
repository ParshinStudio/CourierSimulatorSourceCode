// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/BasePlayerController.h"
#include "Character/BaseCharacter.h"
#include "GameFramework/SpectatorPawn.h"
#include "CSGameModeBase.h"

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<ACSGameModeBase>(GetWorld()->GetAuthGameMode());
}
void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!InputComponent) return;
	InputComponent->BindAction("ShowPhone", IE_Pressed, this, &ABasePlayerController::ShowPhone);
	InputComponent->BindAction("ShowMouse", IE_Pressed, this, &ABasePlayerController::ShowMouse);
	InputComponent->BindAction("ShowMouse", IE_Released, this, &ABasePlayerController::HideMouse);
	FInputActionBinding& toggle = InputComponent->BindAction("GamePause", IE_Pressed, this, &ABasePlayerController::OnMatchStateChanged);
	toggle.bExecuteWhenPaused = true;
}

void ABasePlayerController::OnMatchStateChanged()
{
	if (!GameMode) return;
	if (GameMode->MatchState == CSMatchState::InProgress)
	{
		GameMode->SetMatchState(CSMatchState::Pause);
		SetPause(true);
	}
	else
	{
		GameMode->SetMatchState(CSMatchState::InProgress);
		SetPause(false);
	}
}

void ABasePlayerController::ShowPhone()
{
	if (!ShowedPhone)
	{
		ShowedPhone = true;
		OnShowPhone.Broadcast(true);
		if (GameMode)
		GameMode->PushUpdateWorldTime();
	}
	else
	{
		ShowedPhone = false;
		OnShowPhone.Broadcast(false);
	}
}

void ABasePlayerController::ShowMouse()
{
	bShowMouseCursor = true;
	GetPawn()->DisableInput(this);
}
void ABasePlayerController::HideMouse()
{
	bShowMouseCursor = false;
	GetPawn()->EnableInput(this);
}

void ABasePlayerController::CreateAndPossessSpectatorPawn()
{
	TemporarySpectatorPawn = SpawnSpectatorPawn();
	if(TemporarySpectatorPawn)
	Possess(Cast<APawn>(TemporarySpectatorPawn));
}
void ABasePlayerController::DestroyTempSpectatorPawn()
{
	TemporarySpectatorPawn->Destroy();
}