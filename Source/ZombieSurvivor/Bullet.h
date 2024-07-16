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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime{2.f};

	UPROPERTY()
	FTimerHandle LifeTimerHandle;

	UPROPERTY(VisibleAnywhere)
	bool bIsLaunched {false};

	UPROPERTY(VisibleAnywhere)
	bool bIsDisabled {false};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletDamage{10.f};
	
	UFUNCTION()
	void OnLifeTimerTimeOut();

	UFUNCTION()
	void InitStats(const FRotator& Rotation,const float& MoveSpeed);
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DisableBullet();
	
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
