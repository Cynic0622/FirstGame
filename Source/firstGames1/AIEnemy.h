// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIEnemy.generated.h"

UCLASS()
class FIRSTGAMES1_API AAIEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(EditAnywhere, Category = "AI")
    float MoveSpeed;

    // Attack range
    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackRange;

    // Reference to the player character
    ACharacter* PlayerCharacter;

    // Distance to player
    float DistanceToPlayer;

    // Function to find the player character
    void FindPlayerCharacter();

    // Function to move towards the player
    void MoveTowardsPlayer();

    // Function to attack the player
    void AttackPlayer();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    UAnimMontage* AttackMontage;
};
