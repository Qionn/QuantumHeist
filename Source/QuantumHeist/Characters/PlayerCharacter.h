// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "PlayerCharacter.generated.h"

UCLASS()
class QUANTUMHEIST_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	


public:
	// Sets default values for this character's properties
	APlayerCharacter();
	
	UPROPERTY(EditAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCameraComponent* Camera;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractSignature, APlayerCharacter*, player);
	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FOnInteractSignature _OnInteract;
	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FOnInteractSignature _OnStopInteract;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed;


	// Input Context & Actions for regular Player
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Player")
	class UInputMappingContext* PlayerContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Player")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Player")
	class UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Player")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Player")
	class UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Player")
	class UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Player")
	class UInputAction* InteractAction;


	// Input Context & Actions for Interactables

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Interactable")
	class UInputMappingContext* InteractableContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context Interactable")
	class UInputAction* StopInteractAction;
	


	void SwitchToMappingContext(UInputMappingContext* context);


	// Camera Management
	FVector _OriginalCameraPosition{};

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input Context Functions
	//void BindAction(const UInputAction* action, ETriggerEvent triggerEvent, auto object, auto function);

	void SwitchToPlayerContext();
	void SwitchToInteractableContext();
	void SwitchToContext(UInputMappingContext* context);

	// Movement and input functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartSprint();
	void StopSprint();

	void StartCrouch();
	void StopCrouch();

	void Jump();

	void Interaction();
	void StopInteraction();

	// Camera Management
	void MoveCameraToComponent(USceneComponent* component);
	void ResetCameraToPlayerPos();

};
