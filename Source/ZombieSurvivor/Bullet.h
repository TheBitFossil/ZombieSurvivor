// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class UPaperFlipbookComponent;
class UCapsuleComponent;
class UProjectileMovementComponent; 

UCLASS()
class ZOMBIESURVIVOR_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	ABullet();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent>  FlipBookComponent;

	UPROPERTY(BlueprintReadWrite)
	FVector2D MoveDirection{};

	UPROPERTY(BlueprintReadWrite)
	float MoveSpeed{};
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
