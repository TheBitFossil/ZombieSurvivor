// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Bullet.h"
#include "TopDownCharacter.h"

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;

	FlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbBook Comp"));
	SetRootComponent(FlipBookComponent);

	// Initialize Array size
	GunMuzzles.SetNum(4);

	// Create and Attach Components
	static const TCHAR* GunMuzzleNames[] =
	{
		TEXT("MuzzleUp"),	
		TEXT("MuzzleDown"),	
		TEXT("MuzzleLeft"),	
		TEXT("MuzzleRight"),	
	};

	for (int8 i = 0; i < GunMuzzles.Num(); ++i)
	{
		GunMuzzles[i] = CreateDefaultSubobject<USceneComponent>(GunMuzzleNames[i]);
		GunMuzzles[i]->SetupAttachment(RootComponent);
	}
	
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
}

void AGun::SetAnimation(const bool bIsMoving, const EDirectionFacing& Direction) const
{
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

void AGun::Shoot(const EDirectionFacing& Facing)
{
	int8 idx = static_cast<int8>(Facing);

	FVector SpawnLocation = GunMuzzles[idx]->GetComponentLocation();
	FRotator SpawnRotation = GunMuzzles[idx]->GetComponentRotation();
	// Spawn Bullet
	if(BulletClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		ABullet* NewBullet = GetWorld()->SpawnActor<ABullet>
			(BulletClass, SpawnLocation, SpawnRotation, SpawnParams);
		if(NewBullet)
		{
			NewBullet->FlipBookComponent->SetWorldRotation(SpawnRotation);
		}
	}
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
