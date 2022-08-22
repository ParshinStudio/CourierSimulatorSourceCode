// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/BaseItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/BaseCharacter.h"
#include "Components/WidgetComponent.h"
#include "Widgets/ItemIDWidget.h"
#include "Components/PointOfInterest.h"


DEFINE_LOG_CATEGORY_STATIC(LogBaseItem, All, All)

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponentItem = CreateDefaultSubobject<UStaticMeshComponent>("BoxMesh");
	SetRootComponent(StaticMeshComponentItem);
	CheckBox = CreateDefaultSubobject<UBoxComponent>("Box");
	CheckBox->SetupAttachment(StaticMeshComponentItem);
	WidgetRender = CreateDefaultSubobject<UWidgetComponent>("RenderWidget");
	WidgetRender->SetupAttachment(StaticMeshComponentItem);
	WidgetRender->SetVisibility(false);
	WidgetRender->SetRelativeLocation_Direct(FVector(0.f, 0.f, 50.f));
	WidgetRender->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetRender->SetDrawAtDesiredSize(true);
}


void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	CheckBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnCheckCollisionBeginOverlap);
	CheckBox->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnCheckCollisionEndOverlap);
	StaticMeshComponentItem->SetStaticMesh(StaticMeshItem);
}

void ABaseItem::OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;
	Player->ItemToPickup = this;
	auto Widget = CastChecked<UItemIDWidget>(WidgetRender->GetWidget());
	Widget->SetID(ItemID);
	WidgetRender->SetVisibility(true);
	UE_LOG(LogBaseItem, Display, TEXT("VariableRewrited"));
}

void ABaseItem::OnCheckCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;
	Player->ItemToPickup = nullptr;
	WidgetRender->SetVisibility(false);
	UE_LOG(LogBaseItem, Display, TEXT("VariableCleared"));
}

void ABaseItem::OnAttachToCharacter(ABaseCharacter* Character)
{
	const auto AttachComponent = Cast<USceneComponent>(Character->CharMesh);
	if (AttachComponent)
	UE_LOG(LogBaseItem, Display, TEXT("TryToAttach"));
	AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "ItemSocket0");
	SetActorEnableCollision(false);
}