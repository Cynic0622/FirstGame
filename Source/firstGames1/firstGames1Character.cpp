// Copyright Epic Games, Inc. All Rights Reserved.

#include "firstGames1Character.h"
// #include "DrawDebugHelpers.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AfirstGames1Character

AfirstGames1Character::AfirstGames1Character()
{
	this->Tags.Add(FName("Player"));
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	Health = 100.f;
}

void AfirstGames1Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AfirstGames1Character::Attack()
{
	if (CanAttack)
	{
		CanAttack = false;
		if (AttackMontage && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
		{
			// Play the attack montage if it's not already playing
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
			SphereTrace();
			if (AttackSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
			}
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AfirstGames1Character::ResetAttack, 1.0f, false);

		}
	}
	
}

void AfirstGames1Character::ResetAttack()
{
	CanAttack = true;
}

float AfirstGames1Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                        AController* EventInstigator, AActor* DamageCauser)
{
	
	Health -= DamageAmount;
	if (Health <= 0.f)
	{
		Destroy();
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
	}
	if (DamageSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DamageSound, GetActorLocation());
	}
	if (DamageCauser)
	{
		FVector DamageCauserLocation = DamageCauser->GetActorLocation();
		FVector DirectionToMove = GetActorLocation() - DamageCauserLocation;
		DirectionToMove.Normalize(); // 确保方向向量是单位向量

		// 定义退后步伐的距离
		float BackwardStepDistance = 200.0f; // 调整为你需要的距离

		// 计算新位置
		FVector NewLocation = GetActorLocation() + (DirectionToMove * BackwardStepDistance);

		// 设置主角的新位置
		SetActorLocation(NewLocation);
	}
	return DamageAmount;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AfirstGames1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AfirstGames1Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AfirstGames1Character::Look);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AfirstGames1Character::Attack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AfirstGames1Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AfirstGames1Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AfirstGames1Character::SphereTrace()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 1000.f; // 你可以设置 End 为与 Start 不同的位置以适应需要
	float Radius = 50.0f; // 球体半径
	FHitResult HitResult;

	// 设置碰撞查询参数
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 忽略当前Actor

	// 执行球体碰撞检测
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,                   // Hit result
		Start,                      // 起始位置
		End,
		FQuat::Identity,// 结束位置（可以是 Start + Direction * Distance）
		                    // 球体半径
		ECC_WorldDynamic,                   // 碰撞通道
		FCollisionShape::MakeSphere(Radius),
		QueryParams                 // 碰撞查询参数
	);
	// DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f);
	// DrawDebugSphere(GetWorld(), End, Radius, 12, FColor::Yellow, false, 1.0f);
	// DrawDebugSphere(GetWorld(), Start, Radius, 12, FColor::Red, false, 1.0f);
	if (bHit)
	{
		// 处理碰撞结果
		AActor* HitActor = HitResult.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
		if (HitActor && HitActor->IsA(ACharacter::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
			UGameplayStatics::ApplyDamage(HitActor, 10.0f, GetController(), this, UDamageType::StaticClass());
		}

		// 可选：绘制调试信息
		// DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, Radius, 12, FColor::Red, false, 1.0f);
	}
}