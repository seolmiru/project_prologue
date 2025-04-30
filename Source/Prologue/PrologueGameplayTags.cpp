
#include "PrologueGameplayTags.h"

namespace PrologueGameplayTags
{
	//Input Tags
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Sword, "InputTag.Attack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Bow, "InputTag.Attack.Bow");

	//Comma Tags
	UE_DEFINE_GAMEPLAY_TAG(Comma_Weapon_Sword, "Comma.Weapon.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Comma_Weapon_Bow, "Comma.Weapon.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Attack_Sword, "Comma.Ability.Attack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Attack_Bow, "Comma.Ability.Attack.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Dash, "Comma.Ability.Dash");
	
	//State Tags
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_IsDead, "Comma.State.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_IsAttacking, "Comma.State.IsAttacking");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_IsMoving, "Comma.State.IsMoving");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_SwitchAttack_Sword, "Comma.State.SwitchAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_SwitchAttack_Bow, "Comma.State.SwitchAttack.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Cancel, "Comma.State.Cancel");
	UE_DEFINE_GAMEPLAY_TAG(Comma_State_CanCancel, "Comma.State.CanCancel");

	UE_DEFINE_GAMEPLAY_TAG(Comma_State_Invincible, "Comma.State.Invincible");
	
	//Effect Tags
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_Damaging, "GameplayCue.Effect.Damaging");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_PlayerHit, "GameplayCue.Effect.PlayerHit");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Effect_EnemyHit, "GameplayCue.Effect.EnemyHit");

	//Event Tags
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AttackHitCheck, "Event.Character.AttackHitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AddSword, "Event.Character.AddSword");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AddBow, "Event.Character.AddBow");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_SpawnProjectile, "Event.Character.SpawnProjectile");
}