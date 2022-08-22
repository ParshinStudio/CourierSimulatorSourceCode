// Fill out your copyright notice in the Description page of Project Settings.


#include "Transport/BaseCar.h"
#include "WheeledVehicleMovementComponent4W.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Components/AudioComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"

#include "Transport/BaseCarAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Transport/Animations/CarDoorAnimNotify.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

#include "Character/BaseCharacter.h"
#include "Character/BasePlayerController.h"
#include "CSGameModeBase.h"
#include "CSBasePlayerState.h"

#include "Items/BaseItem.h"
#include "Interaction/GasStation.h"

#include "Character/BaseCharAnimInstance.h"
#include "Components/BaseCharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseCar, All, All)

ABaseCar::ABaseCar()
{
	Vehicle4W = Cast<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	PrimaryActorTick.bCanEverTick = true;

	CarMesh = GetMesh();
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.23f;
	Vehicle4W->MaxNormalizedTireLoad = 2.8f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.8f;

	Vehicle4W->EngineSetup.MaxRPM = 5700.0f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 0.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(800.0f, 0.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(2700.0f, 250.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(3400.0f, 370.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5700.0f, 500.0f);

	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(20.0f, 0.9f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(60.0f, 0.8f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.7f);

	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.45f;

	Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle4W->TransmissionSetup.GearSwitchTime = 0.5f;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 600.0f;
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	SpringArmComponent->SocketOffset = (FVector(0.0f, 0.0f, 290.0f));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagMaxDistance = 100.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->FieldOfView = 80.0f;
	CameraComponent->SetRelativeRotation_Direct(FRotator(330.0f, 0.0f, 0.0f));

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Collision Mesh");
	CollisionBox->SetupAttachment(GetRootComponent());
	CollisionBox->SetRelativeScale3D(FVector(2.6f, 5.8f, 1.7f));
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));

	StartAnimBox = CreateDefaultSubobject<UBoxComponent>("Start Anim Mesh");
	StartAnimBox->SetupAttachment(GetRootComponent());
	StartAnimBox->SetGenerateOverlapEvents(false);
	StartAnimBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StartAnimBox->SetRelativeLocation(FVector(0.0f, -209.0f, 87.0f));
	StartAnimBox->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));
	StartAnimBox->SetRelativeRotation_Direct(FRotator(0.0f, 90.0f, 0.0f));

	//Sound Components
	EngineSound = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSoundComponent"));
	EngineSound->SetupAttachment(GetRootComponent());
	EngineSound->bOverrideAttenuation = true;
	EngineSound->AttenuationOverrides.FalloffDistance = 1500.0f;
	EngineSound->AttenuationOverrides.bSpatialize = false;
	EngineSound->bAutoActivate = false;
	EngineSound->SetRelativeLocation(FVector(140.f, 0.f, 70.f));

	DoorSound = CreateDefaultSubobject<UAudioComponent>(TEXT("DoorSoundComponent"));
	DoorSound->SetupAttachment(GetRootComponent());
	DoorSound->SetRelativeLocation(FVector(190.f, 0.f, 60.f));
	DoorSound->bOverrideAttenuation = true;
	DoorSound->AttenuationOverrides.FalloffDistance = 1500.0f;
	DoorSound->bAutoActivate = false;

	HonkSound = CreateDefaultSubobject<UAudioComponent>(TEXT("HonkSoundComponent"));
	HonkSound->SetupAttachment(GetRootComponent());
	HonkSound->SetRelativeLocation(FVector(-30.f, -80.f, 70.f));
	HonkSound->bOverrideAttenuation = true;
	HonkSound->AttenuationOverrides.FalloffDistance = 1500.0f;
	HonkSound->bAutoActivate = false;

	//Forward Spot Lights
	SpotLightR1 = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightR11 Component"));
	SpotLightR1->SetupAttachment(GetRootComponent());
	SpotLightR1->SetRelativeLocation(FVector(183.38f, 54.36f, 72.27f));
	SpotLightR1->SetRelativeRotation_Direct(FRotator(-10.2f, 0.0f, 0.0f));
	SpotLightR1->Intensity = 0.f;
	SpotLightR1->OuterConeAngle = 32.f;
	SpotLightR1->AttenuationRadius = 350.f;
	SpotLightR2 = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightR22 Component"));
	SpotLightR2->SetupAttachment(GetRootComponent());
	SpotLightR2->SetRelativeLocation(FVector(176.89f, 66.98f,73.44f));
	SpotLightR2->SetRelativeRotation_Direct(FRotator(-10.2f, 0.0f, 0.0f));
	SpotLightR2->Intensity = 0.f;
	SpotLightR2->OuterConeAngle = 32.f;
	SpotLightR2->AttenuationRadius = 350.f;
	SpotLightL1 = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightL11 Component"));
	SpotLightL1->SetupAttachment(GetRootComponent());
	SpotLightL1->SetRelativeLocation(FVector(183.38f,-54.36f, 72.27f));
	SpotLightL1->SetRelativeRotation_Direct(FRotator(-10.2f, 0.0f, 0.0f));
	SpotLightL1->Intensity = 0.f;
	SpotLightL1->OuterConeAngle = 32.f;
	SpotLightL1->AttenuationRadius = 350.f;
	SpotLightL2 = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightL22 Component"));
	SpotLightL2->SetupAttachment(GetRootComponent());
	SpotLightL2->SetRelativeLocation(FVector(176.89f, -66.98f, 73.44f));
	SpotLightL2->SetRelativeRotation_Direct(FRotator(-10.2f, 0.0f, 0.0f));
	SpotLightL2->Intensity = 0.f;
	SpotLightL2->OuterConeAngle = 32.f;
	SpotLightL2->AttenuationRadius = 350.f;
	
	//Forward Additional Lights
	PointLightFR1 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightFR1 Component"));
	PointLightFR1->SetupAttachment(GetRootComponent());
	PointLightFR1->SetRelativeLocation(FVector(172.30f, 69.22f, 73.60f));
	PointLightFR1->Intensity = 0.f;
	PointLightFR1->AttenuationRadius = 12.f;
	PointLightFR1->SetLightColor(FLinearColor::White);
	PointLightFR2 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightFR2 Component"));
	PointLightFR2->SetupAttachment(GetRootComponent());
	PointLightFR2->SetRelativeLocation(FVector(180.73f, 55.81f, 71.96f));
	PointLightFR2->Intensity = 0.f;
	PointLightFR2->AttenuationRadius = 12.f;
	PointLightFR2->SetLightColor(FLinearColor::White);
	PointLightFL1 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightFL1 Component"));
	PointLightFL1->SetupAttachment(GetRootComponent());
	PointLightFL1->SetRelativeLocation(FVector(180.73f, -55.81f, 71.96f));
	PointLightFL1->Intensity = 0.f;
	PointLightFL1->AttenuationRadius = 12.f;
	PointLightFL1->SetLightColor(FLinearColor::White);
	PointLightFL2 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightFL2 Component"));
	PointLightFL2->SetupAttachment(GetRootComponent());
	PointLightFL2->SetRelativeLocation(FVector(172.30f, -69.22f, 73.60f));
	PointLightFL2->Intensity = 0.f;
	PointLightFL2->AttenuationRadius = 12.f;
	PointLightFL2->SetLightColor(FLinearColor::White);

	//Back Lights 
	PointLightRR1 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightRR1 Component"));
	PointLightRR1->SetupAttachment(GetRootComponent());
	PointLightRR1->SetRelativeLocation(FVector(-209.79f, 70.27f, 84.36f));
	PointLightRR1->Intensity = 0.f;
	PointLightRR1->AttenuationRadius = 8.f;
	PointLightRR1->SetLightColor(FLinearColor::Red);
	PointLightRR2 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightRR2 Component"));
	PointLightRR2->SetupAttachment(GetRootComponent());
	PointLightRR2->SetRelativeLocation(FVector(-205.06f, 73.99f, 98.50f));
	PointLightRR2->Intensity = 0.f;
	PointLightRR2->AttenuationRadius = 10.f;
	PointLightRR2->SetLightColor(FLinearColor::Red);
	PointLightRL1 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightRL1 Component"));
	PointLightRL1->SetupAttachment(GetRootComponent());
	PointLightRL1->SetRelativeLocation(FVector(-209.79f, -70.27f, 84.36f));
	PointLightRL1->Intensity = 0.f;
	PointLightRL1->AttenuationRadius = 8.f;
	PointLightRL1->SetLightColor(FLinearColor::Red);
	PointLightRL2 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightRL2 Component"));
	PointLightRL2->SetupAttachment(GetRootComponent());
	PointLightRL2->SetRelativeLocation(FVector(-205.06f, -73.99f, 98.50f));
	PointLightRL2->Intensity = 0.f;
	PointLightRL2->AttenuationRadius = 10.f;
	PointLightRL2->SetLightColor(FLinearColor::Red);
}

