// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<UInputAction> IA_Shoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	float MoveSpeed {200.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	bool bCanMove {true};

	// UP/ DOWN / RIGHT / LEFT
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Run;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Shoot;
	
	virtual void BeginPlay() override;
	void ChangeFlipBookAnimation();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY()
	FVector2D Direction {};

	UPROPERTY()
	EDirectionFacing DirectionFacing {};
	
	UFUNCTION()
	void MoveTriggered(const FInputActionValue& Value);

	UFUNCTION()
	void MoveCompleted(const FInputActionValue& Value);

	UFUNCTION()
	void Shoot(const FInputActionValue& Value);

	UFUNCTION()
	EDirectionFacing GetDirectionFacing(const FVector2D& Value);

};
