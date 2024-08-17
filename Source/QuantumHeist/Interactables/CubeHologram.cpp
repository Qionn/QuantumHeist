// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeHologram.h"

ACubeHologram::ACubeHologram()
{
	_InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	_CameraArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Camera Arrow Component"));

	_InteractableComponent->_OnNewPlayerFound.AddDynamic(this, &ACubeHologram::BindActionsToPlayer);
	_InteractableComponent->_OnInteract.AddDynamic(this, &ACubeHologram::OnPlayerInteract);

	_MiniRooms.Reserve(27);
	for (int index{}; index < 27; ++index)
	{
		//_MiniRooms.Add(CreateDefaultSubobject<>());
	}
}

void ACubeHologram::BindActionsToPlayer(APlayerCharacter* player)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(player->_PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(_StopInteractionAction, ETriggerEvent::Completed, this, FName{ "OnPlayerStopInteract" });
		EnhancedInputComponent->BindAction(_SelectAction, ETriggerEvent::Completed, this, FName{ "OnPlayerSelect" });
	}
}


void ACubeHologram::OnPlayerInteract(APlayerCharacter* player)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Player Interacted With Hologram"));;

	_InteractedPlayer = player;
	player->MoveCameraToComponent(_CameraArrow);
	player->SwitchToMappingContext(_HologramContext);


	_InteractedPlayerController = Cast<APlayerController>(player->GetController());
	_InteractedPlayerController->bShowMouseCursor = true;
}

void ACubeHologram::OnPlayerStopInteract()
{
	if (_InteractedPlayer)
	{
		_InteractedPlayer->ResetCameraToPlayerPos();
		_InteractedPlayer->SwitchToPlayerContext();


		_InteractedPlayerController->bShowMouseCursor = false;
	}

	if (_CurrentlySelectedComponent)
	{
		ChangeMaterialOnHit(_CurrentlySelectedComponent, false);
	}
}

void ACubeHologram::OnPlayerSelect()
{
	if (!_InteractedPlayer) return;

	FHitResult hitResult{};

	FVector start{ _InteractedPlayer->Camera->GetComponentLocation() };
	auto cameraRotation{ _InteractedPlayer->Camera->GetComponentRotation() };
	auto cameraDir{ cameraRotation.Vector().GetSafeNormal() };
	FVector end{ start + cameraDir * _InteractableComponent->_RayCastMaxDistance };

	_InteractedPlayerController->GetHitResultUnderCursor(ECC_WorldDynamic, false, hitResult);

	if (hitResult.Component.IsValid())
	{
		if (auto owner = hitResult.Component->GetOwner())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, owner->GetActorLabel());;

			if (owner->IsA(_BaseRoomBlueprintClass))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Hit a Room");
				
				if (_CurrentlySelectedComponent)
				{
					ChangeMaterialOnHit(_CurrentlySelectedComponent, false);
				}

				_CurrentlySelectedComponent = hitResult.Component.Get();
				ChangeMaterialOnHit(_CurrentlySelectedComponent, true);
			}
		}
	}
}


void ACubeHologram::ChangeMaterialOnHit(UPrimitiveComponent* HitComponent, bool bIsSelected)
{
	if (HitComponent)
	{
		UMaterialInterface* MaterialToApply = bIsSelected ? _SelectedMaterial : _UnselectedMaterial;
		if (MaterialToApply)
		{
			HitComponent->SetMaterial(0, MaterialToApply);
		}
	}
}
