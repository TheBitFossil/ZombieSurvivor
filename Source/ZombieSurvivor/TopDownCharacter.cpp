// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownCharacter.h"

#include "Enemy.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Gun.h"
#include "KismetTraceUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


ATopDownCharacter::ATopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Comp"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ATopDownCharacter::OverlapBegin);
	
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

void ATopDownCharacter::ChangeFlipBookAnimation(const EPlayerStates& EStates)
{
	// Pointer -> Possible Array Pointers
	const TArray<TObjectPtr<UPaperFlipbook>>* NextFlipBookArray = nullptr;

	switch (EStates)
	{
	case EPlayerStates::IDLE_UNARMED:
		NextFlipBookArray = &FB_Idle;
		break;
	case EPlayerStates::IDLE_GUN:
		NextFlipBookArray = &FB_Shoot_Idle;
		break;
	case EPlayerStates::WALKING_UNARMED:
		NextFlipBookArray = &FB_Walk;
		break;
	case EPlayerStates::WALKING_GUN:
		NextFlipBookArray = &FB_Shoot_Walk;
		break;
	case EPlayerStates::DEAD:
		NextFlipBookArray = &FB_Death;
		break;
	}
	
	// Animation to play
	UPaperFlipbook* NextFlipBook  = nullptr;

	// Choose correct facing from our Array
	if(NextFlipBookArray)
	{
		uint32 Idx = static_cast<uint32>(DirectionFacing);
		if(Idx < NextFlipBookArray->Num())
		{
			NextFlipBook = (*NextFlipBookArray)[Idx];
		}
	}

	FlipBookComponent->SetFlipbook(NextFlipBook);
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

	NewLocation += FVector(0.f, 0.f, Velocity.Y);			// Add Vertical Movement
	if(!IsInMapBoundsVertical(NewLocation.Z))
	{
		NewLocation -= FVector(0.f, 0.f, Velocity.Y);
	}
}

void ATopDownCharacter::SetTraceDirection()
{
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
}

void ATopDownCharacter::TraceForClosestTargetInDirection()
{
	const FVector StartLocation = GetActorLocation() + (TraceDirection * TraceOffset);
	const FVector EndPoint = StartLocation + (TraceDirection * TraceDistance);
	if(DebugEnabled)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndPoint, FColor::Yellow, false, .2f);
	}
	
	// Trace for Targets in Facing direction
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
		
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(CollisionShapeRadius);
		
	bool Results = GetWorld()->SweepMultiByChannel(
		HitResults,
		StartLocation,
		EndPoint,
		FQuat::Identity,
		ECC_Visibility,
		SphereShape,
		CollisionQueryParams
	);
	
	if(DebugEnabled)
	{
		DrawDebugSweptSphere(GetWorld(), StartLocation, EndPoint, SphereShape.GetSphereRadius(), FColor::Yellow, false, .2f);
	}
	
	// Get Closest Target
	if(Results)
	{
		float MinDistance = FLT_MAX;

		for(const FHitResult& Hit : HitResults)
		{
			if(AEnemy* Enemy = static_cast<AEnemy*>(Hit.GetActor()))
			{
				//Ignore Dead enemies
				if(!Enemy->bIsAlive)
				{
					continue;
				}
				
				Enemy->SetIsTarget(false);
				
				const float Distance = FVector::Dist(StartLocation, Hit.ImpactPoint);
				if(Distance < MinDistance)
				{
					MinDistance = Distance;
					ClosestTarget = Enemy;
				}
			}
		}

		if(ClosestTarget)
		{
			ClosestTarget->SetIsTarget(true);

			/*GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green,
				FString::Printf(TEXT("Closest Target: %s"), *ClosestTarget->GetName())); */
		}
	}
	else
	{
		ResetTarget();
	}
}

void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CalculateMousePositionInWorld();

	if (bIsAlive)
	{
		if (Direction.Length() != 0.f)
		{
			CalculateFacingDirection(Direction);
			
			// Check if we need to normalize due to incorrect speed of two directions
			if (Direction.Length() > 1.f)
			{
				Direction.Normalize();
			}

			States = bHasGunEquipped ? States = EPlayerStates::WALKING_GUN : States = EPlayerStates::WALKING_UNARMED;
			
			FVector NewLocation;
			CalculateNextLocation(NewLocation);
			SetActorLocation(NewLocation);
		}
		else
		{
			States = bHasGunEquipped ? States = EPlayerStates::IDLE_GUN : States = EPlayerStates::IDLE_UNARMED;
		}
	}
	else
	{
		States = EPlayerStates::DEAD;
		FlipBookComponent->SetLooping(false);
	}

	ChangeFlipBookAnimation(States);
	UpdateGunAnimation(bHasGunEquipped);

	SetTraceDirection();
	if(bHasGunEquipped)
	{
		TraceForClosestTargetInDirection();
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

	if (bIsAlive)
	{
		// Base for most calculations
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
	if(!bHasGunEquipped || !bIsAlive)
	{
		return;
	}
	
	if(AGun* Gun = static_cast<AGun*>(GunChildActor->GetChildActor()))
	{
		if(ClosestTarget)
		{
			const FVector TargetLocation = ClosestTarget->GetActorLocation();
			const FVector TargetLocation2D = FVector(TargetLocation.X, 0.f, TargetLocation.Z);
			Gun->Shoot(GetDirectionFacing(), TargetLocation2D);
		}
		else
		{
			Gun->Shoot(GetDirectionFacing(), FVector::Zero());
		}

		ResetTarget();
	}
}

void ATopDownCharacter::ResetTarget()
{
	if(ClosestTarget)
	{
		ClosestTarget->SetIsTarget(false);
		ClosestTarget = nullptr;
	}
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

	ResetTarget();
	
	if(AGun* Gun = static_cast<AGun*>(GunChildActor->GetChildActor()))
	{
		Gun->SetOwner(this);
		Gun->bIsEquipped = bHasGunEquipped;
		Gun->SetActorHiddenInGame(!bHasGunEquipped);
	}
}

void ATopDownCharacter::CalculateMousePositionInWorld()
{
	// Deprecated only here for future ref
	if(APlayerController* PC = static_cast<APlayerController*>(Controller))
	{
		FVector WorldLocation, WorldDirection;
		bool MouseWorldPos = PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		const FVector Location = GetActorLocation();
		const FVector Start = FVector(Location.X, 0.f, Location.Z);
		const FVector Target = FVector(WorldLocation.X, 0.f, WorldLocation.Z);
		// Only use 2D Vector for Rotation
		const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);

		Marker->SetRelativeRotation(Rotation);
	}
}

void ATopDownCharacter::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor && OtherActor != this)
	{
		if(const AEnemy* Enemy = static_cast<AEnemy*>(OtherActor))
		{
			if(Enemy->bIsAlive)
			{
				bIsAlive = false;
				
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
				OnPlayerDeath.Broadcast();
			}
		}
	}
}