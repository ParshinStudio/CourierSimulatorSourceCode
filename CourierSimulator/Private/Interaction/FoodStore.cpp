// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/FoodStore.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundCue.h"

#include "Kismet/GameplayStatics.h"

#include "Character/BaseCharacter.h"
#include "Interaction/FoodItem.h"

AFoodStore::AFoodStore()
{
	PrimaryActorTick.bCanEverTick = false;
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = _RootComponent;
	StaticMeshComponentMachine = CreateDefaultSubobject<UStaticMeshComponent>("Machine");
	StaticMeshComponentMachine->SetupAttachment(RootComponent);
	CheckBox = CreateDefaultSubobject<UBoxComponent>("ChBox");
	CheckBox->SetupAttachment(RootComponent);
	StartAnimBox = CreateDefaultSubobject<UBoxComponent>("Start Animation Box");
	StartAnimBox->SetupAttachment(GetRootComponent());
	StartAnimBox->SetGenerateOverlapEvents(false);
	StartAnimBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StartAnimBox->SetRelativeLocation(FVector(110.0f, 0.0f, 0.0f));
	StartAnimBox->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));
	StartAnimBox->SetRelativeRotation_Direct(FRotator(0.0f, -90.0f, 0.0f));
}
void AFoodStore::BeginPlay()
{
	Super::BeginPlay();
	CheckBox->OnComponentBeginOverlap.AddDynamic(this, &AFoodStore::OnCheckCollisionBeginOverlap);
	CheckBox->OnComponentEndOverlap.AddDynamic(this, &AFoodStore::OnCheckCollisionEndOverlap);
}

void AFoodStore::OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;
	Player->FoodStore = this;
	Player->ShowFoodStoreWidget(true);
}
void AFoodStore::OnCheckCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;
	Player->FoodStore = nullptr;
	Player->ShowFoodStoreWidget(false);
}

void AFoodStore::PlayOrderSound()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), OrderSound, GetActorLocation(), 1.0f, 1.0f, 0.0f);
}

void AFoodStore::SpawnOrderFood(int32 OrderItemID, float Cost)
{
	FActorSpawnParameters SpawnParams;
	FTransform Transform;
	Transform.SetLocation(SpawnFoodLocation);
	auto SpawnedItem = GetWorld()->SpawnActor<AFoodItem>(FoodItems[OrderItemID], Transform, SpawnParams);
	SpawnedItem->SetIDAndSaturation(OrderItemID, FoodSaturation[OrderItemID]);
}

FTransform AFoodStore::GetStartAnimBoxTransform()
{
	const auto StartAnimPostition = Cast<USceneComponent>(StartAnimBox);
	if (!StartAnimPostition) return GetActorTransform();
	return StartAnimPostition->GetComponentTransform();
}