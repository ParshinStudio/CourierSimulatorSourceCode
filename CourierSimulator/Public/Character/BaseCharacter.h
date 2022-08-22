// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Items/BaseItem.h"
#include "BaseCharacter.generated.h"



class UCameraComponent;
class USpringArmComponent;
class USphereComponent;

class UAnimMontage;
class USoundCue;

class ACSGameModeBase;
class AAIControllerBaseCharacter;
class ABasePlayerController;
class APawn;

class USkeletalMeshComponent;
class UBaseCharAnimInstance;

class ABaseCar;
class ABaseItem;

class AFoodStore;
class AFoodItem;

DECLARE_MULTICAST_DELEGATE(FOnInventoryChangeSignature);
DECLARE_MULTICAST_DELEGATE(FOnInventoryBusySignature);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaChangeSignature, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaturationChangeSignature, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTirednessChangeSignature, float)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowFoodOrderWidgetSignature, bool);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMoneySpentSignature, int32, float);
DECLARE_MULTICAST_DELEGATE(FOnNotEnoughtMoneySignature);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowCarWidgetSignature, bool);

UENUM(BlueprintType)
enum class EInteractionType : uint8 
{

	Car = 0	UMETA(DisplayName = "Car"),
	FoodStore = 1 	UMETA(DisplayName = "FoodStore"),
	FoodItem = 2	UMETA(DisplayName = "FoodItem")
};

UCLASS()
class COURIERSIMULATOR_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter(const FObjectInitializer& ObjInit);
	ACSGameModeBase* GameMode;
	USkeletalMeshComponent* CharMesh;
	UBaseCharAnimInstance* CharAnimInstance;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "CarToPossess")
	ABaseCar* CarToPossess;
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	ABaseItem* Inventory = nullptr;
	ABaseItem* ItemToPickup = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AAIControllerBaseCharacter> PlayerAIControllerClassBP;
	UPROPERTY()
	AAIControllerBaseCharacter* PlayerAI;
	UPROPERTY()
	APawn* CharacterSelf;

	UPROPERTY(BlueprintReadOnly, Category = "Info")
	bool CanSitInCar = false;
	UPROPERTY(BlueprintReadOnly, Category = "Info")
	bool InCarNow = false;
	UPROPERTY(BlueprintReadOnly, Category = "Info")
	bool PlayingMontage = false;

	FOnInventoryChangeSignature OnInventoryChange;
	FOnInventoryBusySignature OnInventoryBusy;

	FOnStaminaChangeSignature OnStaminaChange;
	FOnSaturationChangeSignature OnSaturationChange;
	FOnTirednessChangeSignature OnTirednessChange;

	FOnShowFoodOrderWidgetSignature OnShowFoodOrderWidget;

	FOnMoneySpentSignature OnMoneySpent;
	FOnNotEnoughtMoneySignature OnNotEnoughtMoney;

	FOnShowCarWidgetSignature OnShowCarWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Info")
	UAnimMontage* AnimEnterCar;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Info")
	UAnimMontage* AnimLeaveCar;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Info")
	UAnimMontage* MakeFoodOrder;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Info")
	UAnimMontage* AnimTakeFood;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Info")
	USoundCue* TakeSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Info")
	USoundCue* DrinkSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Info")
	USoundCue* EatSound;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsRunning() const;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OnTakeFromCarItem(int Index);

	UPROPERTY()
	AFoodStore* FoodStore = nullptr;
	UFUNCTION(BlueprintCallable)
	void OnOrderFood(int32 FoodToOrder, float Cost);
	void OnOrderFoodContinue();
	void OnFinishOrderFood();
	void ShowFoodStoreWidget(bool ShowWidget);
	int32 TempFoodOrder;
	float TempFoodCost;

	UPROPERTY()
	AFoodItem* FoodItem = nullptr;
	void UseFoodItem();
	void UseFoodItemContinue();
	void AttachFoodItem();
	void FinishUseFoodItem();

	UFUNCTION(BlueprintCallable)
	FVector GetLeftSocketLoc();
	UFUNCTION(BlueprintCallable)
	FVector GetRightSocketLoc();

	ABasePlayerController* CharController;
	void PossessAIController(FVector MoveToLocation);
	void UnPossessAIController();
	EInteractionType InteractionType;
	FTransform AnimBoxTransform;
	UPROPERTY()
	FRotator TempRot;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* CameraCollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool ShowedPhone = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool CarryingBox = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Stamina = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float MaxStamina = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Saturation", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float Saturation = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Saturation", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float MaxSaturation = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Saturation", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float Tiredness = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Saturation", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float MaxTiredness = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float StaminaModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Saturation", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float SaturationModifier = 1.0f;

	FTimerHandle StaminaHandleDecrease;
	FTimerHandle StaminaHandleIncrease;
	FTimerHandle SaturationHandleDecrease;
	FTimerHandle TirednessHandleDecrease;

private:
	void CheckCameraOverlap();
	void InitAnimations();

	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void OnStartRunning();
	void OnStopRunning();
	void Jumping();
	void StaminaDecrease();
	void StaminaIncrease();

	bool IsMoving = false;
	bool IsMovingRight = false;
	bool IsSprinting = false;

	void OnGetCar();
	void OnGetCarContinue();
	void OnGetCarFinished();
	void LeaveCarFinished();

	void OnGetItem();
	void OnPutInCarItem();

	void DeleteFailedQuestItem(float Reward, float Exp, int32 ID);

	void ChangePhoneState(bool PhoneState);
	void ChangeCarryingBoxStateOnPassQuest(float Reward, float Exp, int32 ID);

	void SaturationDecrease();
	void SaturationIncrease(float FoodItemSaturation);
	void TirednessDecrease();
	void TirednessIncrease();

	void OnEnteringStartCarDoorOpening();
	void OnEnteringEndCarDoorClosing();

	UFUNCTION()
	void OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};