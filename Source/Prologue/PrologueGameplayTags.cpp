
#include "PrologueGameplayTags.h"

namespace PrologueGameplayTags
{
	/** Input Tags */
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Sword, "InputTag.Attack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Bow, "InputTag.Attack.Bow");

	/** Comma Tags */
	UE_DEFINE_GAMEPLAY_TAG(Comma_Weapon_Sword, "Comma.Weapon.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Comma_Weapon_Bow, "Comma.Weapon.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Attack_Sword, "Comma.Ability.Attack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Attack_Bow, "Comma.Ability.Attack.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Dash, "Comma.Ability.Dash");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_OverClock, "Comma.Ability.OverClock");
	
	/** State Tags */
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_SwitchAttack_Sword, "Comma.State.SwitchAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_SwitchAttack_Bow, "Comma.State.SwitchAttack.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Cancel, "Comma.State.Cancel");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_CanCancel, "Comma.State.CanCancel");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Invincible, "Comma.State.Invincible");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_OverClock, "Comma.State.OverClock");
	
	/** Effect Tags */
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Damaging, "GameplayCue.Effect.Damaging");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_PlayerHit, "GameplayCue.Effect.PlayerHit");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_EnemyHit, "GameplayCue.Effect.EnemyHit");

	/** Event Tags */
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AttackHitCheck, "Event.Character.AttackHitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SwitchAttackHitCheck, "Event.Character.SwitchAttackHitCheck");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AddSword, "Event.Character.AddSword");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AddBow, "Event.Character.AddBow");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SpawnProjectile, "Event.Character.SpawnProjectile");

	/** Enemy Tags */
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Attack, "Enemy.Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Jump, "Enemy.Ability.Jump");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_State_Strafing, "Enemy.State.Strafing");
	
	/** Shared Tags */
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsDead, "Shared.State.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsAttacking, "Shared.State.IsAttacking");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsMoving, "Shared.State.IsMoving");
}