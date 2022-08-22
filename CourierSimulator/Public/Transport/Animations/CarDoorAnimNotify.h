// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CarDoorAnimNotify.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnNotifiedSignature);

UCLASS()
class COURIERSIMULATOR_API UCarDoorAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	FOnNotifiedSignature OnNotified;
};
