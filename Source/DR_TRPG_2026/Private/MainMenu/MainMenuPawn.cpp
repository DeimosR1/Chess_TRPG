// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMainMenuPawn::AMainMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

}

// Called when the game starts or when spawned
void AMainMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{	
		UE_LOG(LogTemp, Warning, TEXT("Binded Quit"));
		EnhancedInputComponent->BindAction(ExitGame, ETriggerEvent::Triggered, this,  &AMainMenuPawn::QuitGame);
	}

}

void AMainMenuPawn::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	if (const APlayerController* LocalPlayerController = GetController<APlayerController>())
	{
		if (const ULocalPlayer* LocalPlayer = LocalPlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				UE_LOG(LogTemp, Warning, TEXT("Mapping Reached"));
				EnhancedInputLocalPlayerSubsystem->ClearAllMappings();
				EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void AMainMenuPawn::QuitGame(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Quitting Game %s"), *GetName());
	APlayerController* PlayerController = GetController<APlayerController>();
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
}

