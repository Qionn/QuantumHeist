#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RoomRotatorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUANTUMHEIST_API URoomRotatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URoomRotatorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Room Rotation")
	void RotateRooms(TArray<AActor*> RoomsToRotate, FVector RotationPoint, FVector RotationAxis);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Rotation")
	float RotationSpeed = 90.0f;

private:
	TArray<AActor*> m_Rooms;
	FVector m_RotationPoint;
	FVector m_RotationAxis;
	float m_CurrentRotation;
	bool m_IsRotating;

	void PerformRotation(float DeltaTime);
};
