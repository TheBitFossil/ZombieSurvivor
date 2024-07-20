// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class ABullet;
class ATopDownCharacter;

UCLASS()
class ZOMBIESURVIVOR_API AGun : public AActor
{
	GENERATED_BODY()

public:
	AGun();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> FlipBookComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Walk;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Weapon)
	TArray<TObjectPtr<USceneComponent>> GunMuzzles;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TSubclassOf<ABullet> BulletClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsEquipped {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	float BulletSpeed {200.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	float FireRate {.95f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sound)
	TObjectPtr<USoundBase> ShootSound;
	
	FTimerHandle FireRateTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanShoot{true};
	
	UFUNCTION()
	void SetAnimation(const bool bIsMoving, const EDirectionFacing& Direction) const;
	
	UFUNCTION()
	void Shoot(const EDirectionFacing& Facing, const FVector& TargetPosition);
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnFireRateTimerTimeOut();
	
};
