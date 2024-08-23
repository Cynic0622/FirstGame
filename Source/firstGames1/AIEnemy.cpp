// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"


#include "Kismet/GameplayStatics.h"

// Sets default values
AAIEnemy::AAIEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MoveSpeed = 500.f;
	AttackRange = 100.f;
    
}

// Called when the game starts or when spawned
void AAIEnemy::BeginPlay()
{
	Super::BeginPlay();
	FindPlayerCharacter();
}

// Called every frame
void AAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (PlayerCharacter)
    {
        DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

        if (DistanceToPlayer <= AttackRange)
        {
            AttackPlayer();
        }
        else
        {
            MoveTowardsPlayer();
        }
    }
}

// Called to bind functionality to input
void AAIEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIEnemy::FindPlayerCharacter()
{
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AAIEnemy::MoveTowardsPlayer()
{
    if (PlayerCharacter)
    {
        // Calculate direction to the player
        FVector DirectionToPlayer = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();

        // Set the actor's rotation to face the player
        /*FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
        SetActorRotation(FQuat(NewRotation));*/

        // Move in the direction of the player
        AddMovementInput(DirectionToPlayer, MoveSpeed * GetWorld()->GetDeltaSeconds());
    }
}

void AAIEnemy::AttackPlayer()
{
    UE_LOG(LogTemp, Warning, TEXT("Attacking player!"));
    FVector DirectionToPlayer = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
	SetActorRotation(FQuat(NewRotation));
    if (AttackMontage && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
    {
        // Play the attack montage if it's not already playing
        GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
    }
    
}

