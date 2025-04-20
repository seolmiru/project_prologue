
#include "PrologueGameplayTags.h"

namespace PrologueGameplayTags
{
	//Input Tags
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipHammer, "InputTag.EquipHammer");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequippedHammer, "InputTag.UnequippedHammer");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipBow, "InputTag.EquipBow");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequippedBow, "InputTag.UnequippedBow");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Hammer, "InputTag.Attack.Hammer");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack_Bow, "InputTag.Attack.Bow");

	//Comma Tags
	UE_DEFINE_GAMEPLAY_TAG(Comma_Weapon_Hammer, "Comma.Weapon.Hammer");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Event_Equip_Hammer, "Comma.Event.Equip.Hammer");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Equip_Hammer, "Comma.Ability.Equip.Hammer");
	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Unequip_Hammer, "Comma.Ability.Unequipped.Hammer");

	UE_DEFINE_GAMEPLAY_TAG(Comma_Ability_Attack_Hammer, "Comma.Ability.Attack.Hammer");
}