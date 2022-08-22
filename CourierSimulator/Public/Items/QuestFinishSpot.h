// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestFinishSpot.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class COURIERSIMULATOR_API AQuestFinishSpot : public AActor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:	
	AQuestFinishSpot();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ID")
	int32 QuestID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "StMesh")
	UStaticMeshComponent* StaticMeshComponentSpot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Checkbox")
	UBoxComponent* CheckBox;

	UFUNCTION()
	void OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
