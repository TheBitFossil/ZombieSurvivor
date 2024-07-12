// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "InputMappingContext.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "TopDownCharacter.generated.h"

UENUM()
enum class EDirectionFacing : uint8
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

UCLASS()
class ZOMBIESURVIVOR_API ATopDownCharacter : public APawn
{
	GENERATED_BODY()

public:
	ATopDownCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> FlipBookComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> GunChildActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> IA_Shoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> IA_EquipGun;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	float MoveSpeed {200.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	bool bCanMove {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	bool bHasGun {false};
	
	// UP/ DOWN / RIGHT / LEFT
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Shoot_Idle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Shoot_Walk;
	
	virtual void BeginPlay() override;

	void ChangeFlipBookAnimation(bool HasGun);

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	EDirectionFacing GetDirectionFacing() const { return DirectionFacing; }

	UFUNCTION()
	FVector2D GetDirection() const { return Direction; }

private:
	UPROPERTY()
	FVector2D Direction {};

	UPROPERTY()
	EDirectionFacing DirectionFacing {};

	UFUNCTION()
	EDirectionFacing CalculateFacingDirection(const FVector2D& Value);
	
	UFUNCTION()
	void MoveTriggered(const FInputActionValue& Value);

	UFUNCTION()
	void MoveCompleted(const FInputActionValue& Value);

	UFUNCTION()
	void Shoot(const FInputActionValue& Value);

	UFUNCTION()
	void EquipGun();
	
	
};
