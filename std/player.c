// std/player.c — RO player object (ES2-style feature inheritance)

#include <attribute.h>

inherit "/feature/dbase";
inherit "/feature/attribute";
inherit "/feature/name";
inherit "/feature/save";

// RO-specific: Level/Experience/Job
static int level;
static int base_exp;
static int job_level;
static int job_exp;

// Setup player with initial attributes
void setup(string n)
{
    // Set name and ID
    set_name(n, ({ "player", "user", lower_case(n) }));
    
    // Initialize RO-style attributes (STR/AGI/VIT/INT/DEX/LUK + HP/SP)
    init_attribute(([
        "str": 10,
        "agi": 10,
        "vit": 10,
        "int": 10,
        "dex": 10,
        "luk": 10
    ]));
    
    // Set base stats
    set("max_hp", 100 + query_attr("vit") * 5);
    set("max_sp", 50 + query_attr("int") * 3);
    set("hp", query("max_hp"));
    set("sp", query("max_sp"));
    
    // Initialize level
    level = 1;
    base_exp = 0;
    job_level = 1;
    job_exp = 0;
}

// Query save file path
string query_save_file()
{
    return "/data/players/" + query("id");
}

// Display player stats
void show_stats()
{
    write("=== " + name() + " ===\n");
    write("Level: " + level + "  Job Level: " + job_level + "\n");
    write("EXP: " + base_exp + "  Job EXP: " + job_exp + "\n");
    write("HP: " + query("hp") + "/" + query("max_hp") + "\n");
    write("SP: " + query("sp") + "/" + query("max_sp") + "\n");
    write("\nAttributes:\n");
    write("STR: " + query_attr("str") + "  AGI: " + query_attr("agi") + "\n");
    write("VIT: " + query_attr("vit") + "  INT: " + query_attr("int") + "\n");
    write("DEX: " + query_attr("dex") + "  LUK: " + query_attr("luk") + "\n");
}

// Take damage
void damage(int amount)
{
    int hp = query("hp") - amount;
    if (hp < 0) hp = 0;
    set("hp", hp);
}

// Heal HP
void heal_hp(int amount)
{
    int hp = query("hp") + amount;
    int max = query("max_hp");
    if (hp > max) hp = max;
    set("hp", hp);
}

// Heal SP
void heal_sp(int amount)
{
    int sp = query("sp") + amount;
    int max = query("max_sp");
    if (sp > max) sp = max;
    set("sp", sp);
}

// Gain experience
void gain_exp(int amount)
{
    base_exp += amount;
    write("You gained " + amount + " base EXP.\n");
}
