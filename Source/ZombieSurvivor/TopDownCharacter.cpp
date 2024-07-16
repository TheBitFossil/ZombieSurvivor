// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Gun.h"
#include "Kismet/KismetMathLibrary.h"


ATopDownCharacter::ATopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Comp"));
	SetRootComponent(CapsuleComponent);

	FlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbBook Comp"));
	FlipBookComponent->SetupAttachment(RootComponent);
	FlipBookComponent->SetTranslucentSortPriority(10);
	
	GunChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Gun"));
	GunChildActor->SetupAttachment(FlipBookComponent);
	if(GunClass)
	{
		GunChildActor->SetChildActorClass(AGun::StaticClass());
	}

	Marker = CreateDefaultSubobject<USceneComponent>(TEXT("Marker"));
	Marker->SetupAttachment(RootComponent);
	MarkerSprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("MarkerSprite"));
	MarkerSprite->SetupAttachment(Marker);
}

void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = static_cast<APlayerController*>(Controller))
	{
		PC->SetShowMouseCursor(true);
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Default, 0);
		}
	}
}

EDirectionFacing ATopDownCharacter::CalculateFacingDirection(const FVector2D& Value)
{
	// Setting the scale to flip the Sprite
	// FVector Scale = FlipBook->GetComponentScale();
	// if(Scale.X > 0.f)
	// FlipBook->SetWorldScale3D(FVector(-1.f, 1.f, 1.f));
	
	if (Value.X > 0.f)
	{
		DirectionFacing = EDirectionFacing::RIGHT;
	}
	else if (Value.X < 0.f)
	{
		DirectionFacing = EDirectionFacing::LEFT;
	}
	else if (Value.Y > 0.f)
	{
		DirectionFacing = EDirectionFacing::UP;
	}
	else if (Value.Y < 0.f)
	{
		DirectionFacing = EDirectionFacing::DOWN;
	}

	return DirectionFacing;
}

void ATopDownCharacter::ChangeFlipBookAnimation(const bool bEquipped)
{
	// Either Running or Idling
	TArray<UPaperFlipbook*> NextFlipBook;
	if (Direction.Length() != 0.f)
	{
		NextFlipBook = bEquipped ? FB_Shoot_Walk : FB_Walk;
	}
	else
	{
		NextFlipBook = bEquipped ? FB_Shoot_Idle : FB_Idle;
	}

	if (!NextFlipBook.IsEmpty() && FlipBookComponent != nullptr)
	{
		switch (CalculateFacingDirection(Direction))
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

void ATopDownCharacter::UpdateGunAnimation(bool bEquipped)
{
	if(bEquipped)
	{
		if(AGun* Gun = static_cast<AGun*>(GunChildActor->GetChildActor()))
		{
			Gun->SetAnimation(GetDirection().Length() != 0.f, GetDirectionFacing());
		}
	}
}

bool ATopDownCharacter::IsInMapBoundsHorizontal(float XPos)
{
	return XPos > HorizontalMapBounds.X && XPos < HorizontalMapBounds.Y;
}

bool ATopDownCharacter::IsInMapBoundsVertical(float ZPos)
{
	return ZPos < VerticalMapBounds.X && ZPos > VerticalMapBounds.Y;
}

void ATopDownCharacter::CalculateNextLocation(FVector& NewLocation)
{
	FVector2d Velocity = Direction * MoveSpeed * GetWorld()->GetDeltaSeconds();

	FVector Location = GetActorLocation();
	NewLocation = Location + FVector(Velocity.X, 0.f, 0.f);
	if(!IsInMapBoundsHorizontal(NewLocation.X))
	{
		NewLocation -= FVector(Velocity.X, 0.f, 0.f);		// Remove any Horizontal Movement
	}

	NewLocation += FVector(0.f, 0.f, Velocity.Y);	// Add Vertical Movement
	if(!IsInMapBoundsVertical(NewLocation.Z))
	{
		NewLocation -= FVector(0.f, 0.f, Velocity.Y);
	}
}
void ATopDownCharacter::SetTraceDirection(FVector& StartLocation, FVector& EndPoint)
{
	// All Targets in Facing Direction
	FVector TraceDirection {};
	switch (DirectionFacing)
	{
	case EDirectionFacing::UP:
		TraceDirection = GetActorUpVector();
		break;
	case EDirectionFacing::DOWN:
		TraceDirection = GetActorUpVector() * -1.f;
		break;
	case EDirectionFacing::RIGHT:
		TraceDirection = GetActorForwardVector();
		break;
	case EDirectionFacing::LEFT:
		TraceDirection = GetActorForwardVector() * -1.f;
		break;
	}

	StartLocation = GetActorLocation();
	EndPoint = StartLocation + TraceDirection * 100.f;
	DrawDebugLine(GetWorld(), StartLocation, EndPoint, FColor::Magenta, false, .12f);
}
void ATopDownCharacter::TraceForClosestTargetInDirection(FVector StartLocation, FVector EndPoint)
{
	// Trace for Targets in Facing direction
	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
		
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(CollisionShapeRadius);
		
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		StartLocation,
		EndPoint,
		FQuat::Identity,
		ECC_WorldDynamic,
		SphereShape,
		CollisionQueryParams
	);
		
	// Get Closest Target
	if(bHit)
	{
		const AActor* ClosestTarget = nullptr;
		float MinDistance = FLT_MAX;

		for(const FHitResult& Hit : HitResults)
		{
			const AActor* HitActor = Hit.GetActor();
			if(HitActor)
			{
				const float Distance = FVector::Dist(StartLocation, Hit.ImpactPoint);
				if(Distance < MinDistance)
				{
					MinDistance = Distance;
					ClosestTarget = HitActor;
				}
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, SphereShape.GetSphereRadius(), 12, FColor::Red, false, 1.f);
			}
		}

		if(ClosestTarget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green,
			                                 FString::Printf(TEXT("Closest Target: %s"), *ClosestTarget->GetName()));
		}
	}
	else
	{
		DrawDebugSphere(GetWorld(), EndPoint, SphereShape.GetSphereRadius(), 4, FColor::White, false, .3f);
	}
}

