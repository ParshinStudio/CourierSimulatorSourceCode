// Fill out your copyright notice in the Description page of Project Settings.

#include "Transport/Animations/CarDoorAnimNotify.h"

void UCarDoorAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	OnNotified.Broadcast();
	Super::Notify(MeshComp, Animation);
}