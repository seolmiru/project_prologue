
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

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Parry, "Comma.Ability.Parry");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_SmashAttack, "Comma.Ability.SmashAttack");
	
	/** Comma State Tags */
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_SwitchAttack_Sword, "Comma.State.SwitchAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_SwitchAttack_Bow, "Comma.State.SwitchAttack.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_CancelDisabled, "Comma.State.CancelDisabled");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_CancelEnabled, "Comma.State.CancelEnabled");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Invincible, "Comma.State.Invincible");
	
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Dashing, "Comma.State.Dashing");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_OverClock, "Comma.State.OverClock");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_PerfectShot, "Comma.State.PerfectShot");
	
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Parrying, "Comma.State.Parrying")
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_JustParrying, "Comma.State.JustParrying")

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_ParrySuccess, "Comma.State.ParrySuccess")

	/** Comma Event Tags */
	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_JustDash, "Comma.Event.JustDash");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_PerfectShot, "Comma.Event.PerfectShot");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_SwitchAttackSword, "Comma.Event.SwitchAttackSword");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_SwitchAttackBow, "Comma.Event.SwitchAttackBow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_JustParry, "Comma.Event.JustParry");

	/** Comma Cooldown Tags */
	UE_DEFINE_GAMEPLAY_TAG(Comma_Cooldown_Parry, "Comma.Cooldown.Parry");
	
	/** Effect Tags */
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Damaging, "GameplayCue.Effect.Damaging");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_SwitchAttackDamaging, "GameplayCue.Effect.SwitchAttackDamaging");
	
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_PlayerHit, "GameplayCue.Effect.PlayerHit");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_EnemyHit, "GameplayCue.Effect.EnemyHit");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Parried, "GameplayCue.Effect.Parried");

	/** Event Tags */
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AttackHitCheck, "Event.Character.AttackHitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SecondAttackHitCheck, "Event.Character.SecondAttackHitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_ThirdAttackHitCheck, "Event.Character.ThirdAttackHitCheck");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SwitchAttackHitCheck, "Event.Character.SwitchAttackHitCheck");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_ParryAttackHitCheck, "Event.Character.ParryAttackHitCheck");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_TraceStart, "Event.Character.TraceStart");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_TraceStop, "Event.Character.TraceStop");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AddSword, "Event.Character.AddSword"); 
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AddBow, "Event.Character.AddBow");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SpawnProjectile, "Event.Character.SpawnProjectile");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SpawnSwitchProjectile, "Event.Character.SpawnSwitchProjectile");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SpawnSwitchSubProjectile, "Event.Character.SpawnSwitchSubProjectile");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_EnableInput, "Event.Character.EnableInput");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_DisableInput, "Event.Character.DisableInput");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_NextCombo, "Event.Character.NextCombo");
	
	/** Enemy Tags */
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Attack, "Enemy.Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_RangeAttack, "Enemy.Ability.RangeAttack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_LaserAttack, "Enemy.Ability.LaserAttack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_CantParryAttack, "Enemy.Ability.CantParryAttack");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Charge, "Enemy.Ability.Charge");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Jump, "Enemy.Ability.Jump");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_BackJump, "Enemy.Ability.BackJump");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_FirstEncounter, "Enemy.Ability.FirstEncounter");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_State_Strafing, "Enemy.State.Strafing");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_State_RunningAway, "Enemy.State.RunningAway");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_State_Charge, "Enemy.State.Charge");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_State_Detect, "Enemy.State.Detect");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_Dashed, "Enemy.Event.Dashed");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_LastCombo, "Enemy.Event.LastCombo");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_Parried, "Enemy.Event.Parried");
	
	/** Shared Tags */
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Cooldown, "Shared.Ability.Cooldown");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact");

	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsDead, "Shared.State.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsAttacking, "Shared.State.IsAttacking");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsMoving, "Shared.State.IsMoving");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsOutOfToughness, "Shared.State.IsOutOfToughness");

	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Attack, "Shared.Cooldown.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Attack_Laser, "Shared.Cooldown.Attack.Laser");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Attack_Ranged, "Shared.Cooldown.Attack.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Attack_Run, "Shared.Cooldown.Attack.Run");
}