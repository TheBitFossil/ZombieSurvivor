// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Kismet/GameplayStatics.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Comp"));
	SetRootComponent(CapsuleComponent);

	FlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	FlipBookComponent->SetupAttachment(RootComponent);
	
	TargetMarker = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Marker"));
	TargetMarker->SetupAttachment(RootComponent);
	TargetMarker->SetVisibility(false);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

EDirectionFacing AEnemy::CalculateFacingDirection()
{
	if(!PlayerTarget)
	{
		return DirectionFacing;
	}
	
	const FVector EnemyLocation = GetActorLocation();
	const FVector PlayerLocation = PlayerTarget->GetActorLocation();
	NormalizedDirectionToPlayer = (PlayerLocation - EnemyLocation).GetSafeNormal();
	
	const FVector ForwardVector = GetActorForwardVector();
	const FVector UpVector = GetActorUpVector();

	// Check if the direction to the player is mostly forward/backward or right/left
	const float DotForward = FVector::DotProduct(ForwardVector, NormalizedDirectionToPlayer);
	const float DotUp = FVector::DotProduct(UpVector, NormalizedDirectionToPlayer);
	if (FMath::Abs(DotForward) > FMath::Abs(DotUp))
	{
		if(DotForward > 0.f)
		{
			DirectionFacing = EDirectionFacing::RIGHT;
		}
		else
		{
			DirectionFacing = EDirectionFacing::LEFT;
		}
	}
	else
	{
		if(DotUp > .0f)
		{
			DirectionFacing = EDirectionFacing::UP;
		}
		else
		{
			DirectionFacing = EDirectionFacing::DOWN;
		}
	}

	return DirectionFacing;
}

void AEnemy::ChasePlayer(const float& Delta)
{
	if(bIsAlive && PlayerTarget)
	{
		const FVector Velocity = GetActorLocation() + NormalizedDirectionToPlayer * MoveSpeed * GetWorld()->GetDeltaSeconds();
		SetActorLocation(Velocity);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(PlayerTarget && bIsAlive)
	{
		const FVector Location = GetActorLocation();
		const FVector PlayerLocation = PlayerTarget->GetActorLocation();

		CalculateFacingDirection();

		// Move towards Player
		DistanceToTarget = FVector::Dist(Location, PlayerLocation);
		if(DistanceToTarget >= StoppingDistance)
		{
			CurrentState = EState::CHASING;
			ChasePlayer(DeltaTime);
		}
		else
		{
			CurrentState = EState::ATTACKING;
		}
	}
	else if(!bIsAlive)
	{
		CurrentState = EState::DEAD;
	}
	else if(!PlayerTarget)
	{
		CurrentState = EState::IDLE;
	}

	// Show TargetMarker if is the closest Target
	if(TargetMarker)
	{
		TargetMarker->SetVisibility(bIsTarget);
	}
	UpdateFlipBookAnim(CurrentState, DirectionFacing);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(!bIsAlive)
	{
		return 0;
	}
	
	if(Health - DamageAmount > 0)
	{
		Health -= DamageAmount;
	}
	else
	{
		// EnemyDied //
		bIsAlive = false;
		Death();
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AEnemy::SetIsTarget(const bool Value)
{
	bIsTarget = Value;
}

void AEnemy::SetTarget(ATopDownCharacter* Target)
{
	if(!Target)
	{
		return;
	}
		
	PlayerTarget = Target;
}

void AEnemy::UpdateFlipBookAnim(const EState& State, const EDirectionFacing& Facing)
{
	if(!FlipBookComponent)
	{
		return;
	}
	
	// Sync Array refs with current State
	const TArray<TObjectPtr<UPaperFlipbook>>* NextFlipBookArray = nullptr;
	switch (State)
	{
	case EState::IDLE:
		NextFlipBookArray = &FB_Idle;
		break;
	case EState::CHASING:
		NextFlipBookArray = &FB_Walk;
		break;
	case EState::ATTACKING:
		NextFlipBookArray = &FB_Attack;
		break;
	case EState::DEAD:
		NextFlipBookArray = &FB_Death;
		break;
	}

	// Animation to play
	UPaperFlipbook* NextFlipBook  = nullptr;
	
	// Choose correct facing from our Array
	if(NextFlipBookArray)
	{
		uint8 idx = static_cast<uint8>(Facing);
		// Make sure that we do not leave the possible arrays
		if(idx < NextFlipBookArray->Num())
		{
			// Dereference our pointer and use the idx (facing)
			NextFlipBook = (*NextFlipBookArray)[idx];
		}
	}
	
	FlipBookComponent->SetFlipbook(NextFlipBook);
}

void AEnemy::Death()
{
	// Change Draw Order
	const int32 Sort = FlipBookComponent->TranslucencySortPriority;
	FlipBookComponent->SetTranslucentSortPriority(Sort - 1);
	FlipBookComponent->SetLooping(false);
	TargetMarker->SetVisibility(false);

	// Inform Spawner
	EnemyDiedDelegate.Broadcast();
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	
	GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &AEnemy::OnDespawnTimerTimeout,
		1.f, false, DespawnTime);
}

void AEnemy::OnDespawnTimerTimeout()
{
	Destroy(this);
}


