// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "GameModeTopDown.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class ZOMBIESURVIVOR_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY()
	FTimerHandle SpawnTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnRate{2.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnDistance{200.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DifficultySpikeInterval{10};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnRateMinimumLimit{0.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecreaseSpawnRateByEveryInterval{0.05f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<ATopDownCharacter> PlayerTarget;

	UFUNCTION(BlueprintCallable)
	void StartSpawnTimer();

	UFUNCTION(BlueprintCallable)
	void StopSpawnTimer();

private:
	UPROPERTY()
	TObjectPtr<AGameModeTopDown> GameModeTopDown;

	UPROPERTY(VisibleAnywhere)
	TArray<AEnemy*> SpawnedEnemies {nullptr};
	
	UFUNCTION()
	void OnEnemySpawnTimerTimeout();
	void DifficultySettings(const int& Count);

	UFUNCTION()
	void OnEnemyDied();
	
	UFUNCTION()
	void SpawnEnemy();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPlayerDeath();
};
