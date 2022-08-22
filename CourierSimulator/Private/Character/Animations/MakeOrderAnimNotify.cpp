// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Animations/MakeOrderAnimNotify.h"

void UMakeOrderAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	OnNotified.Broadcast();
	Super::Notify(MeshComp, Animation);
}