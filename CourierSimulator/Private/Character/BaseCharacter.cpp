// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "CSBasePlayerState.h"
#include "CSGameModeBase.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BaseCharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

#include "TimerManager.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Character/BasePlayerController.h"
#include "Character/AIControllerBaseCharacter.h"

#include "Transport/BaseCar.h"
#include "Items/BaseItem.h"

#include "Interaction/FoodStore.h"
#include "Interaction/FoodItem.h"

#include "Character/Animations/EnterCarAnimNotify.h"
#include "Character/Animations/LeaveCarAnimNotify.h"
#include "Character/Animations/MakeOrderAnimNotify.h"
#include "Character/Animations/OnEnterEndOpenCarDoorAN.h"
#include "Character/Animations/OnEnterStartOpenCarDoorAN.h"
#include "Character/Animations/FoodTakeAnimNotify.h"
#include "Character/Animations/FoodConsumptedAnimNotify.h"

#include "Character/BaseCharAnimInstance.h"

#include "Widgets/BaseGameHUD.h"
#include "Widgets/PlayerWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseChar, All, All)

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjInit) :Super(ObjInit.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CharMesh = GetMesh();
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MaxAcceleration = 700.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.3f;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	CameraCollisionComponent = CreateDefaultSubobject<USphereComponent>("CameraCollisionComponent");
	CameraCollisionComponent->SetupAttachment(CameraComponent);
	CameraCollisionComponent->SetSphereRadius(10.0f);
	CameraCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::OnCameraCollisionBeginOverlap);
	CameraCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::OnCameraCollisionEndOverlap);
	CharAnimInstance = Cast<UBaseCharAnimInstance>(GetMesh()->GetAnimInstance());
	InitAnimations();
	OnStaminaChange.Broadcast(Stamina / MaxStamina);
	OnSaturationChange.Broadcast(Saturation / MaxSaturation);
	OnTirednessChange.Broadcast(Tiredness / MaxTiredness);
	GetWorldTimerManager().SetTimer(SaturationHandleDecrease, this, &ABaseCharacter::SaturationDecrease, 5.0f, true, 5.0f);
	GetWorldTimerManager().SetTimer(TirednessHandleDecrease, this, &ABaseCharacter::TirednessDecrease, 5.0f, true, 5.0f);
	const auto TemporaryController = CastChecked<ABasePlayerController>(GetController());
	TemporaryController->OnShowPhone.AddUObject(this, &ABaseCharacter::ChangePhoneState);
	GameMode = Cast<ACSGameModeBase>(GetWorld()->GetAuthGameMode());
	GameMode->OnTimerExpired.AddUObject(this, &ABaseCharacter::DeleteFailedQuestItem);
	GameMode->OnQuestPassed.AddUObject(this, &ABaseCharacter::ChangeCarryingBoxStateOnPassQuest);
	GameMode->SetPlayerReference(Cast<APawn>(this));
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABaseCharacter::Jumping);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ABaseCharacter::OnStartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ABaseCharacter::OnStopRunning);

	PlayerInputComponent->BindAction("GetCar", IE_Pressed, this, &ABaseCharacter::OnGetCar);
	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &ABaseCharacter::OnGetItem);
	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &ABaseCharacter::OnPutInCarItem);
	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &ABaseCharacter::UseFoodItem);
}

// Receive car sockets steering wheel locations for AnimBP
FVector ABaseCharacter::GetLeftSocketLoc()
{
	if (!CarToPossess || !CarToPossess->CarMesh) return FVector(0.0f, 0.0f, 0.0f);
	return CarToPossess->CarMesh->GetSocketLocation(FName("LeftHandSocket"));
}
FVector ABaseCharacter::GetRightSocketLoc()
{
	if (!CarToPossess || !CarToPossess->CarMesh) return FVector(0.0f, 0.0f, 0.0f);
	return CarToPossess->CarMesh->GetSocketLocation(FName("RightHandSocket"));
}

