#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KnighthoodCharacter.generated.h"

UCLASS(config = Game)
class AKnighthoodCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	AKnighthoodCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float MAX_HEALTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float MAX_MANA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GetDamage)
		class UAnimMontage* m_GetHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GetDamage)
		class UAnimMontage* m_Death;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UAnimMontage* m_StandingCast;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UAnimMontage* m_MovingCast;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UAnimMontage* m_StandingMelee;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UAnimMontage* m_MovingMelee;

	// TODO: change AActor with more specific class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
		TSubclassOf<AActor> SpellClass;

protected:
	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void SpellAttack();

	void MeleeAttack();

	bool AxisHasInput(float Value);

	bool IsMoving();

	bool IsHit();

	void StopIfActive(UAnimMontage* montage);

	UFUNCTION(BlueprintCallable, Category = Magic)
		void CastSpell(bool isRightHand);

	UFUNCTION(BlueprintCallable, Category = GetDamage)
		bool GetDamage(float damageAmount);

private:
	bool bIsDead;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

