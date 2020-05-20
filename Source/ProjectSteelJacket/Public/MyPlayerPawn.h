// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyPlayerPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;



UCLASS()
class PROJECTSTEELJACKET_API AMyPlayerPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPlayerPawn();

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, Category = "Player State")
		bool bIsIdle;

	UPROPERTY(BlueprintReadOnly, Category = "Player State")
		bool bIsRunning;

	UPROPERTY(BlueprintReadWrite, Category = "Player State")
		bool bIsAttacking;

	UPROPERTY(BlueprintReadOnly, Category = "Player State")
		bool bIsAbleToAttack;

	UPROPERTY(BlueprintReadWrite, Category = "Player State")
		bool bIsWeaponSheathed;

	UPROPERTY(BlueprintReadOnly, Category = "Player Movement")
		float CurrentSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Player Movement")
		float MaxSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Player Attacks")
		int ComboCount;

	UPROPERTY(BlueprintReadOnly, Category = "Player Attacks")
		// No Attack: -1, Light Attack: 0, Heavy Attack: 1
		int AttackType;

	UPROPERTY(BlueprintReadOnly, Category = "Player Attacks")
		// No Attack: -1, Light Attack: 0, Heavy Attack: 1
		int SavedAttack;

	UPROPERTY(BlueprintReadOnly, Category = "Player Attacks")
		bool bIsAbleToSaveAttack;

	UFUNCTION(BlueprintCallable)
		void EnableSaveAttack(bool IsEnabled);

	UFUNCTION(BlueprintCallable)
		void SetIsAbleToAttack(bool IsEnabled);

	UFUNCTION(BlueprintCallable)
		// Left Joystick Input From Player
		FVector GetMovementAxis() const { return MovementAxis; };

	UFUNCTION(BlueprintCallable)
		// Right Joystick Input From Player
		FVector GetLookAtAxis() const { return LookAtAxis; }

	UFUNCTION(BlueprintCallable)
		void Reset_Combo();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		// Will Return True If the Saved Attack Has been evoked correctly by the player
		bool Use_Saved_Attack();
	virtual bool Use_Saved_Attack_Implementation();

	UFUNCTION(BlueprintCallable)
		void IncrementComboCount();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector MovementAxis;
	FVector LookAtAxis;

private:

	void Init_Camera();

	/*Player Input Bindings*/
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName LookUpBinding;
	static const FName LookRightBinding;
	static const FName LightAttackBinding;
	static const FName HeavyAttackBinding;

	enum { 
		NONE = -1,
		LIGHT_ATTACK = 0,
		HEAVY_ATTACK = 1,
		MAX_COMBO_CHAIN_LENGTH = 5 
	};

public:

	// Player Movement Functionality
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void On_MoveForward_Axis(float value);
	virtual void On_MoveForward_Axis_Implementation(float value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void On_MoveRight_Axis(float value);
	virtual void On_MoveRight_Axis_Implementation(float value);

	// Camera Movement Functionality
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void On_LookUp_Axis(float value);
	virtual void On_LookUp_Axis_Implementation(float value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void On_LookRight_Axis(float value);
	virtual void On_LookRight_Axis_Implementation(float value);

	// Player Attack Functionality
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void On_LightAttack();
	virtual void On_LightAttack_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void On_HeavyAttack();
	virtual void On_HeavyAttack_Implementation();



private:

	void MovePlayer();

	int ComboChain[MAX_COMBO_CHAIN_LENGTH];

	void AddAttackToComboChain(int attack);


};
