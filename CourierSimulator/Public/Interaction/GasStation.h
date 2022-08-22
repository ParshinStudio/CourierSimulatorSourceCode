// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GasStation.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class USoundCue;
class UAudioComponent;
class UMaterialInstanceDynamic;

class ABaseCar;

UCLASS()
class COURIERSIMULATOR_API AGasStation : public AActor
{
	GENERATED_BODY()
	
public:	
	AGasStation();
	virtual void Tick(float DeltaTime) override;
	ABaseCar* PlayerCar;
	UMaterialInstanceDynamic* PlateMaterial;

	void PlayGasStationSound(bool StartStop);
	bool IsPlayingSound();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GasStation")
	float GasPricePerLiters = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GasStation")
	float GasAmountPerSecond = 1.0f;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneComponent* _RootComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UStaticMeshComponent* StaticMeshComponentStation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UStaticMeshComponent* StaticMeshComponentPlate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UBoxComponent* CheckBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAudioComponent* GasStationSound;

private:
	UFUNCTION()
	void OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCheckCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
