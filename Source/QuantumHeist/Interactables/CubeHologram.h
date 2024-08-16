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

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UInteractableComponent* _InteractableComponent{};

	
	UPROPERTY(EditAnywhere, category = "Mini Rooms")
	TArray<AActor*> _MiniRooms{};

	UPROPERTY(EditAnywhere)
	UArrowComponent* _CameraArrow;


private:	

	UFUNCTION()
	void OnPlayerInteract(APlayerCharacter* player);
	UFUNCTION()
	void OnPlayerStopInteract(APlayerCharacter* player);

	
	FVector _OriginalSpringArmRelativeLocation{};
};
