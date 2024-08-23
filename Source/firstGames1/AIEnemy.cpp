// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"


#include "firstGames1Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAIEnemy::AAIEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MoveSpeed = 500.f;
	AttackRange = 100.f;
    Health = 100.f;
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

float AAIEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (DamageSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DamageSound, GetActorLocation());
	}
    Health -= DamageAmount;
	if (Health <= 0.f) {
		Destroy();
		// CanAttack = false;
	}
    return DamageAmount;
}

void AAIEnemy::FindPlayerCharacter()
{
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AAIEnemy::MoveTowardsPlayer()
{
    if (PlayerCharacter && CanAttack)
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
	if (CanAttack) {
		// UE_LOG(LogTemp, Warning, TEXT("Attacking player!"));
		FVector DirectionToPlayer = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
		SetActorRotation(FQuat(NewRotation));
		if (AttackMontage && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
		{
			// Play the attack montage if it's not already playing
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
			SphereTrace();
			if (AttackSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
			}
		}
	}
    
}
void AAIEnemy::SphereTrace()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 20.f; // ��������� End Ϊ�� Start ��ͬ��λ������Ӧ��Ҫ
	float Radius = 100.0f; // ����뾶
	FHitResult HitResult;

	// ������ײ��ѯ����
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // ���Ե�ǰActor

	// ִ��������ײ���
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,                   // Hit result
		Start,                      // ��ʼλ��
		End,
		FQuat::Identity,// ����λ�ã������� Start + Direction * Distance��
		// ����뾶
		ECC_Pawn,                   // ��ײͨ��
		FCollisionShape::MakeSphere(Radius),
		QueryParams                 // ��ײ��ѯ����
	);

	if (bHit)
	{
		// ������ײ���
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->IsA(ACharacter::StaticClass()))
		{
			// UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
			UGameplayStatics::ApplyDamage(HitActor, 10.0f, GetController(), this, UDamageType::StaticClass());
			auto t = Cast<AfirstGames1Character>(HitActor);
			if (t->Health <= 0.f) CanAttack = false;
		}

		// ��ѡ�����Ƶ�����Ϣ
		// DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, Radius, 12, FColor::Red, false, 1.0f);
	}
}

