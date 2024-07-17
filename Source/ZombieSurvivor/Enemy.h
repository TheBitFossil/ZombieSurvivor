// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperSpriteComponent> TargetMarker;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAlive{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health{50.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed{5.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StoppingDistance{20.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DespawnTime{10.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsTarget{false};
	
public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void SetIsTarget(bool Value);

	UFUNCTION()
	bool GetIsTarget() const {return bIsTarget;}
	
private:
	UPROPERTY(VisibleAnywhere)
	float DistanceToTarget{};

	UPROPERTY(VisibleAnywhere)
	FVector NormalizedDirectionToPlayer{};
	
	UPROPERTY(VisibleAnywhere)
	EDirectionFacing DirectionFacing{};

	UPROPERTY(VisibleAnywhere)
	EState CurrentState;

	UPROPERTY()
	FTimerHandle DespawnTimerHandle;
	
	UFUNCTION()
	EDirectionFacing CalculateFacingDirection();
	
	UFUNCTION()
	void ChasePlayer(const float& Delta);
	
	UFUNCTION()
	void UpdateFlipBookAnim(const EState& State, const EDirectionFacing& Facing);

	UFUNCTION()
	void OnDespawnTimerTimeout();

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
};