void ABaseCar::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCar::OnBoxOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCar::OnBoxOverlapEnd);
	const auto GameMode = Cast<ACSGameModeBase>(GetWorld()->GetAuthGameMode());
	GameMode->OnTimerExpired.AddUObject(this, &ABaseCar::DeleteFailedQuestItem);
	CarAnimInstance = Cast<UBaseCarAnimInstance>(CarMesh->GetAnimInstance());
	InitSockets();
	InitAnimations();
	SwitchLights(ELightsMode::Off);
	GetWorldTimerManager().SetTimer(OnGasolineDecrease, this, &ABaseCar::GasolineDecrease, 1.0f, true, 1.0f);
}

void ABaseCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Engine Sound Control
	float ABSSpeed = FMath::Abs(Vehicle4W->GetForwardSpeed());
	EngineSound->SetFloatParameter(FName("RPM"), ABSSpeed);
	EngineSound->SetPitchMultiplier((ABSSpeed / 10000.f + 0.2) * 1.4f);

	if (!CarAnimInstance || !CharToPossess || !CharToPossess->InCarNow || !CharToPossess->CharAnimInstance) return;
	//Steering Wheel Position Control
	float Angle0 = Vehicle4W->Wheels[0]->GetSteerAngle();
	float Angle1 = Vehicle4W->Wheels[1]->GetSteerAngle();
	if (FMath::Abs(Angle0) > FMath::Abs(Angle1))
	{
		CarAnimInstance->SWRot = Angle0 * -2.0f;
		CharToPossess->CharAnimInstance->SWRot = Angle0 * 2.0f;
	}
	else
	{
		CarAnimInstance->SWRot = Angle1 * -2.0f;
		CharToPossess->CharAnimInstance->SWRot = Angle1 * 2.0f;
	}
}

void ABaseCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCar::ApplyThrottle);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCar::ApplySteering);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCar::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseCar::Turn);

	PlayerInputComponent->BindAction("HandBrake", IE_Pressed, this, &ABaseCar::OnHandbrakePressed);
	PlayerInputComponent->BindAction("HandBrake", IE_Released, this, &ABaseCar::OnHandbrakeReleased);

	PlayerInputComponent->BindAction("GetCar", IE_Pressed, this, &ABaseCar::LeaveCar);

	PlayerInputComponent->BindAction("UseHonk", IE_Pressed, this, &ABaseCar::PlayHonkSound);
	PlayerInputComponent->BindAction("SwitchLights", IE_Pressed, this, &ABaseCar::CharSwitchLights);

	PlayerInputComponent->BindAction("GetGasoline", IE_Pressed, this, &ABaseCar::TryGetGasoline);
	PlayerInputComponent->BindAction("GetGasoline", IE_Released, this, &ABaseCar::StopGetGasoline);
}

void ABaseCar::InitAnimations()
{
	if (!OpeningDoorMontage) return;
	const auto OpenDoorEvents = OpeningDoorMontage->Notifies;
	for (auto OpenDoorEvent : OpenDoorEvents)
	{
		auto OpenDoorNotify = Cast<UCarDoorAnimNotify>(OpenDoorEvent.Notify);
		if (OpenDoorNotify)
		{
			OpenDoorNotify->OnNotified.AddUObject(this, &ABaseCar::PlayDoorSound);
			break;
		}
	}
}

