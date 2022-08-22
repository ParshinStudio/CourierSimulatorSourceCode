// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animations/OnEnterEndOpenCarDoorAN.h"

void UOnEnterEndOpenCarDoorAN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	OnNotified.Broadcast();
	Super::Notify(MeshComp, Animation);
}