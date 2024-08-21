// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeHologram.h"

ACubeHologram::ACubeHologram()
{
	_InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	_CameraArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Camera Arrow Component"));

	_InteractableComponent->_OnNewPlayerFound.AddDynamic(this, &ACubeHologram::BindActionsToPlayer);
	_InteractableComponent->_OnInteract.AddDynamic(this, &ACubeHologram::OnPlayerInteract);
	_OnRoomSelectionChanged.AddDynamic(this, &ACubeHologram::OnRoomSelectionChanged);
	
}

void ACubeHologram::BeginPlay()
{
	Super::BeginPlay();

	if (_TestRoomsActor)
	{
		_HologramRotatorComp = _TestRoomsActor->FindComponentByTag<URoomRotatorComponent>(TEXT("Rotator"));
		_HologramRotatorComp->_OnRotationStarted.AddDynamic(this, &ACubeHologram::RecalculateRoomInfos);

		// Get all the HologramRooms and sort them based on location

		TArray<AActor*> hologramRooms{};
		_TestRoomsActor->GetAllChildActors(hologramRooms);
		SortRooms(hologramRooms);


		// Fill the _HologramRooms

		_HologramRooms.Empty();
		for (int index{}; index < hologramRooms.Num(); ++index)
		{
			AActor* room{ hologramRooms[index]};
			RotatingRoomInfo toAddRoom{};

			toAddRoom.hologramRoom = room;
			toAddRoom.hologramRoomComp = room->FindComponentByTag<UPrimitiveComponent>(TEXT("Cube"));
			toAddRoom.coordinates = RoomIndexToCoordinates(index);

			_HologramRooms.Add(toAddRoom);
		}
	}
	
}

void ACubeHologram::BindActionsToPlayer(APlayerCharacter* player)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(player->_PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(_StopInteractionAction, ETriggerEvent::Completed, this, FName{ "OnPlayerStopInteract" });
		EnhancedInputComponent->BindAction(_SelectAction, ETriggerEvent::Canceled, this, FName{ "OnPlayerSelect" });
		
		EnhancedInputComponent->BindAction(_RotateAction, ETriggerEvent::Triggered, this, FName{ "OnPlayerRotateAction" });
		EnhancedInputComponent->BindAction(_ChangeDirectionAction, ETriggerEvent::Started, this, FName{ "OnDirectionChangedAction" });
		EnhancedInputComponent->BindAction(_PerformRotationAction, ETriggerEvent::Started, this, FName{ "PerformRotation" });
	}
}


void ACubeHologram::OnPlayerInteract(APlayerCharacter* player)
{
	if (!player) return;

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
		_InteractedPlayer = nullptr;
	}
}

void ACubeHologram::OnPlayerSelect()
{
	if (!_InteractedPlayer) return;

	FHitResult hitResult{};
	_InteractedPlayerController->GetHitResultUnderCursor(ECC_WorldDynamic, false, hitResult);

	if (hitResult.Component.IsValid())
	{
		if (auto owner = hitResult.Component->GetOwner())
		{
			if (owner->IsA(_BaseRoomBlueprintClass))
			{
				auto hitRoomInfo = _HologramRooms.FindByPredicate([&](const RotatingRoomInfo& room) {return room.hologramRoomComp == hitResult.Component.Get(); });
				_CurrentlySelectedRoomIndex = _HologramRooms.Find(*hitRoomInfo);

				if (_CurrentlySelectedRoomIndex != INDEX_NONE)
				{
					_OnRoomSelectionChanged.Broadcast();
				}
			}
		}
	}
}

void ACubeHologram::OnPlayerRotateAction(const FInputActionValue& value)
{
	if (_HologramRotatorComp->IsRotating()) return;

	FVector2D lookingVector = value.Get<FVector2D>();

	_OnRoomRotation.Broadcast(FQuat{ {0.f, 0.f, 1.f}, _RotationSpeed * -lookingVector.X });
	_OnRoomRotation.Broadcast(FQuat{ {0.f, 1.f, 0.f}, _RotationSpeed * lookingVector.Y });
}

void ACubeHologram::OnDirectionChangedAction(const FInputActionValue& value)
{
	if (_HologramRotatorComp->IsRotating()) return;

	float dir = value.Get<float>();
	_CurrentAxisIndex += dir;
	if (_CurrentAxisIndex < 0) _CurrentAxisIndex = _Axis.Num() - 1;
	else if (_CurrentAxisIndex >= _Axis.Num()) _CurrentAxisIndex = 0;

	_OnRoomSelectionChanged.Broadcast();
}