void ABaseCar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (CharToPossess)
	{
		SwitchLights(ELightsMode::Day);
		CharPlayerState = Cast<ACSBasePlayerState>(GetPlayerState());
		EnableInput(Cast<ABasePlayerController>(NewController));
		if (GasolineAmount > 0.0f)
		{
			EngineSound->Play(0.0f);
		}
	}
}
void ABaseCar::UnPossessed()
{
	Super::UnPossessed();
	EngineSound->Stop();
	SwitchLights(ELightsMode::Off);
	SwitchBreakLights(false);
}

// MOVEMENT FUNCTIONS
void ABaseCar::ApplyThrottle(float Val)
{
	if (GasolineAmount > 0.0f)
	{
		GetVehicleMovementComponent()->SetThrottleInput(Val);
		if (Val < 0.f)
		{
			SwitchBreakLights(true);
		}
		else
		{
			SwitchBreakLights(false);
		}
	}
	else
	{
		GetVehicleMovementComponent()->SetHandbrakeInput(true);
		if(EngineSound->IsPlaying())
		EngineSound->Stop ();
		// Add Stop Engine sound
	}	
}
void ABaseCar::ApplySteering(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}
void ABaseCar::LookUp(float Val)
{
	if (Val != 0.f)
	{
		AddControllerPitchInput(Val);
	}
}
void ABaseCar::Turn(float Val)
{
	if (Val != 0.f)
	{
		AddControllerYawInput(Val);
	}
}
void ABaseCar::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}
void ABaseCar::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

// OVERLAP AND TRANSFORM FUNCTIONS
void ABaseCar::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;
	Player->CanSitInCar = true;
	Player->CarToPossess = this;
	OnInventoryOpen.Broadcast();
}
void ABaseCar::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const auto Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player) return;
	Player->CanSitInCar = false;
	OnInventoryClose.Broadcast();
}
FTransform ABaseCar::GetStartAnimBoxTransform()
{
	const auto StartAnimPostition = Cast<USceneComponent>(StartAnimBox);
	if (!StartAnimPostition) return GetActorTransform();
	return StartAnimPostition->GetComponentTransform();
}

// LEAVE CAR FUNCTIONS
void ABaseCar::LeaveCar()
{
	auto Velocity = GetVehicleMovementComponent()->GetForwardSpeed();
	UE_LOG(LogTemp, Warning, TEXT("%f"), Velocity);
	if (Velocity <= 1.0f)
	{
		DisableInput(Cast<ABasePlayerController>(GetController()));
		GetVehicleMovementComponent()->SetHandbrakeInput(true);
		if (!CharToPossess) return;
		CharToPossess->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		CharToPossess->PlayAnimMontage(CharToPossess->AnimLeaveCar);
		CharToPossess->PlayingMontage = true;
		CharToPossess->InCarNow = false;
		Cast<ABasePlayerController>(GetController())->SetViewTargetWithBlend(CharToPossess, 5.79, EViewTargetBlendFunction::VTBlend_Cubic);
		// Need to disable wheel movement
	}
}
void ABaseCar::LeaveCarFinished()
{
	if (!CharToPossess) return;
	CharToPossess->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CharToPossess->SetActorLocation(StartAnimBox->GetComponentLocation());
	CharToPossess->SetActorEnableCollision(true);
	CharToPossess->GetCharacterMovement()->SetDefaultMovementMode();
	CharToPossess->PlayingMontage = false;
	GetController()->Possess(CastChecked<APawn>(CharToPossess));
	OnInventoryOpen.Broadcast();
}

