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
		FVector Location = GetActorLocation();
		FVector PlayerLocation =  PlayerTarget->GetActorLocation();
		Direction =  PlayerLocation - Location;
		//DrawDebugLine(GetWorld(), Location,PlayerLocation, FColor::Blue, false, .2f);
		
		DistanceToTarget = Direction.Size();
		
		FVector NormalizedDirection = Direction.GetSafeNormal();
		FVector Velocity = Location + NormalizedDirection * MoveSpeed * DeltaTime;
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
	TArray<TObjectPtr<UPaperFlipbook>> NextFlipBookArray;
	UPaperFlipbook* NextFlipBook  = nullptr;

	// Sync Array with current State
	switch (State)
	{
	case EState::IDLE:
		NextFlipBookArray = FB_Idle;
		break;
	case EState::CHASING:
		NextFlipBookArray = FB_Walk;
		break;
	case EState::ATTACKING:
		NextFlipBookArray = FB_Attack;
		break;
	case EState::DEAD:
		NextFlipBookArray = FB_Death;
		break;
	}

	// Choose correct facing from our Array
	switch (Facing)
	{
	case EDirectionFacing::UP:
		NextFlipBook = NextFlipBookArray[0];
		break;
	case EDirectionFacing::DOWN:
		NextFlipBook = NextFlipBookArray[1];
		break;
	case EDirectionFacing::RIGHT:
		NextFlipBook = NextFlipBookArray[2];
		break;
	case EDirectionFacing::LEFT:
		NextFlipBook = NextFlipBookArray[3];
		break;
	}

	FlipBookComponent->SetFlipbook(NextFlipBook);
}


