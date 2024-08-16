// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "../Characters/PlayerCharacter.h"

#include "InteractableComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUANTUMHEIST_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractableComponent();
	void BeginPlay() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractSignature, APlayerCharacter*, player);
	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FOnInteractSignature _OnInteract;

	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FOnInteractSignature _OnStopInteract;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collisions")
	UBoxComponent* _PlayerOverlapCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collisions")
	UBoxComponent* _PlayerViewCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collisions")
	UArrowComponent* _ArrowRootComponent;

protected:

	UPROPERTY(EditAnywhere, Category = "RayCasting")
	float _RayCastMaxDistance{500.f};

private:
	UFUNCTION()
	void BeginOverlapBox(UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex,
		bool bFromSweep,
		const FHitResult& sweepResult);

	UFUNCTION()
	void EndOverlapBox(UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsLookingAtViewCollision(APlayerCharacter* actor);

	UFUNCTION()
	void OnPlayerInteracts(APlayerCharacter* playerCharacter);
	UFUNCTION()
	void OnPlayerStopsInteract(APlayerCharacter* playerCharacter);

	TMap<APlayerCharacter*, bool> _PlayerOverlaps{};
	TArray<APlayerCharacter*> _LookingPlayers{};
	
};
