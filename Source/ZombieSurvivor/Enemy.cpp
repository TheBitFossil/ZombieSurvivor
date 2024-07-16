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
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Enemies should have the player from start
	if(PlayerTarget == nullptr)
	{
		if(AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownCharacter::StaticClass()))
		{
			PlayerTarget = static_cast<ATopDownCharacter*>(Actor);
			//DrawDebugLine(GetWorld(), GetActorLocation(),PlayerTarget->GetActorLocation(), FColor::Red, true);
		}
	}
}

EDirectionFacing AEnemy::CalculateFacingDirection()
{
	FVector ForwardVector = GetActorForwardVector();
	FVector UpVector = GetActorUpVector();
	
	GEngine->AddOnScreenDebugMessage(-1, .2f, FColor::Green, *ForwardVector.ToString());
	
	float DotForward = FVector::DotProduct(ForwardVector, Direction);
	GEngine->AddOnScreenDebugMessage(-1, .2f, FColor::Green, FString::Printf(TEXT("DotF: %f"), DotForward));

	float DotUp = FVector::DotProduct(UpVector, Direction);
	GEngine->AddOnScreenDebugMessage(-1, .2f, FColor::Green, FString::Printf(TEXT("DotU: %f"), DotUp));

	// Check if the direction to the player is mostly forward/backward or right/left
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

void AEnemy::ChasePlayer(float DeltaTime)
{
	if(bIsAlive && PlayerTarget)
	{
		FVector NormalizedDirection = Direction.GetSafeNormal();
		FVector Velocity = GetActorLocation() + NormalizedDirection * MoveSpeed * DeltaTime;
		SetActorLocation(Velocity);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateFacingDirection();
	
	if(!bIsAlive)
	{
		CurrentState = EState::DEAD;
	}
	else if(!PlayerTarget)
	{
		CurrentState = EState::IDLE;
	}
	else
	{
		// Active Enemy
		Direction =  PlayerTarget->GetActorLocation() - GetActorLocation();
		DistanceToTarget = Direction.Size();
		
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

	UpdateFlipBookAnim(CurrentState, DirectionFacing);
}

void AEnemy::UpdateFlipBookAnim(const EState& State, const EDirectionFacing& Facing)
{
	if(!FlipBookComponent)
	{
		return;
	}
	
	// Animation to play
	UPaperFlipbook* NextFlipBook  = nullptr;

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


