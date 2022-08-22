// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoodStore.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class USoundCue;
class AFoodItem;

UCLASS()
class COURIERSIMULATOR_API AFoodStore : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	AFoodStore();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	USoundCue* OrderSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FVector SpawnFoodLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	TArray<TSubclassOf<AFoodItem>> FoodItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	TArray<float> FoodSaturation;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	USceneComponent* _RootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UStaticMeshComponent* StaticMeshComponentMachine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UBoxComponent* CheckBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UBoxComponent* StartAnimBox;

public:
	void SpawnOrderFood(int32 OrderItemID, float Cost);
	void PlayOrderSound();
	FTransform GetStartAnimBoxTransform();

private:
	UFUNCTION()
	void OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCheckCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
