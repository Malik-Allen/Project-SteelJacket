// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

/*Player Input Bindings*/
const FName AMyPlayerPawn::MoveForwardBinding("MoveForward");
const FName AMyPlayerPawn::MoveRightBinding("MoveRight");
const FName AMyPlayerPawn::LookUpBinding("LookUp");
const FName AMyPlayerPawn::LookRightBinding("LookRight");
const FName AMyPlayerPawn::LightAttackBinding("LightAttack");
const FName AMyPlayerPawn::HeavyAttackBinding("HeavyAttack");



// Sets default values
AMyPlayerPawn::AMyPlayerPawn() :
	CameraComponent(nullptr), CameraBoom(nullptr), 
	bIsIdle(false), bIsRunning(false), bIsAttacking(false), bIsAbleToAttack(true), bIsWeaponSheathed(false),
	CurrentSpeed(0.0f), MaxSpeed(125.0f),
	ComboCount(0), AttackType(NONE), SavedAttack(NONE), bIsAbleToSaveAttack(false),
	MovementAxis(FVector(0.0f)), LookAtAxis(FVector(0.0f))
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Init_Camera();
}

void AMyPlayerPawn::Init_Camera() {
	// Creating the SpringArmComponent/ 'CameraBoom'
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 95.0f;	// How far will the camera be from the pawn
	// Camera position and rotation is currently set over the right shoulder 
	CameraBoom->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	CameraBoom->SetRelativeLocation(FVector(-60.0f, 55.0f, 60.0f));
	CameraBoom->bDoCollisionTest = true;	// We will be using a collision test for now, when we give the player control of camera rotation a new system will be made

	
	// Now.. onto creating the camera component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // The pawn will control the rotation of the camera
}


// Called when the game starts or when spawned
void AMyPlayerPawn::BeginPlay()
{
	bIsIdle = true;
	Reset_Combo();
	Super::BeginPlay();
}

// Called every frame
void AMyPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Axes
	PlayerInputComponent->BindAxis(MoveForwardBinding, this, &AMyPlayerPawn::On_MoveForward_Axis);
	PlayerInputComponent->BindAxis(MoveRightBinding, this, &AMyPlayerPawn::On_MoveRight_Axis);
	PlayerInputComponent->BindAxis(LookUpBinding, this, &AMyPlayerPawn::On_LookUp_Axis);
	PlayerInputComponent->BindAxis(LookRightBinding, this, &AMyPlayerPawn::On_LookRight_Axis);

	// Actions
	PlayerInputComponent->BindAction(LightAttackBinding, EInputEvent::IE_Pressed, this, &AMyPlayerPawn::On_LightAttack);
	PlayerInputComponent->BindAction(HeavyAttackBinding, EInputEvent::IE_Pressed, this, &AMyPlayerPawn::On_HeavyAttack);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, TEXT("Player Input Set Up!"));
}

void AMyPlayerPawn::On_MoveForward_Axis_Implementation(float value) {
	MovementAxis.X = value;
	MovePlayer();
}
void AMyPlayerPawn::On_MoveRight_Axis_Implementation(float value) {
	MovementAxis.Y = value;
	MovePlayer();
}
void AMyPlayerPawn::On_LookUp_Axis_Implementation(float value) {
	LookAtAxis.Y = value;
}
void AMyPlayerPawn::On_LookRight_Axis_Implementation(float value) {
	LookAtAxis.X = value;
}

void AMyPlayerPawn::MovePlayer() {
	if (bIsAttacking) {
		CurrentSpeed = 0.0f;
		return;
	}

	CurrentSpeed = MovementAxis.Size() * MaxSpeed;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(3, 5.0f, FColor::Orange, TEXT("Current Speed: " + FString::SanitizeFloat(CurrentSpeed)));

	AddMovementInput(MovementAxis, CurrentSpeed);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(4, 5.0f, FColor::Orange, TEXT("Current Movement Axis: " + MovementAxis.ToString()));


	if (CurrentSpeed > 0.0f) {
		bIsIdle = false;
		bIsRunning = true;
	}
	else {
		bIsIdle = true;
		bIsRunning = false;
	}
}

void AMyPlayerPawn::On_LightAttack_Implementation() {
	
	if (bIsAbleToAttack) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Player Light Attacked!"));
		if (!bIsAttacking) {
			bIsAttacking = true;
			AttackType = LIGHT_ATTACK;
			AddAttackToComboChain(AttackType);
		}
		else {
			if (bIsAbleToSaveAttack) {
				SavedAttack = LIGHT_ATTACK;
			}
		}
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Player Is Not Able to perform: Light Attack"));
	}
	
	
}

void AMyPlayerPawn::On_HeavyAttack_Implementation() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("Player Heavy Attacked!"));

	if (bIsAbleToAttack) {
		if (!bIsAttacking) {
			bIsAttacking = true;
			AttackType = HEAVY_ATTACK;
			AddAttackToComboChain(AttackType);
		}
		else {
			if (bIsAbleToSaveAttack) {
				SavedAttack = HEAVY_ATTACK;
			}
		}
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("Player Is Not Able to perform: Heavy Attack"));
	}
	
}

void AMyPlayerPawn::Reset_Combo() {
	AttackType = SavedAttack = NONE;
	ComboCount = 0;
	for (int i = 0; i < MAX_COMBO_CHAIN_LENGTH; ++i) {
		ComboChain[i] = NONE;
	}
	bIsAttacking = false;
	bIsAbleToSaveAttack = false;
	bIsAbleToAttack = true;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Combo Reset!"));
}

void AMyPlayerPawn::EnableSaveAttack(bool IsEnabled) {
	bIsAbleToSaveAttack = IsEnabled;
}

void AMyPlayerPawn::SetIsAbleToAttack(bool IsEnabled) {
	bIsAbleToAttack = IsEnabled;
}

void AMyPlayerPawn::AddAttackToComboChain(int attack) {
	
	if (ComboCount >= MAX_COMBO_CHAIN_LENGTH) {
		bIsAbleToAttack = false;
	}
		// Reset_Combo();

	if (ComboCount >= 0 && ComboCount < MAX_COMBO_CHAIN_LENGTH) {
		ComboChain[ComboCount] = attack;
	}

	if (GEngine)
	{
		FString comboChain;
		for (int i = 0; i < MAX_COMBO_CHAIN_LENGTH; ++i) {
			comboChain.Append(FString::FromInt(ComboChain[i])+ ",");
		}
		GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::White, "Current Combo Count: " + FString::FromInt(ComboCount));
		GEngine->AddOnScreenDebugMessage(2, 15.0f, FColor::White, "Combo Chain: " + comboChain);
	}
}

void AMyPlayerPawn::IncrementComboCount() {
	++ComboCount;
}

bool AMyPlayerPawn::Use_Saved_Attack_Implementation() {
	if (SavedAttack > NONE) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(7, 5.0f, FColor::Cyan, "Using Save Attack, Attack Type:  " + FString::FromInt(SavedAttack));
		AttackType = SavedAttack;
		AddAttackToComboChain(AttackType);
		SavedAttack = NONE;
		return true;
	}
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Player Did Not Save an Attack!"));
	return false;
}