// INITIALIZE AND SIGN FOR ANIMATIONS NOTIFIES
void ABaseCharacter::InitAnimations()
{
	// EnterCarAnims
	if (!AnimEnterCar) return;
	const auto EnterCarEvents = AnimEnterCar->Notifies;
	for (auto EnterCarEvent : EnterCarEvents)
	{
		auto EnterFinished = Cast<UEnterCarAnimNotify>(EnterCarEvent.Notify);
		if (EnterFinished)
		{
			EnterFinished->OnNotified.AddUObject(this, &ABaseCharacter::OnGetCarFinished);
			break;
		}
	}
	for (auto EnterCarEvent : EnterCarEvents)
	{
		auto OnEnterStartDoorOpening = Cast<UOnEnterStartOpenCarDoorAN>(EnterCarEvent.Notify);
		if (OnEnterStartDoorOpening)
		{
			OnEnterStartDoorOpening->OnNotified.AddUObject(this, &ABaseCharacter::OnEnteringStartCarDoorOpening);
			break;
		}
	}
	for (auto EnterCarEvent : EnterCarEvents)
	{
		auto OnEnterStartDoorClosing = Cast<UOnEnterEndOpenCarDoorAN>(EnterCarEvent.Notify);
		if (OnEnterStartDoorClosing)
		{
			OnEnterStartDoorClosing->OnNotified.AddUObject(this, &ABaseCharacter::OnEnteringEndCarDoorClosing);
			break;
		}
	}

	// LeaveCarAnims
	if (!AnimLeaveCar) return;
	const auto LeaveCarEvents = AnimLeaveCar->Notifies;
	for (auto LeaveCarEvent : LeaveCarEvents)
	{
		auto LeaveFinished = Cast<ULeaveCarAnimNotify>(LeaveCarEvent.Notify);
		if (LeaveFinished)
		{
			LeaveFinished->OnNotified.AddUObject(this, &ABaseCharacter::LeaveCarFinished);
			break;
		}
	}
	for (auto LeaveCarEvent : LeaveCarEvents)
	{
		auto OnLeaveStartDoorOpening = Cast<UOnEnterStartOpenCarDoorAN>(LeaveCarEvent.Notify);
		if (OnLeaveStartDoorOpening)
		{
			OnLeaveStartDoorOpening->OnNotified.AddUObject(this, &ABaseCharacter::OnEnteringStartCarDoorOpening);
			break;
		}
	}
	for (auto LeaveCarEvent : LeaveCarEvents)
	{
		auto OnLeaveEndDoorOpening = Cast<UOnEnterEndOpenCarDoorAN>(LeaveCarEvent.Notify);
		if (OnLeaveEndDoorOpening)
		{
			OnLeaveEndDoorOpening->OnNotified.AddUObject(this, &ABaseCharacter::OnEnteringEndCarDoorClosing);
			break;
		}
	}

	// InteractionAnims
	if (!MakeFoodOrder) return;
	const auto MakeOrderEvents = MakeFoodOrder->Notifies;
	for (auto MakeOrderEvent : MakeOrderEvents)
	{
		auto OrderFinished = Cast<UMakeOrderAnimNotify>(MakeOrderEvent.Notify);
		if (OrderFinished)
		{
			OrderFinished->OnNotified.AddUObject(this, &ABaseCharacter::OnFinishOrderFood);
			break;
		}
	}

	if (!AnimTakeFood) return;
	const auto TakeFoodEvents = AnimTakeFood->Notifies;
	for (auto TakeFoodEvent : TakeFoodEvents)
	{
		auto OnFoodTake = Cast<UFoodTakeAnimNotify>(TakeFoodEvent.Notify);
		if (OnFoodTake)
		{
			OnFoodTake->OnNotified.AddUObject(this, &ABaseCharacter::AttachFoodItem);
			break;
		}
	}
	for (auto TakeFoodEvent : TakeFoodEvents)
	{
		auto OnFoodConsumpted = Cast<UFoodConsumptedAnimNotify>(TakeFoodEvent.Notify);
		if (OnFoodConsumpted)
		{
			OnFoodConsumpted->OnNotified.AddUObject(this, &ABaseCharacter::FinishUseFoodItem);
			break;
		}
	}

}

