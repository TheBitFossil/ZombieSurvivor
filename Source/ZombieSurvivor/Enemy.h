// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "TopDownCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"

UENUM()
enum class EState : uint8
{
	IDLE,
	CHASING,
	ATTACKING,
	DEAD
};


UCLASS()
class ZOMBIESURVIVOR_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbookComponent> FlipBookComponent;

	// UP/ DOWN / RIGHT / LEFT
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Attack;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Death;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<ATopDownCharacter> PlayerTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAlive{true};
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed{5.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StoppingDistance{20.f};

	UPROPERTY(VisibleAnywhere)
	float DistanceToTarget{};

	UPROPERTY(VisibleAnywhere)
	FVector Direction{};
	
	UPROPERTY(VisibleAnywhere)
	EDirectionFacing DirectionFacing {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EState CurrentState;
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	EDirectionFacing CalculateFacingDirection();
	
	UFUNCTION()
	void ChasePlayer(float DeltaTime);
	
	UFUNCTION()
	void UpdateFlipBookAnim(const EState& State, const EDirectionFacing& Facing);
};