// QUEST ITEMS AND SOCKETS FUNCTIONS
void ABaseCar::InitSockets()
{
	for (int i = 0; i <= 3; i++)
	{
		FString Name = "ItemSocket" + FString::FromInt(i);
		CarSocketData.SocketName = FName(*Name);
		CarSocketData.SocketBusy = false;
		CarSocketsData.Add(CarSocketData);
	}
}
FName ABaseCar::FindEmptySocket()
{
	for (int i = 0; i < CarSocketsData.Num(); i++)
	{
		if (CarSocketsData[i].SocketBusy == false)
		{
			CarSocketsData[i].SocketBusy = true;
			return FName(CarSocketsData[i].SocketName);
		}
	}
	return FName("Trunk");
	// ONLY 5 QUESTS AVAILABLE WITH THIS FUNCTION
}
void ABaseCar::DeleteFailedQuestItem(float Reward, float Exp, int32 ID)
{
	for (int i = 0; i < CarInventory.Num(); i++)
	{
		if (CarInventory[i]->ItemID == ID)
			CarInventory.RemoveAt(i);
	}
}
void ABaseCar::ClearSocket(const FName & SocketName)
{
	FString ToString1 = SocketName.ToString();
	for (int i = 0; i < CarSocketsData.Num(); i++)
	{
		FString ToString2 = CarSocketsData[i].SocketName.ToString();
		if (ToString1 == ToString2)
		CarSocketsData[i].SocketBusy = false;
	}
}

