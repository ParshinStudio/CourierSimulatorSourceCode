// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/FoodItem.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundCue.h"

#include "Character/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

AFoodItem::AFoodItem()
{
	PrimaryActorTick.bCanEverTick = false;
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = _RootComponent;
	StaticMeshComponentFood = CreateDefaultSubobject<UStaticMeshComponent>("FoodMesh");
	StaticMeshComponentFood->SetupAttachment(RootComponent);
	CheckBox = CreateDefaultSubobject<UBoxComponent>("ChBox");
	CheckBox->SetupAttachment(RootComponent);
	CheckBox->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
	StartAnimBox = CreateDefaultSubobject<UBoxComponent>("Start Animation Box");
	StartAnimBox->SetupAttachment(GetRootComponent());
	StartAnimBox->SetGenerateOverlapEvents(false);
	StartAnimBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StartAnimBox->SetRelativeLocation(FVector(0.0f, 170.0f, 0.0f));
	StartAnimBox->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));
}

void AFoodItem::BeginPlay()
{
	Super::BeginPlay();
	CheckBox->OnComponentBeginOverlap.AddDynamic(this, &AFoodItem::OnCheckCollisionBeginOverlap);
	CheckBox->OnComponentEndOverlap.AddDynamic(this, &AFoodItem::OnCheckCollisionEndOverlap);
}

void AFoodItem::OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;
	Player->FoodItem = this;
}
void AFoodItem::OnCheckCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Taken) return;
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;
	Player->FoodItem = nullptr;
}

FTransform AFoodItem::GetStartAnimBoxTransform()
{
	const auto StartAnimPostition = Cast<USceneComponent>(StartAnimBox);
	if (!StartAnimPostition) return GetActorTransform();
	return StartAnimPostition->GetComponentTransform();
}
void AFoodItem::PlayTakeSound()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), TakeSound, GetActorLocation(), 1.0f, 1.0f, 0.0f);
}
void AFoodItem::SetIDAndSaturation(int32 OrderItemID, float FoodSaturation)
{
	FoodItemID = OrderItemID;
	FoodItemSaturation = FoodSaturation;
}