// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeHologram.h"

ACubeHologram::ACubeHologram()
{
	_InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	_CameraArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Camera Arrow Component"));


	_InteractableComponent->_OnInteract.AddDynamic(this, &ACubeHologram::OnPlayerInteract);
	_InteractableComponent->_OnStopInteract.AddDynamic(this, &ACubeHologram::OnPlayerStopInteract);

	_MiniRooms.Reserve(27);
	for (int index{}; index < 27; ++index)
	{
		//_MiniRooms.Add(CreateDefaultSubobject<>());
	}
}

void ACubeHologram::BeginPlay()
{
	Super::BeginPlay();

}


void ACubeHologram::OnPlayerInteract(APlayerCharacter* player)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Player Interacted With Hologram"));;

	player->MoveCameraToComponent(_CameraArrow);
	player->SwitchToInteractableContext();
}

void ACubeHologram::OnPlayerStopInteract(APlayerCharacter* player)
{
	player->ResetCameraToPlayerPos();
	player->SwitchToPlayerContext();
}