// CAR SPECIFIED FUNCTIONS
void ABaseCar::PlayHonkSound()
{
	if(!HonkSound->IsPlaying())
	HonkSound->Play(0.0f);
}
void ABaseCar::OpenDoor(bool DoorOpen)
{
	IsDoorOpened = DoorOpen;
	if (!CarAnimInstance) return;
	(DoorOpen) ? (CarAnimInstance->Montage_Play(OpeningDoorMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true)) : 
	(CarAnimInstance->Montage_Play(OpeningDoorMontage, -1.0f, EMontagePlayReturnType::MontageLength, OpeningDoorMontage->GetPlayLength(), true));

}
void ABaseCar::PlayDoorSound()
{
	DoorSound->SetBoolParameter(FName("DoorOpen"), IsDoorOpened);
	DoorSound->Play();
}
void ABaseCar::CharSwitchLights()
{
	switch (LightsState)
	{
	case ELightsMode::Day:
		SwitchLights(ELightsMode::Night);
		break;
	case ELightsMode::Night:
		SwitchLights(ELightsMode::Day);
		break;
	case ELightsMode::Off:
		SwitchLights(ELightsMode::Day);
		break;
	}
}
void ABaseCar::SwitchLights(ELightsMode Mode)
{
	LightsState = Mode;
	switch (Mode)
	{
	case ELightsMode::Day:

		SpotLightR1->SetIntensity(0.f);
		SpotLightR2->SetIntensity(0.f);
		SpotLightL1->SetIntensity(0.f);
		SpotLightL2->SetIntensity(0.f);
		PointLightFR1->SetIntensity(5000.f);
		PointLightFR2->SetIntensity(0.f);
		PointLightFL1->SetIntensity(0.f);
		PointLightFL2->SetIntensity(5000.f);
		PointLightRR1->SetIntensity(3000.f);
		PointLightRL1->SetIntensity(3000.f);
		break;

	case ELightsMode::Night:
		
		SpotLightR1->SetIntensity(90000.f);
		SpotLightR2->SetIntensity(90000.f);
		SpotLightL1->SetIntensity(90000.f);
		SpotLightL2->SetIntensity(90000.f);
		PointLightFR1->SetIntensity(5000.f);
		PointLightFR2->SetIntensity(5000.f);
		PointLightFL1->SetIntensity(5000.f);
		PointLightFL2->SetIntensity(5000.f);
		PointLightRR1->SetIntensity(3000.f);
		PointLightRL1->SetIntensity(3000.f);
		break;

	case ELightsMode::Off:

		SpotLightR1->SetIntensity(0.f);
		SpotLightR2->SetIntensity(0.f);
		SpotLightL1->SetIntensity(0.f);
		SpotLightL2->SetIntensity(0.f);
		PointLightFR1->SetIntensity(0.f);
		PointLightFR2->SetIntensity(0.f);
		PointLightFL1->SetIntensity(0.f);
		PointLightFL2->SetIntensity(0.f);
		PointLightRR1->SetIntensity(0.f);
		PointLightRL1->SetIntensity(0.f);
		break;
	}
}
void ABaseCar::SwitchBreakLights(bool BrakeLights)
{
	if (BrakeLights)
	{
		PointLightRR2->SetIntensity(3000.f);
		PointLightRL2->SetIntensity(3000.f);
	}
	else
	{
		PointLightRR2->SetIntensity(0.f);
		PointLightRL2->SetIntensity(0.f);
	}
}
// GASOLINE DECREASE, INTERACTION WITH GAS STATION
void ABaseCar::GasolineDecrease()
{
	if (!CharToPossess || !CharToPossess->InCarNow || GasolineAmount == 0.0f) return;
	GasolineAmount = GasolineAmount - (1.0f + FMath::Clamp(Vehicle4W->GetForwardSpeed() * 0.036f / 10.0f, 0.0f, 10.0f));
	GasolineAmount = FMath::Clamp(GasolineAmount, 0.0f, MaxGasolineAmount);
	CharPlayerState->UpdateCarGasoline(GasolineAmount, 0.0f, 0.0f);
}
void ABaseCar::TryGetGasoline()
{
	if (!GasStation || !CanGetGasoline || GetVehicleMovementComponent()->GetForwardSpeed() >= 1.0f) return;
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
	if (GetWorldTimerManager().IsTimerActive(OnGasolineIncrease)) return;
	GetWorldTimerManager().SetTimer(OnGasolineIncrease, this, &ABaseCar::GasolineIncrease, 1.0f, true, 0.3f);
	UE_LOG(LogTemp, Warning, TEXT("TryGetGasoline()"));
}
void ABaseCar::StopGetGasoline()
{
	if (!GasStation || !CanGetGasoline || GetVehicleMovementComponent()->GetForwardSpeed() >= 1.0f) return;
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
	GetWorldTimerManager().ClearTimer(OnGasolineIncrease);
	GasStation->PlayGasStationSound(false);
	UE_LOG(LogTemp, Warning, TEXT("StopGetGasoline()"));
	if (GasolineReceived == 0.0f && GasolineMoneySpent == 0.0f) return;
	CharPlayerState->UpdateCarGasoline(GasolineAmount, GasolineMoneySpent, GasolineReceived);
	GasolineMoneySpent = 0.0f;
	GasolineReceived = 0.0f;
}
void ABaseCar::GasolineIncrease()
{
	if (GasolineAmount < MaxGasolineAmount - 10.0f && CharPlayerState && CharPlayerState->MoneyAmount - GasolineMoneySpent >= 10.0f)
	{
		auto PrevGasolineAmount = GasolineAmount;
		GasolineAmount = FMath::Clamp(GasolineAmount + 10.0f, 0.0f, MaxGasolineAmount);
		GasolineReceived = GasolineReceived + (GasolineAmount - PrevGasolineAmount);
		GasolineMoneySpent = GasolineMoneySpent + 10.0f;
		CharPlayerState->UpdateCarGasoline(GasolineAmount, 0.0f, 0.0f);
		if (GasStation->IsPlayingSound()) return;
		GasStation->PlayGasStationSound(true);
		UE_LOG(LogTemp, Warning, TEXT("GasolineIncrease()"));
	}
	else 
	{
		StopGetGasoline();
		UE_LOG(LogTemp, Warning, TEXT("StopGetGasolineInIncrease()"));
	}
}