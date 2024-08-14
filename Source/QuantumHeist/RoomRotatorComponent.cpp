#include "RoomRotatorComponent.h"
#include "Kismet/KismetMathLibrary.h"

URoomRotatorComponent::URoomRotatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_CurrentRotation = 0.0f;
	m_IsRotating = false;
}


void URoomRotatorComponent::BeginPlay()
{
	Super::BeginPlay();
}


void URoomRotatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_IsRotating)
	{
		PerformRotation(DeltaTime);
	}
}


void URoomRotatorComponent::RotateRooms(TArray<AActor*> RoomsToRotate, FVector RotationPoint, FVector RotationAxis)
{
	if (m_IsRotating) return;

	m_Rooms = RoomsToRotate;
	m_RotationPoint = RotationPoint;
	m_RotationAxis = RotationAxis;
	m_CurrentRotation = 0.0f;
	m_IsRotating = true;

	m_RotationAxis.Normalize();
}


void URoomRotatorComponent::PerformRotation(float DeltaTime)
{
	float RotationThisFrame = RotationSpeed * DeltaTime;
	m_CurrentRotation += RotationThisFrame;

	if (m_CurrentRotation >= 90.0f)
	{
		RotationThisFrame -= (m_CurrentRotation - 90.0f);
		m_IsRotating = false;
	}

	for (AActor* Room : m_Rooms)
	{
		if (!Room) continue;

		FVector RoomLocation = Room->GetActorLocation();
		FVector ToRoom = RoomLocation - m_RotationPoint;
		ToRoom = ToRoom.RotateAngleAxis(RotationThisFrame, m_RotationAxis);
		RoomLocation = m_RotationPoint + ToRoom;

		FQuat RotationQuat = FQuat(m_RotationAxis, FMath::DegreesToRadians(RotationThisFrame));
		FQuat NewRotation = RotationQuat * Room->GetActorRotation().Quaternion();

		Room->SetActorLocationAndRotation(RoomLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (!m_IsRotating)
	{
		m_CurrentRotation = 0.0f;
	}
}