// CHANGE CHARACTER STATES FUNCTIONS
void ABaseCharacter::ChangePhoneState(bool PhoneState)
{
	ShowedPhone = PhoneState;
	if (PhoneState)
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	else if (!PhoneState)
	GetCharacterMovement()->MaxWalkSpeed = 375.f;
}
void ABaseCharacter::ChangeCarryingBoxStateOnPassQuest(float Reward, float Exp, int32 ID) 
{
	CarryingBox = false;
}

//MOVEMENT FUNCTIONS
void ABaseCharacter::MoveForward(float Amount)
{
	if (Amount == 0.0f) return;
	IsMoving = Amount > 0.0f;
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Amount);
}
void ABaseCharacter::MoveRight(float Amount)
{
	if (IsSprinting) return;
	IsMovingRight = Amount != 0.0f;
	if (Amount == 0.0f)	return;
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Amount);
}
void ABaseCharacter::OnStartRunning()
{
	IsSprinting = true;
	if (IsRunning())
	GetWorldTimerManager().SetTimer(StaminaHandleDecrease, this, &ABaseCharacter::StaminaDecrease, 1.0f, true, 0.1f);
	GetWorldTimerManager().ClearTimer(StaminaHandleIncrease);
}
void ABaseCharacter::OnStopRunning()
{
	GetWorldTimerManager().ClearTimer(StaminaHandleDecrease);
	GetWorldTimerManager().SetTimer(StaminaHandleIncrease, this, &ABaseCharacter::StaminaIncrease, 1.0f, true, 1.0f);
	IsSprinting = false;
}
bool ABaseCharacter::IsRunning() const
{
	return !IsMovingRight && IsMoving && !GetVelocity().IsZero() && Stamina > 0.0f && IsSprinting && !ShowedPhone && !CarryingBox;
}
void ABaseCharacter::Jumping()
{
	if (!CarryingBox || !GetCharacterMovement()->IsFalling())
	Jump();
}

// CHANGE STAMINA
void ABaseCharacter::StaminaDecrease()
{
	if (Stamina > 0.0f)
	Stamina = Stamina - (1.0f * StaminaModifier);
	OnStaminaChange.Broadcast(Stamina / MaxStamina);
}
void ABaseCharacter::StaminaIncrease()
{
	if (Stamina < MaxStamina)
	Stamina = Stamina + 1.0f;
	OnStaminaChange.Broadcast(Stamina / MaxStamina);
	if(Stamina >= MaxStamina)
	GetWorldTimerManager().ClearTimer(StaminaHandleIncrease);
}

// CAMERA COLLISION HIDES CHAR MESH FUNCTIONS
void ABaseCharacter::OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckCameraOverlap();
}
void ABaseCharacter::OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckCameraOverlap();
}
void ABaseCharacter::CheckCameraOverlap()
{
	const auto HideMesh = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
	GetMesh()->SetOwnerNoSee(HideMesh);

	TArray<USceneComponent*> MeshChildren;
	GetMesh()->GetChildrenComponents(true, MeshChildren);
	for (auto MeshChild : MeshChildren)
	{
		const auto MeshChildGeometry = Cast<UPrimitiveComponent>(MeshChild);
		if (MeshChildGeometry)
		{
			MeshChildGeometry->SetOwnerNoSee(HideMesh);
		}
	}
}

