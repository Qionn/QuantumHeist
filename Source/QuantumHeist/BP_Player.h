// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "BP_Player.generated.h"

UCLASS()
class QUANTUMHEIST_API ABP_Player : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;


public:
	// Sets default values for this character's properties
	ABP_Player();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed;



	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* LookAction;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* JumpAction;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* MovementContext;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Movement and input functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartSprint();
	void StopSprint();

	void StartCrouch();
	void StopCrouch();

	void Jump();





};
