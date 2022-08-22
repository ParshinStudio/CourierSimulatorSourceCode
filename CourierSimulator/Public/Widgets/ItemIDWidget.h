// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemIDWidget.generated.h"

UCLASS()
class COURIERSIMULATOR_API UItemIDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Info")
	int32 ID;
	void SetID(int32 ItemID);
};
