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
	
	MoveDirection = FVector2D(1.f, 0.f);
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector2D Velocity = MoveDirection * MoveSpeed * DeltaTime;
	
	const FVector Location = GetActorLocation();
	const FVector NewLocation = Location + FVector(Velocity.X, 0.f, Velocity.Y);
	
	SetActorLocation(NewLocation);
}

	