void ACubeHologram::PerformRotation(const FInputActionValue& value)
{
	if (_HologramRotatorComp->IsRotating()) return;

	int dir = value.Get<float>();
	RotationDirection rotationDir{ dir };

	TArray<AActor*> toRotateRooms{};
	for (auto index : _CurrentlySelectedRoomIndices)
	{
		toRotateRooms.Add(_HologramRooms[index].hologramRoom);
	}

	// Calculate rotation point (the center room's ActorLocation)
	FVector rotationPoint{};
	FIntVector3 inverseAxis{ FIntVector3{1, 1, 1} - _Axis[_CurrentAxisIndex] };

	for (int index : _CurrentlySelectedRoomIndices)
	{
		FIntVector3 coords{ inverseAxis * _HologramRooms[index].coordinates};
		if (coords == inverseAxis)
		{
			rotationPoint = _HologramRooms[index].hologramRoom->GetActorLocation();
			break;
		}
	}

	// Calculate Rotation Axis
	FVector rotationAxis{};
	FRotator actorRotation{ _TestRoomsActor->GetActorRotation() };

	rotationAxis = FVector{ (float)_Axis[_CurrentAxisIndex].X, (float)_Axis[_CurrentAxisIndex].Y, (float)_Axis[_CurrentAxisIndex].Z, };
	rotationAxis = actorRotation.RotateVector(rotationAxis);


	_HologramRotatorComp->RotateRooms(toRotateRooms, rotationPoint, rotationAxis, rotationDir);
}

//*******
// Delegate Functions
//*******

void ACubeHologram::OnRoomSelectionChanged()
{
	CalculateSelectedRoomIndices();

	for (int index{}; index < _HologramRooms.Num(); ++index)
	{
		bool shouldBeSelected{ _CurrentlySelectedRoomIndices.Find(index) != INDEX_NONE };
		ChangeMaterialOnHit(_HologramRooms[index].hologramRoomComp, shouldBeSelected);
	}

}

void ACubeHologram::RecalculateRoomInfos()
{
	for (int index : _CurrentlySelectedRoomIndices)
	{
		FIntVector3 currentCoords = _HologramRooms[index].coordinates;
		FIntVector3 axis = _Axis[_CurrentAxisIndex];

		FIntVector3 newCoords = currentCoords;

		if (axis == FIntVector3{ 1, 0, 0 })
		{
			newCoords.Y = _CubeSize - 1 - currentCoords.Z;
			newCoords.Z = currentCoords.Y;
		}
		else if (axis == FIntVector3{ 0, 1, 0 })
		{
			newCoords.X = currentCoords.Z;
			newCoords.Z = _CubeSize - 1 - currentCoords.X;
		}
		else if (axis == FIntVector3{ 0, 0, 1 })
		{
			newCoords.X = _CubeSize - 1 - currentCoords.Y;
			newCoords.Y = currentCoords.X;
		}


		_HologramRooms[index].coordinates = newCoords;
	}
}


//*******
// Helper Functions
//*******

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

FIntVector3 ACubeHologram::RoomIndexToCoordinates(int index)
{
	FIntVector3 coords{};

	coords.X = index % _CubeSize;
	coords.Y = (index / _CubeSize) % _CubeSize;
	coords.Z = index / FMath::Square(_CubeSize);

	return coords;
}

void ACubeHologram::CalculateSelectedRoomIndices()
{
	_CurrentlySelectedRoomIndices.Empty();
	_CurrentlySelectedRoomIndices.Reserve(_CubeSize * _CubeSize);

	FIntVector3 toCompareCoords{ _Axis[_CurrentAxisIndex] * _HologramRooms[_CurrentlySelectedRoomIndex].coordinates };

	for (int index{}; index < _HologramRooms.Num(); ++index)
	{
		auto coords{ _HologramRooms[index].coordinates };
		bool shouldBeSelected = _Axis[_CurrentAxisIndex] * coords == toCompareCoords;

		if (shouldBeSelected)
		{
			_CurrentlySelectedRoomIndices.Add(index);
		}
	}
}

void ACubeHologram::SortRooms(TArray<AActor*>& rooms)
{
	rooms.Sort([](const AActor& first, const AActor& second)
		{
			auto locFirst{ first.GetActorLocation() };
			auto locSecond{ second.GetActorLocation() };

			if (locFirst.Z != locSecond.Z)
			{
				return locFirst.Z < locSecond.Z;
			}
			if (locFirst.Y != locSecond.Y)
			{
				return locFirst.Y < locSecond.Y;
			}
			return locFirst.X < locSecond.X;
		});
}
