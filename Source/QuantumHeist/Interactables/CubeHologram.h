// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableComponent.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

#include "CubeHologram.generated.h"

UCLASS()
class QUANTUMHEIST_API ACubeHologram : public AActor
{
	GENERATED_BODY()
	
public:	

	ACubeHologram();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomRotationSignature, FQuat, xRotation);
	UPROPERTY(BlueprintAssignable, Category = "Rotation")
	FOnRoomRotationSignature _OnRoomRotation{};

protected:

	UPROPERTY(VisibleAnywhere)
	UInteractableComponent* _InteractableComponent{};

	
	UPROPERTY(EditAnywhere, category = "Mini Rooms")
	TArray<AActor*> _MiniRooms{};

	UPROPERTY(EditAnywhere)
	UArrowComponent* _CameraArrow;


	// Input

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* _HologramContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _StopInteractionAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _SelectAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	class UInputAction* _RotateAction;


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


	void ChangeMaterialOnHit(UPrimitiveComponent* HitComponent, bool bIsSelected);
	
	APlayerCharacter* _InteractedPlayer{};
	APlayerController* _InteractedPlayerController{};


	UPrimitiveComponent* _CurrentlySelectedComponent{};
};
