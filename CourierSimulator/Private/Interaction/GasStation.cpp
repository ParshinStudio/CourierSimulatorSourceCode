// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/GasStation.h"
#include "Transport/BaseCar.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetStringLibrary.h"

AGasStation::AGasStation()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetTickFunctionEnable(false);
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = _RootComponent;
	StaticMeshComponentStation = CreateDefaultSubobject<UStaticMeshComponent>("Station");
	StaticMeshComponentStation->SetupAttachment(RootComponent);
	StaticMeshComponentStation->SetRelativeLocation(FVector(135.0f, -300.0f, 100.0f));
	StaticMeshComponentStation->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));
	StaticMeshComponentPlate = CreateDefaultSubobject<UStaticMeshComponent>("StationPlate");
	StaticMeshComponentPlate->SetupAttachment(RootComponent);
	StaticMeshComponentPlate->SetVisibility(true);
	StaticMeshComponentPlate->SetGenerateOverlapEvents(false);
	StaticMeshComponentPlate->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponentPlate->SetRelativeScale3D(FVector(0.95f, 0.95f, 0.95f));
	StaticMeshComponentPlate->SetRelativeRotation_Direct(FRotator(0.0f, 180.0f, 0.0f));
	CheckBox = CreateDefaultSubobject<UBoxComponent>("ChBox");
	CheckBox->SetupAttachment(RootComponent);
	CheckBox->SetBoxExtent(FVector(450.0f, 250.0f, 100.0f), true);
	CheckBox->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));

	GasStationSound = CreateDefaultSubobject<UAudioComponent>("GasStationSounds");
	GasStationSound->SetupAttachment(StaticMeshComponentStation);
	GasStationSound->bOverrideAttenuation = true;
	GasStationSound->AttenuationOverrides.FalloffDistance = 1500.0f;
	GasStationSound->bAutoActivate = false;
	GasStationSound->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
}

void AGasStation::BeginPlay()
{
	Super::BeginPlay();
	CheckBox->OnComponentBeginOverlap.AddDynamic(this, &AGasStation::OnCheckCollisionBeginOverlap);
	CheckBox->OnComponentEndOverlap.AddDynamic(this, &AGasStation::OnCheckCollisionEndOverlap);
	PlateMaterial = UMaterialInstanceDynamic::Create(StaticMeshComponentPlate->GetMaterial(0), NULL);
	StaticMeshComponentPlate->SetMaterial(0, PlateMaterial);
}

void AGasStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!PlayerCar) return;
	if (FVector::Dist(PlayerCar->GetActorLocation(), StaticMeshComponentPlate->GetComponentLocation()) < 50.0f && FMath::Abs(PlayerCar->GetActorRotation().Yaw - StaticMeshComponentPlate->GetComponentRotation().Yaw) <= 10.f)
	{
		PlayerCar->CanGetGasoline = true;
		PlateMaterial->SetScalarParameterValue(FName(TEXT("LightColor")), 1.0f);
	}
	else
	{
		PlayerCar->CanGetGasoline = false;
		PlateMaterial->SetScalarParameterValue(FName(TEXT("LightColor")), 0.0f);
	}
}


void AGasStation::OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerCar = Cast<ABaseCar>(OtherActor);
	if (!PlayerCar) return;
	PlayerCar->GasStation = this;
	PrimaryActorTick.SetTickFunctionEnable(true);
}
void AGasStation::OnCheckCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerCar = Cast<ABaseCar>(OtherActor);
	if (!PlayerCar) return;
	PlayerCar->GasStation = nullptr;
	PrimaryActorTick.SetTickFunctionEnable(false);
}

void AGasStation::PlayGasStationSound(bool StartStop)
{
	GasStationSound->SetBoolParameter(FName("PumpSwitch"), StartStop);
	GasStationSound->Play(0.0f);
}
bool AGasStation::IsPlayingSound()
{
	bool Playing = GasStationSound->IsPlaying();
	return Playing;
}