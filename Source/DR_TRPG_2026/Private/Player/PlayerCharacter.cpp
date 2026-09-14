// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"

#include "INodeAndChannelMappings.h"
#include "TPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/ChessGameMode.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = GetController<APlayerController>()->
		GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) 
	{
		EnhancedInputLocalPlayerSubsystem->ClearAllMappings();
		EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
	}
	
	TargetSpringArmYaw = SpringArmComponent->GetRelativeRotation().Yaw;
	CurrentSpringArmYaw = SpringArmComponent->GetRelativeRotation().Yaw;
	
	GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FRotator CurrentRotation = SpringArmComponent->GetRelativeRotation();
	
	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetSpringArmYaw);
	
	if (FMath::IsNearlyZero(DeltaYaw, 0.01f)){return;}

	float NewYaw = FMath::FInterpTo(CurrentRotation.Yaw, CurrentRotation.Yaw + DeltaYaw, DeltaTime, CameraSensitivity);
	SpringArmComponent->SetRelativeRotation(FRotator(CurrentRotation.Pitch, NewYaw, CurrentRotation.Roll));
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlayerMovement, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveCharacter);
		EnhancedInputComponent->BindAction(CameraTurnLeft, ETriggerEvent::Completed, this, &APlayerCharacter::TurnCameraLeft);
		EnhancedInputComponent->BindAction(CameraTurnRight, ETriggerEvent::Completed, this, &APlayerCharacter::TurnCameraRight);
		EnhancedInputComponent->BindAction(SpringArmSizeChange, ETriggerEvent::Triggered, this, &APlayerCharacter::ChangeSpringArmSize);
		EnhancedInputComponent->BindAction(EndTurnButton, ETriggerEvent::Completed, this, &APlayerCharacter::EndTurn);
	}
}

void APlayerCharacter::MoveCharacter(const FInputActionValue& Value)
{
	FVector2D MovementDirection = Value.Get<FVector2D>();

	const FRotator YawRotation(0, CameraComponent->GetComponentRotation().Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementDirection.Y);
	AddMovementInput(RightDirection, MovementDirection.X);
}

void APlayerCharacter::TurnCameraRight(const FInputActionValue& Value)
{
	TargetSpringArmYaw = TargetSpringArmYaw + 90.f;
}

void APlayerCharacter::TurnCameraLeft(const FInputActionValue& Value)
{
	TargetSpringArmYaw = TargetSpringArmYaw - 90.f;
}

void APlayerCharacter::ChangeSpringArmSize(const FInputActionValue& Value)
{
	const float ZoomDir = Value.Get<float>();
	if (Controller != nullptr)
	{
		float DesiredLength = SpringArmComponent->TargetArmLength + ZoomDir * CameraZoomSpeed;
		SpringArmComponent->TargetArmLength = DesiredLength;
	}
}

void APlayerCharacter::EndTurn(const FInputActionValue& Value)
{
	if (IsValid(GameMode))
	{
		GameMode->EndCurrentTurn();
	}
}

