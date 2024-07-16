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

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateFacingDirection();
	
	if(bIsAlive && PlayerTarget)
	{
		FVector Location = GetActorLocation();
		FVector PlayerLocation =  PlayerTarget->GetActorLocation();
		Direction =  PlayerLocation - Location;
		//DrawDebugLine(GetWorld(), Location,PlayerLocation, FColor::Blue, false, .2f);
		
		DistanceToTarget = Direction.Size();
		if(DistanceToTarget >= StoppingDistance)
		{
			FVector NormalizedDirection = Direction.GetSafeNormal();
			FVector Velocity = Location + NormalizedDirection * MoveSpeed * DeltaTime;
			SetActorLocation(Velocity);
		}
	}
}

EDirectionFacing AEnemy::CalculateFacingDirection()
{
	// Setting the scale to flip the Sprite
	// FVector Scale = FlipBook->GetComponentScale();
	// if(Scale.X > 0.f)
	// FlipBook->SetWorldScale3D(FVector(-1.f, 1.f, 1.f));

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

