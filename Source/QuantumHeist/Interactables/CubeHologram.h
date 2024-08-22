// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableComponent.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "../Rotation/RoomRotatorComponent.h"

#include "CubeHologram.generated.h"

UCLASS()
class QUANTUMHEIST_API ACubeHologram : public AActor
{
	GENERATED_BODY()
	
public:	

	struct RotatingRoomInfo
	{
		AActor* hologramRoom{};
		AActor* realRoom{};

		UPrimitiveComponent* hologramRoomComp{};

		FIntVector3 coordinates{};

		bool operator==(const RotatingRoomInfo& other) const
		{
			return  hologramRoom == other.hologramRoom &&
					hologramRoomComp == other.hologramRoomComp &&
					coordinates == other.coordinates;
		}
	};

	ACubeHologram();
	void BeginPlay() override;

	// Delegates

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedRoomsChanged);
	UPROPERTY(BlueprintAssignable)
	FOnSelectedRoomsChanged _OnRoomSelectionChanged{};

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UInteractableComponent* _InteractableComponent{};

	UPROPERTY(EditAnywhere)
	UArrowComponent* _CameraArrow;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rooms")
	AActor* _HologramCube{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rooms")
	AActor* _RealCube{};

	// Variables
	APlayerCharacter* _InteractedPlayer{};
	APlayerController* _InteractedPlayerController{};

	URoomRotatorComponent* _HologramRotatorComp{};
	URoomRotatorComponent* _RealRotatorComp{};
	TArray<RotatingRoomInfo> _HologramRooms{};

	TArray<int> _CurrentlySelectedRoomIndices{};
	int _CurrentlySelectedRoomIndex{ INDEX_NONE };

	TArray<FIntVector3> _Axis{ {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };
	int _CurrentAxisIndex{ 0 };

	const int _CubeSize{ 3 };

	// Input

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* _HologramContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _StopInteractionAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _SelectAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _RotateAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _ChangeDirectionAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _PerformRotationAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _ResetRotationAction;

	// Room

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cubes")
	TSubclassOf<AActor> _BaseRoomBlueprintClass{};

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rotation")
	float _RotationSpeed{ 0.01f };

	
	// Materials
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* _UnselectedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* _SelectedMaterial;

private:	

	// Input Functions

	UFUNCTION()
	void BindActionsToPlayer(APlayerCharacter* player);

	UFUNCTION()
	void OnPlayerInteract(APlayerCharacter* player);
	UFUNCTION()
	void OnPlayerStopInteract();

	UFUNCTION()
	void OnPlayerSelect();

	UFUNCTION()
	void OnPlayerRotateAction(const FInputActionValue& value);
	UFUNCTION()
	void OnDirectionChangedAction(const FInputActionValue& value);

	UFUNCTION()
	void PerformRotationAction(const FInputActionValue& value);

	UFUNCTION()
	void ResetRotationAction();

	// Delegate Functions
	UFUNCTION()
	void OnRoomSelectionChanged();
	UFUNCTION()
	void RecalculateRoomInfos();

	// Helper Functions
	void ChangeMaterialOnHit(UPrimitiveComponent* HitComponent, bool bIsSelected);
	FIntVector3 RoomIndexToCoordinates(int index);
	void CalculateSelectedRoomIndices();
	void SortRooms(TArray<AActor*>& rooms);

	void PerformRotation(URoomRotatorComponent* rotatorComp, TArray<AActor*> rooms, TArray<FIntVector> coordinates, AActor* cubeActor, RotationDirection dir);

};
