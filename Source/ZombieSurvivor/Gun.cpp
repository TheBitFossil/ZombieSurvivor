// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Bullet.h"
#include "TopDownCharacter.h"
#include "Kismet/KismetMathLibrary.h"

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

void AGun::Shoot(const EDirectionFacing& Facing, const FVector& TargetPosition)
{
	if(bCanShoot)
	{
		bCanShoot = false;
		int8 idx = static_cast<int8>(Facing);

		// Where to spawn and how to rotate the bullet
		FVector SpawnLocation = GunMuzzles[idx]->GetComponentLocation();
		FRotator SpawnRotation = GunMuzzles[idx]->GetComponentRotation();

		// Aim Bullet at Target, Rotate the Muzzle 
		if(TargetPosition != FVector::Zero())
		{
			SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetPosition);
		}
		
		if(BulletClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Use a TSubClass of for a UObject to be spawned
			ABullet* NewBullet = GetWorld()->SpawnActor<ABullet>
				(BulletClass, SpawnLocation, SpawnRotation, SpawnParams);

			// Assertion: crashes the game if the bullet is not working
			// Development Code
			check(NewBullet);
			
			if(NewBullet)
			{
				NewBullet->InitStats(SpawnRotation, BulletSpeed);
			}
			GetWorldTimerManager().SetTimer(FireRateTimer, this, &AGun::OnFireRateTimerTimeOut,
				1.f, false, FireRate);

			// Reset Auto Aim Rotation
			SpawnRotation = GunMuzzles[idx]->GetComponentRotation();
		}
	}
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::OnFireRateTimerTimeOut()
{
	bCanShoot = true;
}
