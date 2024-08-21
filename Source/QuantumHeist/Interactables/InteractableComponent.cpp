#include "InteractableComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	_ArrowRootComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowRootComponent"));
	
	_PlayerClickCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ViewCollision"));
	_PlayerClickCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_PlayerClickCollision->SetBoxExtent({100.f, 100.f, 100.f});
	_PlayerClickCollision->SetHiddenInGame(false);

	
	_PlayerOverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollision"));
	_PlayerOverlapCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_PlayerOverlapCollision->SetBoxExtent({300.f, 300.f, 300.f});
	_PlayerOverlapCollision->SetHiddenInGame(false);

	auto owner{ GetOwner() };
	if (owner)
	{
		owner->SetRootComponent(_ArrowRootComponent);

		_PlayerClickCollision->SetupAttachment(owner->GetRootComponent());
		_PlayerOverlapCollision->SetupAttachment(owner->GetRootComponent());
	}

}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();


	_PlayerClickCollision->AttachToComponent(GetOwner()->GetRootComponent()->GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform);
	_PlayerOverlapCollision->AttachToComponent(GetOwner()->GetRootComponent()->GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform);

	_PlayerOverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::BeginOverlapBox);
	_PlayerOverlapCollision->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::EndOverlapBox);

	// Add all players to the map
	TArray<AActor*> players{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), players);
	{
		for (const auto& playerActor : players)
		{
			auto player{ Cast<APlayerCharacter>(playerActor) };

			_PlayerInfos.Add({ player, false, false });

			player->_OnInteract.AddDynamic(this, &UInteractableComponent::OnPlayerInteracts);
			_OnNewPlayerFound.Broadcast(player);
		}
	}
}


void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& playerInfo : _PlayerInfos)
	{
		if (!playerInfo.player) continue;
		
		if (playerInfo.inOverlapCollision)
		{
			playerInfo.lookingAtClickCollision = IsLookingAtClickCollision(playerInfo.player);
		}
	}
}

bool UInteractableComponent::IsLookingAtClickCollision(APlayerCharacter* actor)
{
	FVector start{ actor->Camera->GetComponentLocation() };

	auto cameraRotation{ actor->Camera->GetComponentRotation() };
	auto cameraDir{ cameraRotation.Vector().GetSafeNormal() };

	FVector end{ start + cameraDir * FLT_MAX };

	DrawDebugLine(GetWorld(), start, end, FColor::Black);

	return FMath::LineBoxIntersection(_PlayerClickCollision->Bounds.GetBox(), start, end, end - start);
}


void UInteractableComponent::BeginOverlapBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (!otherActor) return;

	if (APlayerCharacter* playerChar = Cast<APlayerCharacter>(otherActor))
	{
		if (auto playerInfo = _PlayerInfos.FindByPredicate([&](const PlayerInfo& toCompareInfo) {return playerChar == toCompareInfo.player;}))
		{
			playerInfo->inOverlapCollision = true;
		}
		else
		{
			_PlayerInfos.Add({ playerChar, true, false });

			playerChar->_OnInteract.AddDynamic(this, &UInteractableComponent::OnPlayerInteracts);
			_OnNewPlayerFound.Broadcast(playerChar);
		}
	}
}

void UInteractableComponent::EndOverlapBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (APlayerCharacter* playerChar = Cast<APlayerCharacter>(otherActor))
	{
		if (auto playerInfo = _PlayerInfos.FindByPredicate([&](const PlayerInfo& toCompareInfo) {return playerChar == toCompareInfo.player; }))
		{
			playerInfo->inOverlapCollision = false;
			playerInfo->lookingAtClickCollision = false;
		}
	}
}


void UInteractableComponent::OnPlayerInteracts(APlayerCharacter* playerCharacter)
{
	auto toFindPlayerInfo{ PlayerInfo{ playerCharacter, true, true } };

	if (_PlayerInfos.FindByPredicate([&](const PlayerInfo& other) {return 
						toFindPlayerInfo.player == other.player &&
						toFindPlayerInfo.inOverlapCollision == other.inOverlapCollision &&
						toFindPlayerInfo.lookingAtClickCollision == other.lookingAtClickCollision;}))
	{
		_OnInteract.Broadcast(playerCharacter);
	}
}

