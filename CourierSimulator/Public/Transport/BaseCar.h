// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "BaseCar.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UBoxComponent;

class USkeletalMeshComponent;
class UWheeledVehicleMovementComponent4W;
class UBaseCarAnimInstance;

class UAudioComponent;
class USpotLightComponent;
class UPointLightComponent;

class UAnimMontage;

class ABaseCharacter;
class ACSBasePlayerState;
class ABaseItem;
class AGasStation;

DECLARE_MULTICAST_DELEGATE(FOnInventoryOpenSignature);
DECLARE_MULTICAST_DELEGATE(FOnInventoryCloseSignature);

USTRUCT(BlueprintType)
struct FSocketData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "SocketData")
	FName SocketName;
	UPROPERTY(BlueprintReadOnly, Category = "SocketData")
	bool SocketBusy;
};

UENUM(BlueprintType)
enum class ELightsMode : uint8 {

	Off = 0	UMETA(DisplayName = "Off"),
	Day = 1	UMETA(DisplayName = "Day"),
	Night = 2 UMETA(DisplayName = "Night"),
};

UCLASS()
class COURIERSIMULATOR_API ABaseCar : public AWheeledVehicle
{
	GENERATED_BODY()
	
public:
	ABaseCar();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* CarMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWheeledVehicleMovementComponent4W* Vehicle4W;
	UPROPERTY()
	UBaseCarAnimInstance* CarAnimInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABaseCharacter* CharToPossess;
	UPROPERTY()
	ACSBasePlayerState* CharPlayerState;
	

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void LeaveCar();
	void LeaveCarFinished();
	FTransform GetStartAnimBoxTransform();
	void OpenDoor(bool DoorOpen);

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<ABaseItem*> CarInventory;
	UPROPERTY()
	TArray<FSocketData> CarSocketsData;
	UPROPERTY()
	FSocketData CarSocketData;
	void InitSockets();
	FName FindEmptySocket();
	void ClearSocket(const FName& SocketName);

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	FOnInventoryOpenSignature OnInventoryOpen;
	FOnInventoryCloseSignature OnInventoryClose;

	void DeleteFailedQuestItem(float Reward, float Exp, int32 ID);

	FTimerHandle OnGasolineDecrease;
	FTimerHandle OnGasolineIncrease;
	bool CanGetGasoline = false;
	AGasStation* GasStation;

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CollisionBox")
	UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimBox")
	UBoxComponent* StartAnimBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAudioComponent* EngineSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAudioComponent* DoorSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAudioComponent* HonkSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* OpeningDoorMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	USpotLightComponent* SpotLightR1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	USpotLightComponent* SpotLightR2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	USpotLightComponent* SpotLightL1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	USpotLightComponent* SpotLightL2;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightFR1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightFR2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightFL1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightFL2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightRR1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightRR2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightRL1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightRL2;	

private:
	void InitAnimations();
	
	UFUNCTION()
	void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ApplyThrottle(float Val);
	void ApplySteering(float Val);
	void LookUp(float Val);
	void Turn(float Val);
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	void PlayHonkSound();

	void SwitchLights(ELightsMode Mode);
	void CharSwitchLights();
	void SwitchBreakLights(bool BrakeLights);
	ELightsMode LightsState;

	bool IsDoorOpened;
	void PlayDoorSound();

	void GasolineIncrease();
	void GasolineDecrease();
	void TryGetGasoline();
	void StopGetGasoline();

	float GasolineAmount = 40.0f;
	float MaxGasolineAmount = 80.0f;
	float GasolineReceived = 0.0f;
	float GasolineMoneySpent = 0.0f;
};