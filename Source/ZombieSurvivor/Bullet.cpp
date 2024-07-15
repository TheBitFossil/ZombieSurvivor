// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	SetRootComponent(CapsuleComponent);
	
	ProjectileMovementComponent =  CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	FlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipBook"));
	FlipBookComponent->SetupAttachment(RootComponent);
}

void ABullet::InitStats(const FRotator& Rotation, const float& MoveSpeed)
{
	if(bIsLaunched)
	{
		return;
	}
	bIsLaunched = true;
	FlipBookComponent->SetWorldRotation(Rotation);
	ProjectileMovementComponent->InitialSpeed = MoveSpeed;
	
	GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &ABullet::OnLifeTimerTimeOut,
		1.f, false, LifeTime);
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ProjectileMovementComponent->bAllowConcurrentTick = bIsLaunched;
}

void ABullet::OnLifeTimerTimeOut()
{
	Destroy(this);
}

