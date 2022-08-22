// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/BaseCharacter.h"
#include "BaseItem.generated.h"

class USoundCue;
class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class UPointOfInterest;

UCLASS()
class COURIERSIMULATOR_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	

	ABaseItem();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UStaticMesh* StaticMeshItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	USoundCue* PickupSound;

	UPROPERTY(BlueprintReadOnly, Category = "Info")
	int32 ItemID;
	UPROPERTY(BlueprintReadOnly, Category = "Info")
	int32 ItemType;
	UPROPERTY(BlueprintReadOnly, Category = "Info")
	FName Socket;

	UFUNCTION()
	void OnAttachToCharacter(ABaseCharacter* Character);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "StaticMeshComponent")
	UStaticMeshComponent* StaticMeshComponentItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Checkbox")
	UBoxComponent* CheckBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TextRender")
	UWidgetComponent* WidgetRender;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POIComponent")
	TSubclassOf<UPointOfInterest> POIClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POIComponent")
	TSubclassOf<UPointOfInterest> POI;


	UFUNCTION()
	void OnCheckCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCheckCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};