// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animations/OnEnterStartOpenCarDoorAN.h"

void UOnEnterStartOpenCarDoorAN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	OnNotified.Broadcast();
	Super::Notify(MeshComp, Animation);
}