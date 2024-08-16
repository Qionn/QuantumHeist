#include "InteractableComponent.h"
#include "DrawDebugHelpers.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	_ArrowRootComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowRootComponent"));
	
	_PlayerViewCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ViewCollision"));
	_PlayerViewCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_PlayerViewCollision->SetBoxExtent({100.f, 100.f, 100.f});
	_PlayerViewCollision->SetHiddenInGame(false);

	
	_PlayerOverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollision"));
	_PlayerOverlapCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_PlayerOverlapCollision->SetBoxExtent({300.f, 300.f, 300.f});
	_PlayerOverlapCollision->SetHiddenInGame(false);

	auto owner{ GetOwner() };
	if (owner)
	{
		owner->SetRootComponent(_ArrowRootComponent);

		_PlayerViewCollision->SetupAttachment(owner->GetRootComponent());
		_PlayerOverlapCollision->SetupAttachment(owner->GetRootComponent());
	}

}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();


	_PlayerViewCollision->AttachToComponent(GetOwner()->GetRootComponent()->GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform);
	_PlayerOverlapCollision->AttachToComponent(GetOwner()->GetRootComponent()->GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform);

	_PlayerOverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::BeginOverlapBox);
	_PlayerOverlapCollision->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::EndOverlapBox);
}


void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (const auto& playerInfo : _PlayerOverlaps)
	{
		if (!playerInfo.Key)
		{
			_PlayerOverlaps.Remove(playerInfo.Key);
		}

		
		if (playerInfo.Value)
		{
			if (IsLookingAtViewCollision(playerInfo.Key))
			{
				if (_LookingPlayers.Find(playerInfo.Key) != INDEX_NONE) continue;

				_LookingPlayers.Add(playerInfo.Key);

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Player Looking At Interactable"));;
			}
			else
			{
				if (_LookingPlayers.Find(playerInfo.Key) == INDEX_NONE) continue;

				_LookingPlayers.Remove(playerInfo.Key);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Player Stopped Looking At Interactable"));;

			}
		}
	}
}

bool UInteractableComponent::IsLookingAtViewCollision(APlayerCharacter* actor)
{
	FVector start{ actor->Camera->GetComponentLocation() };

	auto cameraRotation{ actor->Camera->GetComponentRotation() };
	auto cameraDir{ cameraRotation.Vector().GetSafeNormal() };

	FVector end{ start + cameraDir * _RayCastMaxDistance };

	DrawDebugLine(GetWorld(), start, end, FColor::Black);

	return FMath::LineBoxIntersection(_PlayerViewCollision->Bounds.GetBox(), start, end, end - start);
}


void UInteractableComponent::BeginOverlapBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (!otherActor) return;

	if (APlayerCharacter* playerChar = Cast<APlayerCharacter>(otherActor))
	{
		if (auto isPlayerInBox = _PlayerOverlaps.Find(playerChar))
		{
			*isPlayerInBox = true;
		}
		else
		{
			_PlayerOverlaps.Add(playerChar, true);
			playerChar->_OnInteract.AddDynamic(this, &UInteractableComponent::OnPlayerInteracts);
			playerChar->_OnStopInteract.AddDynamic(this, &UInteractableComponent::OnPlayerStopsInteract);
		}
	}
}

void UInteractableComponent::EndOverlapBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (APlayerCharacter* playerChar = Cast<APlayerCharacter>(otherActor))
	{
		if (auto isPlayerInBox = _PlayerOverlaps.Find(playerChar))
		{
			*isPlayerInBox = false;
		}
		if (_LookingPlayers.Find(playerChar) != INDEX_NONE)
		{
			_LookingPlayers.Remove(playerChar);
		}
	}
}


void UInteractableComponent::OnPlayerInteracts(APlayerCharacter* playerCharacter)
{
	if (_LookingPlayers.Find(playerCharacter) != INDEX_NONE)
	{
		_OnInteract.Broadcast(playerCharacter);
	}
}

void UInteractableComponent::OnPlayerStopsInteract(APlayerCharacter* playerCharacter)
{
	_OnStopInteract.Broadcast(playerCharacter);
}

