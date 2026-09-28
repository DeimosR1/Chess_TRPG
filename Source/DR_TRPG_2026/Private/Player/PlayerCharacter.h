// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Multiplayer/ChessPlayerState.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(EditAnywhere, Category = "GameState")
	TObjectPtr<class AChessGameState> GameState;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> PlayerMovement;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> CameraTurnRight;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> CameraTurnLeft;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SpringArmSizeChange;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> EndTurnButton;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DeselectButton;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> QuitButton;
	
	
	UFUNCTION(BlueprintCallable)
	void MoveCharacter(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void TurnCameraRight(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void TurnCameraLeft(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void ChangeSpringArmSize(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void EndTurn(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void DeselectPiece(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void QuitGame(const FInputActionValue& Value);
	
	UPROPERTY(VisibleAnywhere, Category = "View")
	FRotator CharacterViewRotation;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	float TargetSpringArmYaw = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "View")
	float CurrentSpringArmYaw = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "View")
	float CameraSensitivity = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "View")
	float CameraZoomSpeed = 10.0f;
	
	//For Multiplayer purposes
	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer")
	TObjectPtr<AChessPlayerState> ChessPlayerStateClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer")
	bool bIsPlayerTwo;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetupPlayer();
	
};
