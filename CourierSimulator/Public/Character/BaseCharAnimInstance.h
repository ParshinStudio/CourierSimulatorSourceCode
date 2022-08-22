// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseCharAnimInstance.generated.h"

UCLASS()
class COURIERSIMULATOR_API UBaseCharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		float SWRot;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		FVector LeftSocketHandLoc;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		FVector RightSocketHandLoc;

};
