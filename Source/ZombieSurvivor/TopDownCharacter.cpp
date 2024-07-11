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
	
}

void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(APlayerController* PC = static_cast<APlayerController*>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Default, 0);
		}		
	}
}

void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATopDownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* InputComponent = static_cast<UEnhancedInputComponent*>(PlayerInputComponent))
	{
		InputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATopDownCharacter::Move);
	}
	
}

void ATopDownCharacter::Move(const FInputActionValue& Value)
{
	const FVector2d InputAction = Value.Get<FVector2d>();

	if(InputAction.Length() != 0.f)
	{
		// Check direction we are moving in
		FVector2d Direction = InputAction;
		// Add movement in this direction
		FVector2d Velocity = Direction * GetWorld()->GetDeltaSeconds() * MoveSpeed;
		FVector Location = GetActorLocation();
		FVector NewLocation = Location + FVector(Velocity.X, 0.f, Velocity.Y);
		
		SetActorLocation(NewLocation);
	}
}

