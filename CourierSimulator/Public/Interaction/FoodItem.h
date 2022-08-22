// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoodItem.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UStaticMesh;
class USoundCue;

UCLASS()
class COURIERSIMULATOR_API AFoodItem : public AActor
{
	GENERATED_BODY()
	
public:
	AFoodItem();

	void SetIDAndSaturation(int32 OrderItemID, float FoodSaturation);
	int32 FoodItemID;
	float FoodItemSaturation;
	bool Taken = false;

	void PlayTakeSound();
	FTransform GetStartAnimBoxTransform();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	USceneComponent* _RootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UStaticMeshComponent* StaticMeshComponentFood;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UBoxComponent* CheckBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UBoxComponent* StartAnimBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Info")
	USoundCue* TakeSound;

private:
	UFUNCTION()
	void OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCheckCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};