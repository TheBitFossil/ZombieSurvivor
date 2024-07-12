// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


ATopDownCharacter::ATopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Comp"));
	SetRootComponent(CapsuleComponent);

	FlipBookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbBook Comp"));
	FlipBookComponent->SetupAttachment(RootComponent);
	FlipBookComponent->SetTranslucentSortPriority(10);

	GunChildActor->CreateDefaultSubobject<UChildActorComponent>(TEXT("Gun"));
	GunChildActor->SetupAttachment(RootComponent);
}

void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = static_cast<APlayerController*>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Default, 0);
		}
	}
}

EDirectionFacing ATopDownCharacter::CalculateFacingDirection(const FVector2D& Value)
{
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

void ATopDownCharacter::ChangeFlipBookAnimation(bool HasGun)
{
	// Either Running or Idling
	TArray<UPaperFlipbook*> NextFlipBook;
	if (Direction.Length() != 0.f)
	{
		NextFlipBook = HasGun ? FB_Shoot_Walk : FB_Walk;
	}
	else
	{
		NextFlipBook = HasGun ? FB_Shoot_Idle : FB_Idle;
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

void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			FVector NewLocation = Location + FVector(Velocity.X, 0.f, Velocity.Y);

			SetActorLocation(NewLocation);
		}

		ChangeFlipBookAnimation(bHasGun);
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
		InputComponent->BindAction(IA_Shoot, ETriggerEvent::Triggered, this, &ATopDownCharacter::Shoot);

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
	const FVector2d InputAction = Value.Get<FVector2d>();

	Direction = FVector2D::Zero();
}

void ATopDownCharacter::Shoot(const FInputActionValue& Value)
{
	// Get Facing Direction
	// Play Animation
}

void ATopDownCharacter::EquipGun()
{
	bHasGun = !bHasGun;
	if(AGun* Gun = static_cast<AGun*>(GunChildActor->GetChildActor()))
	{
		Gun->SetOwner(this);
		Gun->bIsEquipped = bHasGun;
	
	}
}