void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateMousePositionInWorld();
	
	if (bCanMove)
	{
		if (Direction.Length() != 0.f)
		{
			// Check if we need to normalize due to incorrect speed of two directions
			if (Direction.Length() > 1.f)
			{
				Direction.Normalize();
			}

			FVector NewLocation;
			CalculateNextLocation(NewLocation);
			SetActorLocation(NewLocation);
		}
		
		ChangeFlipBookAnimation(bHasGunEquipped);
		UpdateGunAnimation(bHasGunEquipped);

		FVector StartLocation;
		FVector EndPoint;
		SetTraceDirection(StartLocation, EndPoint);

		TraceForClosestTargetInDirection(StartLocation, EndPoint);
	}
}

void ATopDownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* InputComponent = static_cast<UEnhancedInputComponent*>(PlayerInputComponent))
	{
		InputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATopDownCharacter::MoveTriggered);
		InputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &ATopDownCharacter::MoveCompleted);
		InputComponent->BindAction(IA_Shoot, ETriggerEvent::Started, this, &ATopDownCharacter::Shoot);
		InputComponent->BindAction(IA_EquipGun, ETriggerEvent::Started, this, &ATopDownCharacter::EquipGun);
	}
}

void ATopDownCharacter::MoveTriggered(const FInputActionValue& Value)
{
	const FVector2d InputAction = Value.Get<FVector2d>();

	if (bCanMove)
	{
		// Base for all calculations
		Direction = InputAction;
	}
}

void ATopDownCharacter::MoveCompleted(const FInputActionValue& Value)
{
	//const FVector2d InputAction = Value.Get<FVector2d>();
	Direction = FVector2D::Zero();
}

void ATopDownCharacter::Shoot(const FInputActionValue& Value)
{
	if(!bHasGunEquipped)
	{
		return;
	}
	
	if(AGun* Gun = static_cast<AGun*>(GunChildActor->GetChildActor()))
	{
		Gun->Shoot(GetDirectionFacing());
	}
	// Subtract Bullets ?
	// Play Animation/VFX ?
}

void ATopDownCharacter::EquipGun()
{
	bHasGunEquipped = !bHasGunEquipped;
	if(APlayerController* PC = static_cast<APlayerController*>(Controller))
	{
		if(bHasGunEquipped)
		{
			PC->CurrentMouseCursor = EMouseCursor::Type::Crosshairs;
		}
		else
		{
			PC->CurrentMouseCursor = EMouseCursor::Type::Default;
		}
	}
	
	if(AGun* Gun = static_cast<AGun*>(GunChildActor->GetChildActor()))
	{
		Gun->SetOwner(this);
		Gun->bIsEquipped = bHasGunEquipped;
		Gun->SetActorHiddenInGame(!bHasGunEquipped);
	}
}

void ATopDownCharacter::CalculateMousePositionInWorld()
{
	if(APlayerController* PC = static_cast<APlayerController*>(Controller))
	{
		FVector WorldLocation, WorldDirection;
		bool MouseWorldPos = PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		FVector Location = GetActorLocation();
		FVector Start = FVector(Location.X, 0.f, Location.Z);
		FVector Target = FVector(WorldLocation.X, 0.f, WorldLocation.Z);
		// Only use 2D Vector for Rotation
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);

		Marker->SetRelativeRotation(Rotation);
	}
}
