// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "Enemy.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"


ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OverlapBegin);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

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

void ABullet::DisableBullet()
{
	if(bIsDisabled)
	{
		return;
	}

	bIsDisabled = true;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipBookComponent->DestroyComponent();
}

void ABullet::OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AEnemy* Enemy = static_cast<AEnemy*>(Other))
	{
		if(!Enemy->bIsAlive)
		{
			return;
		}
		
		FDamageEvent DamageEvent;
		Enemy->TakeDamage(BulletDamage, DamageEvent, nullptr, this);

		DisableBullet();
	}
}

void ABullet::OnLifeTimerTimeOut()
{
	Destroy(this);
}