// OPERATIONS WITH CAR
void ABaseCharacter::OnGetCar()
{
	if (!CarToPossess || !CanSitInCar) return;
	if (Inventory)
	{
		OnPutInCarItem();
	}
	InteractionType = EInteractionType::Car;
	AnimBoxTransform = CarToPossess->GetStartAnimBoxTransform();
	PossessAIController(AnimBoxTransform.GetLocation());
}
void ABaseCharacter::OnGetCarContinue()
{
	CarToPossess->CharToPossess = this;
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
	SetActorLocationAndRotation(AnimBoxTransform.GetLocation(), AnimBoxTransform.GetRotation());
	PlayingMontage = true;
	PlayAnimMontage(AnimEnterCar);
	CharController->SetViewTargetWithBlend(CarToPossess, 4.0, EViewTargetBlendFunction::VTBlend_Cubic);
}
void ABaseCharacter::OnGetCarFinished()
{
	InCarNow = true;
	PlayingMontage = false;
	CharController->Possess(Cast<APawn>(CarToPossess));
	const auto AttachComponent = Cast<USceneComponent>(CarToPossess->CarMesh);
	if (AttachComponent)
	AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "DriverSocket");
	OnShowCarWidget.Broadcast(true);
	SpringArmComponent->bUsePawnControlRotation = true;
}
void ABaseCharacter::LeaveCarFinished()
{
	CarToPossess->LeaveCarFinished();
	OnShowCarWidget.Broadcast(false);
}
void ABaseCharacter::OnEnteringStartCarDoorOpening()
{
	CarToPossess->OpenDoor(true);
}
void ABaseCharacter::OnEnteringEndCarDoorClosing()
{
	CarToPossess->OpenDoor(false);
}

//	OPERATIONS WITH QUEST ITEMS
void ABaseCharacter::OnGetItem()
{
	if (ItemToPickup && !Inventory)
	{
		Inventory = ItemToPickup;
		ItemToPickup->OnAttachToCharacter(this);
		OnInventoryChange.Broadcast();
		const auto GameModeBase = Cast<ACSGameModeBase>(GetWorld()->GetAuthGameMode());
		GameModeBase->OnTakeItem(Inventory->ItemID);
		if(Inventory->ItemType > 1)
		CarryingBox = true;
	}
	else if (Inventory && ItemToPickup)
	{
		OnInventoryBusy.Broadcast();
	}
}
void ABaseCharacter::OnPutInCarItem()
{
	if (!Inventory || !CarToPossess || !CanSitInCar) return;
	CarToPossess->CarInventory.Add(Inventory);
	Inventory->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	const auto AttachComponent = Cast<USceneComponent>(CarToPossess->CarMesh);
	if (AttachComponent)
	Inventory->Socket = CarToPossess->FindEmptySocket();
	Inventory->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Inventory->Socket);
	Inventory = nullptr;
	OnInventoryChange.Broadcast();
	CarryingBox = false;
}
void ABaseCharacter::OnTakeFromCarItem(int Index)
{
	if (!CarToPossess || !CanSitInCar) return;
	if (Inventory)
	{
		OnInventoryBusy.Broadcast();
	}
	else
	{
		CarToPossess->CarInventory[Index]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		const auto AttachComponent = Cast<USceneComponent>(CharMesh);
		if (AttachComponent)
		CarToPossess->CarInventory[Index]->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "ItemSocket0");
		Inventory = CarToPossess->CarInventory[Index];
		CarToPossess->ClearSocket(Inventory->Socket);
		CarToPossess->CarInventory.RemoveSingle(Inventory);
		OnInventoryChange.Broadcast();
		if (Inventory->ItemType > 1)
		CarryingBox = true;
	}	
}
void ABaseCharacter::DeleteFailedQuestItem(float Reward, float Exp, int32 ID)
{
	if (!Inventory || Inventory->ItemID != ID) return;
	Inventory = nullptr;
	if(CarryingBox)
	CarryingBox = false;
}

