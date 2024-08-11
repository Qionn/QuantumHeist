// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABP_Player::ABP_Player()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize movement speeds
	MoveSpeed = 400.0f;
	SprintSpeed = 800.0f;
	CrouchSpeed = 200.0f;

	// Set initial movement speed
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	// Create camera component

	


}

// Called when the game starts or when spawned
void ABP_Player::BeginPlay()
{
	Super::BeginPlay();

	// Add input mapping context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MovementContext, 1);
		}
	}
}

// Called every frame
void ABP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABP_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind movement actions
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABP_Player::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABP_Player::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABP_Player::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABP_Player::StopSprint);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABP_Player::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABP_Player::StopCrouch);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABP_Player::Jump);
	}
}

void ABP_Player::Move(const FInputActionValue& Value)
{
	// Get movement vector from input
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);


		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABP_Player::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerPitchInput(-LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);

}

void ABP_Player::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ABP_Player::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void ABP_Player::StartCrouch()
{
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	GetCharacterMovement()->bWantsToCrouch = true;
}

void ABP_Player::StopCrouch()
{
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	GetCharacterMovement()->bWantsToCrouch = false;
}

void ABP_Player::Jump()
{
	ACharacter::Jump();
}
