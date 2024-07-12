// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "TopDownCharacter.h"


AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;

	FlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbBook Comp"));
	SetRootComponent(FlipBookComponent);
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
}

void AGun::SetAnimation(const bool bIsMoving, const EDirectionFacing& Direction) const
{
	
	// Either Running or Idling
	TArray<UPaperFlipbook*> NextFlipBook;
	if(!bIsEquipped)
	{
		return;
	}
	
	if(bIsMoving)
	{
		NextFlipBook = FB_Walk;
	}
	else
	{
		NextFlipBook = FB_Idle;
	}

	if (!NextFlipBook.IsEmpty() && FlipBookComponent != nullptr)
	{
		switch (Direction)
		{
		case EDirectionFacing::UP:
			FlipBookComponent->SetFlipbook(NextFlipBook[0]);
			break;
		case EDirectionFacing::DOWN:
			FlipBookComponent->SetFlipbook(NextFlipBook[1]);
			break;
		case EDirectionFacing::RIGHT:
			FlipBookComponent->SetFlipbook(NextFlipBook[2]);
			break;
		case EDirectionFacing::LEFT:
			FlipBookComponent->SetFlipbook(NextFlipBook[3]);
			break;
		}
	}
}

void AGun::RotateTowardsMouse(const FVector2D& MouseLocation)
{
	FVector PlayerLocation = Owner->GetActorLocation();
	FVector2D PlayerLocation2D(PlayerLocation.X, PlayerLocation.Y);

	FVector2D Direction = MouseLocation - PlayerLocation2D;
	float Angle = FMath::Atan2(Direction.Y, Direction.X) * (180.f / PI);

	SetActorRotation(FRotator(0.0f, Angle, 0.0f));
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