// OPERATIONS WITH FOOD STORE
void ABaseCharacter::OnOrderFood(int32 FoodToOrder, float Cost)
{
	if (FoodStore && !Inventory)
	{
		auto CharPawn = Cast<APawn>(this);
		auto CharPlayerState = Cast<ACSBasePlayerState>(CharPawn->GetPlayerState());
		if (CharPlayerState && Cost <= CharPlayerState->MoneyAmount)
		{
			Cast<ABasePlayerController>(GetController())->HideMouse();
			InteractionType = EInteractionType::FoodStore;
			AnimBoxTransform = FoodStore->GetStartAnimBoxTransform();
			PossessAIController(AnimBoxTransform.GetLocation());
			OnShowFoodOrderWidget.Broadcast(false);
			TempFoodOrder = FoodToOrder;
			TempFoodCost = Cost;
		}
		else if (CharPlayerState && Cost > CharPlayerState->MoneyAmount)
		{
			OnNotEnoughtMoney.Broadcast();
			OnShowFoodOrderWidget.Broadcast(false);
			FoodStore = nullptr;
		}
	}
	else if (Inventory && FoodStore)
	{
		OnInventoryBusy.Broadcast();
		OnShowFoodOrderWidget.Broadcast(false);
		FoodStore = nullptr;
	}
}
void ABaseCharacter::OnOrderFoodContinue()
{
	GetCharacterMovement()->DisableMovement();
	SetActorLocationAndRotation(AnimBoxTransform.GetLocation(), AnimBoxTransform.GetRotation());
	PlayAnimMontage(MakeFoodOrder);
	PlayingMontage = true;
	//TempRot = CharController->GetPawn()->GetViewRotation();
}
void ABaseCharacter::OnFinishOrderFood()
{
	auto CharPlayerState = Cast<ACSBasePlayerState>(CharacterSelf->GetPlayerState());
	if(CharacterSelf && CharPlayerState)
	PlayingMontage = false;
	FoodStore->PlayOrderSound();
	FoodStore->SpawnOrderFood(TempFoodOrder, TempFoodCost);
	CharPlayerState->MakeOrder(TempFoodCost);
	OnMoneySpent.Broadcast(TempFoodOrder, TempFoodCost);
	FoodStore = nullptr;
	SpringArmComponent->bUsePawnControlRotation = true;
	GetCharacterMovement()->SetDefaultMovementMode();
	SpringArmComponent->SetWorldRotation(TempRot);
}
void ABaseCharacter::ShowFoodStoreWidget(bool ShowWidget)
{
	OnShowFoodOrderWidget.Broadcast(ShowWidget);
}
// OPERATIONS WITH FOOD ITEM
void ABaseCharacter::UseFoodItem()
{
	if (!FoodItem || !AnimTakeFood || Inventory) return;
	InteractionType = EInteractionType::FoodItem;
	AnimBoxTransform = FoodItem->GetStartAnimBoxTransform();
	PossessAIController(AnimBoxTransform.GetLocation());
}
void ABaseCharacter::UseFoodItemContinue()
{
	GetCharacterMovement()->DisableMovement();
	FoodItem->Taken = true;
	FoodItem->SetActorEnableCollision(false);
	SetActorLocationAndRotation(AnimBoxTransform.GetLocation(), AnimBoxTransform.GetRotation());
	PlayAnimMontage(AnimTakeFood);
	PlayingMontage = true;
	//TempRot = CharController->GetPawn()->GetViewRotation();
}
void ABaseCharacter::AttachFoodItem()
{
	const auto AttachComponent = Cast<USceneComponent>(CharMesh);
	if (AttachComponent)
	FoodItem->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "ItemSocket0");
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), TakeSound, GetActorLocation(), 1.0f, 1.0f, 0.0f);
}
void ABaseCharacter::FinishUseFoodItem()
{
	PlayingMontage = false;
	if (FoodItem->FoodItemID == 0)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DrinkSound, GetActorLocation(), 1.0f, 1.0f, 0.0f);
		// REPLACE TO AUDIO COMPONENT
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EatSound, GetActorLocation(), 1.0f, 1.0f, 0.0f);
	}
	SaturationIncrease(FoodItem->FoodItemSaturation);
	FoodItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	FoodItem->Destroy();
	FoodItem = nullptr;
	GetCharacterMovement()->SetDefaultMovementMode();
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetWorldRotation(TempRot);
}

