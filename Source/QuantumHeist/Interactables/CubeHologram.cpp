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

	
	if (_HologramCube && _RealCube)
	{
		_HologramRotatorComp = _HologramCube->FindComponentByTag<URoomRotatorComponent>(TEXT("Rotator"));
		_HologramRotatorComp->_OnRotationStarted.AddDynamic(this, &ACubeHologram::RecalculateRoomInfos);

		_RealRotatorComp = _RealCube->FindComponentByTag<URoomRotatorComponent>(TEXT("Rotator"));

		// Get all the HologramRooms and sort them based on location

		TArray<AActor*> hologramRooms{};
		_HologramCube->GetAllChildActors(hologramRooms);
		SortRooms(hologramRooms);

		TArray<AActor*> realRooms{};
		_RealCube->GetAllChildActors(realRooms);
		SortRooms(realRooms);
		// Fill the _HologramRooms

		_HologramRooms.Empty();
		for (int index{}; index < hologramRooms.Num(); ++index)
		{
			RotatingRoomInfo toAddRoom{};

			AActor* hologramRoom{ hologramRooms[index]};
			AActor* realRoom{ realRooms[index]};

			toAddRoom.hologramRoom = hologramRoom;
			toAddRoom.hologramRoomComp = hologramRoom->FindComponentByTag<UPrimitiveComponent>(TEXT("HologramCube"));
			toAddRoom.realRoom = realRoom;
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
		EnhancedInputComponent->BindAction(_PerformRotationAction, ETriggerEvent::Started, this, FName{ "PerformRotationAction" });
		EnhancedInputComponent->BindAction(_ResetRotationAction, ETriggerEvent::Started, this, FName{ "ResetRotationAction" });
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
	FInputModeGameAndUI InputMode{};
	_InteractedPlayerController->SetInputMode(InputMode);
}

void ACubeHologram::OnPlayerStopInteract()
{
	if (_InteractedPlayer)
	{
		_InteractedPlayer->ResetCameraToPlayerPos();
		_InteractedPlayer->SwitchToPlayerContext();


		_InteractedPlayerController->bShowMouseCursor = false;
		FInputModeGameOnly InputMode{};
		_InteractedPlayerController->SetInputMode(InputMode);

		_InteractedPlayer = nullptr;

		_InteractableComponent->StopPlayerInteracting(_InteractedPlayer);
	}
}

void ACubeHologram::OnPlayerSelect()
{
	if (!_InteractedPlayer) return;
	if (_HologramRotatorComp->IsRotating()) return;

	FHitResult hitResult{};
	_InteractedPlayerController->GetHitResultUnderCursor(ECC_WorldDynamic, false, hitResult);

	if (hitResult.Component.IsValid())
	{
		if (auto owner = hitResult.Component->GetOwner())
		{
			if (owner->IsA(_BaseRoomBlueprintClass))
			{
				auto hitRoomInfo = _HologramRooms.FindByPredicate([&](const RotatingRoomInfo& room) {return room.hologramRoomComp == hitResult.Component.Get(); });
				if (!hitRoomInfo) return;
				
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

	_HologramCube->AddActorWorldRotation(FQuat{ {0.f, 0.f, 1.f}, _RotationSpeed * -lookingVector.X });
	_HologramCube->AddActorWorldRotation(FQuat{ {0.f, 1.f, 0.f}, _RotationSpeed * lookingVector.Y });
}

void ACubeHologram::OnDirectionChangedAction(const FInputActionValue& value)
{
	if (_HologramRotatorComp->IsRotating()) return;
	if (_CurrentlySelectedRoomIndex == INDEX_NONE) return;

	float dir = value.Get<float>();
	_CurrentAxisIndex += dir;
	if (_CurrentAxisIndex < 0) _CurrentAxisIndex = _Axis.Num() - 1;
	else if (_CurrentAxisIndex >= _Axis.Num()) _CurrentAxisIndex = 0;

	_OnRoomSelectionChanged.Broadcast();
}

void ACubeHologram::PerformRotationAction(const FInputActionValue& value)
{
	if (_HologramRotatorComp->IsRotating()) return;
	if (_CurrentlySelectedRoomIndex == INDEX_NONE) return;

	int dir = value.Get<float>();
	RotationDirection rotationDir{ dir };

	TArray<AActor*> hologramToRotateRooms{};
	TArray<AActor*> realToRotateRooms{};
	TArray<FIntVector3> coordinates{};
	for (auto index : _CurrentlySelectedRoomIndices)
	{
		hologramToRotateRooms.Add(_HologramRooms[index].hologramRoom);
		realToRotateRooms.Add(_HologramRooms[index].realRoom);
		coordinates.Add(_HologramRooms[index].coordinates);
	}

	PerformRotation(_HologramRotatorComp, hologramToRotateRooms, coordinates, _HologramCube, rotationDir);
	PerformRotation(_RealRotatorComp, realToRotateRooms, coordinates, _RealCube, rotationDir);

}

void ACubeHologram::ResetRotationAction()
{
	if (_HologramRotatorComp->IsRotating()) return;

	_HologramCube->SetActorRelativeRotation(FQuat{});
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

void ACubeHologram::PerformRotation(URoomRotatorComponent* rotatorComp, TArray<AActor*> rooms, TArray<FIntVector> coordinates, AActor* cubeActor, RotationDirection dir)
{
	if (rotatorComp->IsRotating()) return;
	if (!cubeActor) return;
	if (rooms.Num() != coordinates.Num()) return;

	// Calculate rotation point (the center room's ActorLocation)
	FVector rotationPoint{};
	FIntVector3 inverseAxis{ FIntVector3{1, 1, 1} - _Axis[_CurrentAxisIndex] };

	for (int index{}; index < rooms.Num(); ++index)
	{
		FIntVector3 coords{ inverseAxis * coordinates[index] };
		if (coords == inverseAxis)
		{
			rotationPoint = rooms[index]->GetActorLocation();
			break;
		}
	}

	// Calculate Rotation Axis
	FVector rotationAxis{};
	FRotator actorRotation{ cubeActor->GetActorRotation() };

	rotationAxis = FVector{ (float)_Axis[_CurrentAxisIndex].X, (float)_Axis[_CurrentAxisIndex].Y, (float)_Axis[_CurrentAxisIndex].Z, };
	rotationAxis = actorRotation.RotateVector(rotationAxis);


	rotatorComp->RotateRooms(rooms, rotationPoint, rotationAxis, dir);
}

