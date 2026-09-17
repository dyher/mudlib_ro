// std/component/combat.c — HP/MP/attack/defense

mapping combat_stats;

void init_combat()
{
    combat_stats = ([
        "hp": 100,
        "max_hp": 100,
        "mp": 50,
        "max_mp": 50,
        "atk": 10,
        "def": 5
    ]);
}

int query_hp() { return combat_stats["hp"]; }
int query_mp() { return combat_stats["mp"]; }
int query_atk() { return combat_stats["atk"]; }
int query_def() { return combat_stats["def"]; }

void damage(int amount)
{
    combat_stats["hp"] -= amount;
    if (combat_stats["hp"] < 0) combat_stats["hp"] = 0;
}

void heal(int amount)
{
    combat_stats["hp"] += amount;
    if (combat_stats["hp"] > combat_stats["max_hp"])
        combat_stats["hp"] = combat_stats["max_hp"];
}

mapping query_combat_stats()
{
    return combat_stats;
}
