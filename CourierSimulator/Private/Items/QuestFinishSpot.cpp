// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/QuestFinishSpot.h"
#include "Character/BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CSGameModeBase.h"

AQuestFinishSpot::AQuestFinishSpot()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponentSpot = CreateDefaultSubobject<UStaticMeshComponent>("StatMesh");
	RootComponent = StaticMeshComponentSpot;
	CheckBox = CreateDefaultSubobject<UBoxComponent>("Box");
	CheckBox->SetupAttachment(StaticMeshComponentSpot);
	CheckBox->SetRelativeScale3D(FVector(1.7f, 1.7f, 1.7f));
}
void AQuestFinishSpot::BeginPlay()
{
	Super::BeginPlay();
	CheckBox->OnComponentBeginOverlap.AddDynamic(this, &AQuestFinishSpot::OnCheckCollisionBeginOverlap);
}
void AQuestFinishSpot::OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player || Player->Inventory == nullptr) return;
	if (Player->Inventory->ItemID != QuestID) return;
	Player->Inventory = nullptr;
	auto GameMode = Cast<ACSGameModeBase>(GetWorld()->GetAuthGameMode());
	GameMode->PassQuest(QuestID);
	Destroy();
}