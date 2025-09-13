
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

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Skill, "Comma.Ability.Skill");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_SmashAttack, "Comma.Ability.SmashAttack");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Heal, "Comma.Ability.Heal");
	
	/** Comma State Tags */
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_SwitchAttack_Sword, "Comma.State.SwitchAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_SwitchAttack_Bow, "Comma.State.SwitchAttack.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_CancelDisabled, "Comma.State.CancelDisabled");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_CancelEnabled, "Comma.State.CancelEnabled");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Invincible, "Comma.State.Invincible");
	
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Dashing, "Comma.State.Dashing");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_OverClock, "Comma.State.OverClock");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Intro, "Comma.State.Intro");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Skill, "Comma.State.Skill");

	/** Comma Event Tags */
	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_Sword, "Comma.Event.Sword");
	
	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_JustDash, "Comma.Event.JustDash");
	
	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_SwitchAttackSword, "Comma.Event.SwitchAttackSword");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_HitStop, "Comma.Event.HitStop");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_ResetSkillCooldown, "Comma.Event.ResetSkillCooldown");

	/** Comma Cooldown Tags */
	UE_DEFINE_GAMEPLAY_TAG(Comma_Cooldown_Skill, "Comma.Cooldown.Skill");
	
	UE_DEFINE_GAMEPLAY_TAG(Comma_Cooldown_Heal, "Comma.Cooldown.Heal");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Cooldown_OverClock, "Comma.Cooldown.OverClock");

	/** Effect Tags */
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Damaging, "GameplayCue.Effect.Damaging");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_SmashAttackDamaging, "GameplayCue.Effect.SmashAttackDamaging");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_SkillDamaging, "GameplayCue.Effect.SkillDamaging");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_DamagingSound, "GameplayCue.Effect.DamagingSound");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_SmashDamagingSound, "GameplayCue.Effect.SmashDamagingSound");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_SkillDamagingSound, "GameplayCue.Effect.SkillDamagingSound");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_PlayerHit, "GameplayCue.Effect.PlayerHit");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_EnemyHit, "GameplayCue.Effect.EnemyHit");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_EnemySmashHit, "GameplayCue.Effect.EnemySmashHit");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_EnemySkillHit, "GameplayCue.Effect.EnemySKillHit");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Stun, "GameplayCue.Effect.Stun");
	
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Heal, "GameplayCue.Effect.Heal");

	/** Event Tags */
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AttackHitCheck, "Event.Character.AttackHitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SecondAttackHitCheck, "Event.Character.SecondAttackHitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_ThirdAttackHitCheck, "Event.Character.ThirdAttackHitCheck");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SwitchAttackHitCheck, "Event.Character.SwitchAttackHitCheck");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SkillHitCheck, "Event.Character.SkillHitCheck");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_BrokenCheck, "Event.Character.BrokenCheck");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AddSword, "Event.Character.AddSword"); 
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AddBow, "Event.Character.AddBow");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SpawnProjectile, "Event.Character.SpawnProjectile");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_EnableInput, "Event.Character.EnableInput");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_DisableInput, "Event.Character.DisableInput");

	UE_DEFINE_GAMEPLAY_TAG(Event_Character_NextCombo, "Event.Character.NextCombo");
	
	/** Enemy Tags */
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Attack, "Enemy.Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_RangeAttack, "Enemy.Ability.RangeAttack");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_LaserAttack, "Enemy.Ability.LaserAttack");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Charge, "Enemy.Ability.Charge");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Jump, "Enemy.Ability.Jump");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_BackJump, "Enemy.Ability.BackJump");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_FirstEncounter, "Enemy.Ability.FirstEncounter");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_State_Charge, "Enemy.State.Charge");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_State_Detect, "Enemy.State.Detect");
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_State_CanParry, "Enemy.State.CanParry");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_Dashed, "Enemy.Event.Dashed");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_LastCombo, "Enemy.Event.LastCombo");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_LaserHitCheck, "Enemy.Event.LaserHitCheck");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_Stun, "Enemy.Event.Stun");
	
	/** Shared Tags */
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Cooldown, "Shared.Ability.Cooldown");
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitFx, "Shared.Event.HitFx");

	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsDead, "Shared.State.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsAttacking, "Shared.State.IsAttacking");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsMoving, "Shared.State.IsMoving");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_IsOutOfToughness, "Shared.State.IsOutOfToughness");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_NoHitEffect, "Shared.State.NoHitEffect");
	UE_DEFINE_GAMEPLAY_TAG(Shared_State_Broken, "Shared.State.Broken");

	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Attack, "Shared.Cooldown.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Attack_Laser, "Shared.Cooldown.Attack.Laser");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Attack_Ranged, "Shared.Cooldown.Attack.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Attack_Run, "Shared.Cooldown.Attack.Run");
}