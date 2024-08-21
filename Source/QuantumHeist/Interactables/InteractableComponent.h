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


	// Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collisions")
	UBoxComponent* _PlayerOverlapCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collisions")
	UBoxComponent* _PlayerClickCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UArrowComponent* _ArrowRootComponent;


	// Delegates

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewPlayerSignature, APlayerCharacter*, player);
	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FOnNewPlayerSignature _OnNewPlayerFound;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractSignature, APlayerCharacter*, player);
	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FOnInteractSignature _OnInteract;


	struct PlayerInfo 
	{
		APlayerCharacter* player{};
		bool inOverlapCollision{ false };
		bool lookingAtClickCollision{ false };
	};
	TArray<PlayerInfo> _PlayerInfos{};

protected:

	

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

	bool IsLookingAtClickCollision(APlayerCharacter* actor);

	UFUNCTION()
	void OnPlayerInteracts(APlayerCharacter* player);
};
