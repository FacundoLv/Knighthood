#include "KnighthoodCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/UnrealMathUtility.h"

//////////////////////////////////////////////////////////////////////////
// AKnighthoodCharacter

AKnighthoodCharacter::AKnighthoodCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	health = 200.0f;
	MAX_HEALTH = 200.0f;
	
	mana = 100.0f;
	MAX_MANA = 100.0f;

	bIsDead = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AKnighthoodCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AKnighthoodCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AKnighthoodCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AKnighthoodCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AKnighthoodCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AKnighthoodCharacter::SpellAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AKnighthoodCharacter::MeleeAttack);
}

void AKnighthoodCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AKnighthoodCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AKnighthoodCharacter::SpellAttack()
{
	if (!m_StandingCast || !m_MovingCast) return;

	if (IsMoving())
		PlayAnimMontage(m_MovingCast, 1.2f);
	else
		PlayAnimMontage(m_StandingCast, 1.5f);
}

void AKnighthoodCharacter::MeleeAttack()
{
	if (!m_StandingMelee || !m_MovingMelee) return;

	if (IsMoving())
		PlayAnimMontage(m_MovingMelee, 1.2f);
	else
		PlayAnimMontage(m_StandingMelee, 1.5f);
}

void AKnighthoodCharacter::CastSpell(bool isRightHand)
{
	if (!SpellClass || mana <= 0.0f) return;

	FName socketName = isRightHand ? "Hand_RSocket" : "Hand_LSocket";
	FVector Location = GetMesh()->GetSocketLocation(socketName);
	FRotator Rotation = GetActorRotation();
	FActorSpawnParameters SpawnInfo;
	AActor* actor = GetWorld()->SpawnActor<AActor>(SpellClass, Location, Rotation);
	actor->SetOwner(this);

	float spellCost = 10.0f;
	mana = FMath::Clamp((mana - spellCost), 0.0f, MAX_MANA);
}

bool AKnighthoodCharacter::GetDamage(float damageAmount)
{
	if (IsHit() || bIsDead) return bIsDead;

	health = FMath::Clamp((health - damageAmount), 0.0f, MAX_HEALTH);

	if (health <= 0.0f)
	{
		PlayAnimMontage(m_Death);
		bIsDead = true;
	}
	else
		PlayAnimMontage(m_GetHit);
	
	return bIsDead;
}

void AKnighthoodCharacter::MoveForward(float Value)
{
	if (!AxisHasInput(Value)) return;

	StopIfActive(m_StandingCast);
	StopIfActive(m_StandingMelee);

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AKnighthoodCharacter::MoveRight(float Value)
{
	if (!AxisHasInput(Value)) return;

	StopIfActive(m_StandingCast);
	StopIfActive(m_StandingMelee);

	// find out which way is right
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get right vector 
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// add movement in that direction
	AddMovementInput(Direction, Value);
}

bool AKnighthoodCharacter::AxisHasInput(float Value)
{
	return (Controller != nullptr) && (Value != 0.0f);
}

bool AKnighthoodCharacter::IsMoving()
{
	return !(this->GetVelocity().IsNearlyZero());
}

bool AKnighthoodCharacter::IsHit()
{
	if (!m_GetHit) return false;

	return GetMesh()->GetAnimInstance()->Montage_IsPlaying(m_GetHit);
}

void AKnighthoodCharacter::StopIfActive(UAnimMontage* montage)
{
	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(montage))
		StopAnimMontage(montage);
}