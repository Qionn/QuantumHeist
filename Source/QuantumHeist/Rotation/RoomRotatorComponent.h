#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RoomRotatorComponent.generated.h"

UENUM()
enum class RotationDirection
{
	clockwise = 1,
	counterClockwise = -1
};


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
	void RotateRooms(TArray<AActor*> RoomsToRotate, FVector RotationPoint, FVector RotationAxis, RotationDirection rotationDir);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Rotation")
	float RotationSpeed = 90.0f;

	bool IsRotating() const { return m_IsRotating; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRotation);
	UPROPERTY(BlueprintAssignable)
	FOnRotation _OnRotationCompleted{};

	UPROPERTY(BlueprintAssignable)
	FOnRotation _OnRotationStarted{};

private:
	TArray<AActor*> m_Rooms;
	FVector m_RotationPoint;
	FVector m_RotationAxis;
	float m_CurrentRotation;
	bool m_IsRotating{ false };
	int m_RotationDirection{};

	void PerformRotation(float DeltaTime);
};
