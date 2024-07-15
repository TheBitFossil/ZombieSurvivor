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

void ATopDownCharacter::ChangeFlipBookAnimation(bool bEquipped)
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

			FVector2d Velocity = Direction * MoveSpeed * GetWorld()->GetDeltaSeconds();

			FVector Location = GetActorLocation();
			FVector NewLocation = Location + FVector(Velocity.X, 0.f, 0.f);
			if(!IsInMapBoundsHorizontal(NewLocation.X))
			{
				NewLocation -= FVector(Velocity.X, 0.f, 0.f);		// Remove any Horizontal Movement
			}

			NewLocation += FVector(0.f, 0.f, Velocity.Y);	// Add Vertical Movement
			if(!IsInMapBoundsVertical(NewLocation.Z))
			{
				NewLocation -= FVector(0.f, 0.f, Velocity.Y);
			}
			// Finally set the corrected location Value
			SetActorLocation(NewLocation);
		}
		ChangeFlipBookAnimation(bHasGunEquipped);
		UpdateGunAnimation(bHasGunEquipped);
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
		//InputComponent->BindAction(IA_Shoot, ETriggerEvent::Triggered, this, &ATopDownCharacter::Shoot);

		InputComponent->BindAction(IA_EquipGun, ETriggerEvent::Started, this, &ATopDownCharacter::EquipGun);
		//InputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ATopDownCharacter::Aim);
	}
}

void ATopDownCharacter::MoveTriggered(const FInputActionValue& Value)
{
	const FVector2d InputAction = Value.Get<FVector2d>();

	if (bCanMove)
	{
		// Check direction we are moving in
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
	// Get Gun and call its shoot
	if(AGun* Gun = static_cast<AGun*>(GunChildActor->GetChildActor()))
	{
		Gun->Shoot(GetDirectionFacing());
	}
	// Subtract Bullets ?
	// Cooldown ?
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
