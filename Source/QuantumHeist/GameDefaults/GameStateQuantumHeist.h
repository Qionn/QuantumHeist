// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Actor.h"

#include "GameStateQuantumHeist.generated.h"

/**
 * 
 */
UCLASS()
class QUANTUMHEIST_API AGameStateQuantumHeist : public AGameStateBase
{
	GENERATED_BODY()
	
	void BeginPlay() override;
};