// CHANGE SATURATION
void ABaseCharacter::SaturationDecrease()
{
	if (Saturation > 5.0f)
	{
		Saturation = Saturation - (5.0f * SaturationModifier);
		OnSaturationChange.Broadcast(Saturation / MaxSaturation);
	}
	else
	{
		if (!InCarNow)
		{
			Saturation = MaxSaturation;
			Tiredness = MaxTiredness;
			SetActorLocation(FVector(0.f, 0.f, 0.f)); // Medical centre location
			OnMoneySpent.Broadcast(3, 500.f);
			auto CharPawn = Cast<APawn>(this);
			auto CharPlayerState = Cast<ACSBasePlayerState>(CharPawn->GetPlayerState());
			CharPlayerState->CallMedicalAssistance(500.f);
		}
		// ADD HERE CASE FOR PLAYER IN CAR
	}
}
void ABaseCharacter::SaturationIncrease(float FoodItemSaturation)
{
	auto NewSaturation = Saturation + FoodItemSaturation;
	Saturation = FMath::Clamp(NewSaturation, 0.0f, MaxSaturation);
	OnSaturationChange.Broadcast(Saturation / MaxSaturation);
}

// CHANGE TIREDNESS
void ABaseCharacter::TirednessDecrease()
{
	if (Tiredness > 5.0f)
	{
		Tiredness = Tiredness - 5.0f;
		OnTirednessChange.Broadcast(Tiredness / MaxTiredness);
	}
	else
	{
		if (!InCarNow)
		{
			Tiredness = MaxTiredness;
			SetActorLocation(FVector(0.f, 0.f, 0.f)); // Home location
			// Home logic
		}
		// ADD HERE CASE FOR PLAYER IN CAR
	}
}
void ABaseCharacter::TirednessIncrease()
{
	Tiredness = MaxTiredness;
	OnTirednessChange.Broadcast(Tiredness / MaxTiredness);
	// After sleep animations
}

void ABaseCharacter::PossessAIController(FVector MoveToLocation)
{
	CharController = Cast<ABasePlayerController>(GetController());
	CharacterSelf = Cast<APawn>(this);

	if (!CharacterSelf || !CharController) return;
	TempRot = CharController->GetPawn()->GetViewRotation();
	CharController->CreateAndPossessSpectatorPawn();

	FVector Location;
	FRotator Rotation;
	FActorSpawnParameters Params;
	PlayerAI = GetWorld()->SpawnActor<AAIControllerBaseCharacter>(PlayerAIControllerClassBP, Location, Rotation, Params);

	if (PlayerAI)
	{
		PlayerAI->SetMoveToLocation(MoveToLocation);
		PlayerAI->Possess(CharacterSelf);
		PlayerAI->MoveToLocation(MoveToLocation, -1.0f, false);

		SpringArmComponent->bUsePawnControlRotation = false;
		SpringArmComponent->SetWorldRotation(TempRot);
		CharController->SetViewTargetWithBlend(CharacterSelf, 0.0);
	}
}
void ABaseCharacter::UnPossessAIController()
{
	if (!CharController || !CharacterSelf || !PlayerAI) return;
	CharController->Possess(CharacterSelf);
	CharController->DestroyTempSpectatorPawn();
	PlayerAI->Destroy();

	switch (InteractionType)
	{
	case EInteractionType::Car:
		OnGetCarContinue();
		break;
	case EInteractionType::FoodStore:
		OnOrderFoodContinue();
		break;
	case EInteractionType::FoodItem:
		UseFoodItemContinue();
		break;
	}
}