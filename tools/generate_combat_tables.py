from __future__ import annotations

import argparse
import csv
from dataclasses import dataclass
from pathlib import Path


BALANCE_VERSION = 1
PLAYER_LEVEL_MAX = 40
WEAPON_ENHANCE_MAX = 25


@dataclass(frozen=True)
class WeaponCurve:
    weapon_id: int
    weapon_code: str
    weapon_name: str
    weapon_type: str
    stat_id_prefix: int
    skill_set_id: str
    identity_skill_id: str
    smart_drop_tag: str
    option_pool_id: str
    base_durability_max: int
    repair_cost_rate: float
    equip_level_min: int
    trade_limit_count: int
    animation_set_id: str
    icon_key: str
    model_key: str
    display_order: int
    hand_type: str
    combat_role_tag: str
    range_profile: str
    weapon_stat_table_id: int
    resource_type: str
    base_weapon_power: int
    weapon_power_per_level: int
    base_move_speed: float
    attack_speed: float
    cast_speed: float
    identity_gauge_max: int
    identity_gauge_gain: float
    base_resource_max: int
    resource_max_step: int
    resource_regen: int
    resource_regen_step: int
    base_stagger_power: int
    stagger_power_per_level: int
    threat_gen: float
    parry_window_bonus: float


WEAPON_CURVES = (
    WeaponCurve(
        weapon_id=3001,
        weapon_code="sword_shield_01",
        weapon_name="검방",
        weapon_type="SWORD_SHIELD",
        stat_id_prefix=2000,
        skill_set_id="SKILLSET_SWORD_SHIELD_V1",
        identity_skill_id="SKILL_IDENTITY_SHIELD_COUNTER",
        smart_drop_tag="SWORD_SHIELD",
        option_pool_id="OPT_POOL_DEFENSE",
        base_durability_max=120,
        repair_cost_rate=1.00,
        equip_level_min=1,
        trade_limit_count=3,
        animation_set_id="ANIMSET_SWORD_SHIELD",
        icon_key="weapon_sword_shield",
        model_key="mdl_weapon_sword_shield",
        display_order=10,
        hand_type="ONE_HAND_SHIELD",
        combat_role_tag="GUARD",
        range_profile="MELEE",
        weapon_stat_table_id=200,
        resource_type="STAMINA",
        base_weapon_power=60,
        weapon_power_per_level=4,
        base_move_speed=4.2,
        attack_speed=0.00,
        cast_speed=-0.05,
        identity_gauge_max=1000,
        identity_gauge_gain=0.00,
        base_resource_max=100,
        resource_max_step=2,
        resource_regen=6,
        resource_regen_step=1,
        base_stagger_power=110,
        stagger_power_per_level=2,
        threat_gen=0.30,
        parry_window_bonus=0.04,
    ),
    WeaponCurve(
        weapon_id=3002,
        weapon_code="great_sword_01",
        weapon_name="대검",
        weapon_type="GREATSWORD",
        stat_id_prefix=2100,
        skill_set_id="SKILLSET_GREAT_SWORD_V1",
        identity_skill_id="SKILL_IDENTITY_BLADE_BREAK",
        smart_drop_tag="GREATSWORD",
        option_pool_id="OPT_POOL_BRUISER",
        base_durability_max=110,
        repair_cost_rate=1.10,
        equip_level_min=1,
        trade_limit_count=3,
        animation_set_id="ANIMSET_GREATSWORD",
        icon_key="weapon_greatsword",
        model_key="mdl_weapon_greatsword",
        display_order=20,
        hand_type="TWO_HAND",
        combat_role_tag="BURST",
        range_profile="MELEE",
        weapon_stat_table_id=210,
        resource_type="STAMINA",
        base_weapon_power=85,
        weapon_power_per_level=5,
        base_move_speed=4.0,
        attack_speed=-0.05,
        cast_speed=-0.10,
        identity_gauge_max=1000,
        identity_gauge_gain=0.00,
        base_resource_max=100,
        resource_max_step=2,
        resource_regen=5,
        resource_regen_step=1,
        base_stagger_power=135,
        stagger_power_per_level=3,
        threat_gen=0.05,
        parry_window_bonus=0.00,
    ),
    WeaponCurve(
        weapon_id=3003,
        weapon_code="staff_01",
        weapon_name="지팡이",
        weapon_type="STAFF",
        stat_id_prefix=2200,
        skill_set_id="SKILLSET_STAFF_V1",
        identity_skill_id="SKILL_IDENTITY_ARCANE_BURST",
        smart_drop_tag="STAFF",
        option_pool_id="OPT_POOL_CASTER",
        base_durability_max=90,
        repair_cost_rate=0.90,
        equip_level_min=1,
        trade_limit_count=3,
        animation_set_id="ANIMSET_STAFF",
        icon_key="weapon_staff",
        model_key="mdl_weapon_staff",
        display_order=30,
        hand_type="CHANNELING",
        combat_role_tag="CASTER",
        range_profile="RANGED",
        weapon_stat_table_id=220,
        resource_type="MANA",
        base_weapon_power=110,
        weapon_power_per_level=5,
        base_move_speed=4.3,
        attack_speed=0.05,
        cast_speed=0.15,
        identity_gauge_max=1000,
        identity_gauge_gain=0.00,
        base_resource_max=140,
        resource_max_step=3,
        resource_regen=8,
        resource_regen_step=1,
        base_stagger_power=90,
        stagger_power_per_level=2,
        threat_gen=-0.10,
        parry_window_bonus=-0.02,
    ),
)


def float_text(value: float) -> str:
    return f"{value:.2f}"


def write_csv(path: Path, fieldnames: list[str], rows: list[dict[str, str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def build_player_stat_rows() -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for level in range(1, PLAYER_LEVEL_MAX + 1):
        rows.append(
            {
                "PlayerStatTableId": str(1000 + level),
                "Level": str(level),
                "MaxHP": str(1000 + (level - 1) * 80),
                "AttackPower": str(80 + (level - 1) * 6),
                "Defense": str(100 + (level - 1) * 9),
                "CriticalChance": float_text(0.05),
                "CriticalDamage": float_text(0.50),
                "CooldownReduction": float_text(0.00),
                "DamageReduction": float_text(0.00),
                "MoveSpeedBonus": float_text(0.00),
                "BalanceVersion": str(BALANCE_VERSION),
            }
        )
    return rows


def build_user_level_link_rows() -> list[dict[str, str]]:
    return [
        {
            "Level": str(level),
            "PlayerStatTableId": str(1000 + level),
            "BalanceVersion": str(BALANCE_VERSION),
        }
        for level in range(1, PLAYER_LEVEL_MAX + 1)
    ]


def build_weapon_stat_rows() -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for curve in WEAPON_CURVES:
        for level in range(1, WEAPON_ENHANCE_MAX + 1):
            rows.append(
                {
                    "WeaponStatTableId": str(curve.weapon_stat_table_id),
                    "EnhanceLevel": str(level),
                    "WeaponPower": str(curve.base_weapon_power + (level - 1) * curve.weapon_power_per_level),
                    "BaseMoveSpeed": float_text(curve.base_move_speed),
                    "AttackSpeed": float_text(curve.attack_speed),
                    "CastSpeed": float_text(curve.cast_speed),
                    "IdentityGaugeMax": str(curve.identity_gauge_max),
                    "IdentityGaugeGain": float_text(curve.identity_gauge_gain),
                    "ResourceType": curve.resource_type,
                    "ResourceMax": str(curve.base_resource_max + ((level - 1) // 5) * curve.resource_max_step),
                    "ResourceRegen": str(curve.resource_regen + ((level - 1) // 10) * curve.resource_regen_step),
                    "StaggerPower": str(curve.base_stagger_power + (level - 1) * curve.stagger_power_per_level),
                    "ThreatGen": float_text(curve.threat_gen),
                    "ParryWindowBonus": float_text(curve.parry_window_bonus),
                    "BalanceVersion": str(BALANCE_VERSION),
                }
            )
    return rows


def build_weapon_level_link_rows() -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for curve in WEAPON_CURVES:
        for level in range(1, WEAPON_ENHANCE_MAX + 1):
            rows.append(
                {
                    "WeaponId": str(curve.weapon_id),
                    "EnhanceLevel": str(level),
                    "WeaponStatTableId": str(curve.weapon_stat_table_id),
                    "BalanceVersion": str(BALANCE_VERSION),
                }
            )
    return rows


def build_weapon_table_rows() -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for curve in WEAPON_CURVES:
        rows.append(
            {
                "WeaponId": str(curve.weapon_id),
                "WeaponCode": curve.weapon_code,
                "WeaponName": curve.weapon_name,
                "WeaponType": curve.weapon_type,
                "WeaponStatTableId": str(curve.weapon_stat_table_id),
                "MaxEnhanceLevel": str(WEAPON_ENHANCE_MAX),
                "SkillSetId": curve.skill_set_id,
                "IdentitySkillId": curve.identity_skill_id,
                "SmartDropTag": curve.smart_drop_tag,
                "OptionPoolId": curve.option_pool_id,
                "BaseDurabilityMax": str(curve.base_durability_max),
                "RepairCostRate": float_text(curve.repair_cost_rate),
                "EquipLevelMin": str(curve.equip_level_min),
                "TradeLimitCount": str(curve.trade_limit_count),
                "AnimationSetId": curve.animation_set_id,
                "IconKey": curve.icon_key,
                "ModelKey": curve.model_key,
                "DisplayOrder": str(curve.display_order),
                "HandType": curve.hand_type,
                "CombatRoleTag": curve.combat_role_tag,
                "RangeProfile": curve.range_profile,
                "BalanceVersion": str(BALANCE_VERSION),
            }
        )
    return rows


def validate_rows(
    player_rows: list[dict[str, str]],
    user_level_rows: list[dict[str, str]],
    weapon_rows: list[dict[str, str]],
    weapon_level_rows: list[dict[str, str]],
) -> None:
    if len(player_rows) != PLAYER_LEVEL_MAX:
        raise ValueError("player stat row count mismatch")
    if len(user_level_rows) != PLAYER_LEVEL_MAX:
        raise ValueError("user level link row count mismatch")
    if len(weapon_rows) != len(WEAPON_CURVES) * WEAPON_ENHANCE_MAX:
        raise ValueError("weapon stat row count mismatch")
    if len(weapon_level_rows) != len(WEAPON_CURVES) * WEAPON_ENHANCE_MAX:
        raise ValueError("weapon level link row count mismatch")

    player_ids = {row["PlayerStatTableId"] for row in player_rows}
    for row in user_level_rows:
        if row["PlayerStatTableId"] not in player_ids:
            raise ValueError("user level link references unknown player stat row")

    weapon_ids = {row["WeaponStatTableId"] for row in weapon_rows}
    prev_hp = 0
    prev_attack = 0
    prev_defense = 0
    for row in player_rows:
        hp = int(row["MaxHP"])
        attack = int(row["AttackPower"])
        defense = int(row["Defense"])
        if hp <= prev_hp or attack <= prev_attack or defense <= prev_defense:
            raise ValueError("player stat rows must be strictly increasing")
        prev_hp = hp
        prev_attack = attack
        prev_defense = defense

    for curve in WEAPON_CURVES:
        type_rows = [row for row in weapon_rows if row["WeaponStatTableId"] == str(curve.weapon_stat_table_id)]
        prev_power = 0
        prev_stagger = 0
        for row in type_rows:
            power = int(row["WeaponPower"])
            stagger = int(row["StaggerPower"])
            if power <= prev_power or stagger <= prev_stagger:
                raise ValueError(f"{curve.weapon_type} rows must be strictly increasing")
            prev_power = power
            prev_stagger = stagger

    for row in weapon_level_rows:
        if row["WeaponStatTableId"] not in weapon_ids:
            raise ValueError("weapon level link references unknown weapon stat row")


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate proposal combat CSV tables.")
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path(__file__).resolve().parents[1] / "docs" / "proposal" / "ingame" / "data",
        help="Directory where combat CSV tables will be written.",
    )
    args = parser.parse_args()

    player_rows = build_player_stat_rows()
    user_level_rows = build_user_level_link_rows()
    weapon_stat_rows = build_weapon_stat_rows()
    weapon_level_rows = build_weapon_level_link_rows()
    weapon_table_rows = build_weapon_table_rows()

    validate_rows(player_rows, user_level_rows, weapon_stat_rows, weapon_level_rows)

    write_csv(args.output_dir / "player_stat_table.csv", ["PlayerStatTableId", "Level", "MaxHP", "AttackPower", "Defense", "CriticalChance", "CriticalDamage", "CooldownReduction", "DamageReduction", "MoveSpeedBonus", "BalanceVersion"], player_rows)
    write_csv(args.output_dir / "user_level_stat_link_table.csv", ["Level", "PlayerStatTableId", "BalanceVersion"], user_level_rows)
    write_csv(args.output_dir / "weapon_stat_table.csv", ["WeaponStatTableId", "EnhanceLevel", "WeaponPower", "BaseMoveSpeed", "AttackSpeed", "CastSpeed", "IdentityGaugeMax", "IdentityGaugeGain", "ResourceType", "ResourceMax", "ResourceRegen", "StaggerPower", "ThreatGen", "ParryWindowBonus", "BalanceVersion"], weapon_stat_rows)
    write_csv(args.output_dir / "weapon_level_stat_link_table.csv", ["WeaponId", "EnhanceLevel", "WeaponStatTableId", "BalanceVersion"], weapon_level_rows)
    write_csv(args.output_dir / "weapon_table.csv", ["WeaponId", "WeaponCode", "WeaponName", "WeaponType", "WeaponStatTableId", "MaxEnhanceLevel", "SkillSetId", "IdentitySkillId", "SmartDropTag", "OptionPoolId", "BaseDurabilityMax", "RepairCostRate", "EquipLevelMin", "TradeLimitCount", "AnimationSetId", "IconKey", "ModelKey", "DisplayOrder", "HandType", "CombatRoleTag", "RangeProfile", "BalanceVersion"], weapon_table_rows)

    print(f"Generated combat tables in {args.output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
