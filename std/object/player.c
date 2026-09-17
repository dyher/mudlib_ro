// std/object/player.c — RO player (ES2 features + rAthena combat/leveling)

#include <attribute.h>

inherit "/feature/dbase";
inherit "/feature/attribute";
inherit "/feature/name";
inherit "/feature/save";

// Forward prototypes
void choose_job(int job_id);
void recompute_max();
int calc_atk();
int calc_def();
void gain_exp(int base, int job);
void check_level_up();
void kill_mob(string name);

// ===== Stats =====
void recompute_max()
{
    int maxhp = 40 + query_attr("vit") * 5 + query("job_hp_bonus", 1) + query("base_level") * 3;
    int maxsp = 10 + query_attr("int") * 3 + query("job_sp_bonus", 1) + query("base_level") * 1;
    set("max_hp", maxhp);
    set("max_sp", maxsp);
    if (query("hp", 1) > maxhp) set("hp", maxhp);
    if (query("sp", 1) > maxsp) set("sp", maxsp);
}

void setup(string n)
{
    set_name(n, ({ "player", "user", n }));
    init_attribute(([ "str":1, "agi":1, "vit":1, "int":1, "dex":1, "luk":1 ]));
    set("base_level", 1);
    set("job_level", 1);
    set("base_exp", 0);
    set("job_exp", 0);
    choose_job(0);   // start as Novice
}

void choose_job(int job_id)
{
    object JL = find_object("/std/loader/job_loader");
    mapping j;
    if (!JL) { write("Job system unavailable.\n"); return; }
    j = JL->query_job(job_id);
    if (!j) { write("Unknown job ID.\n"); return; }
    set("job", job_id);
    set("job_name", j["name"]);
    set("job_hp_bonus", j["base_hp"]);
    set("job_sp_bonus", j["base_sp"]);
    recompute_max();
    set("hp", query("max_hp"));
    set("sp", query("max_sp"));
}

// ===== Combat =====
int calc_atk()
{
    return 10 + query_attr("str") * 2 + query_attr("dex") + query("base_level") * 2;
}

int calc_def()
{
    return query_attr("vit") / 2 + query("base_level");
}

void kill_mob(string name)
{
    object ML = find_object("/std/loader/mob_loader");
    mapping mob;
    int mob_hp, p_hp, p_atk, p_def, rounds;

    if (!ML) { write("Monster system unavailable.\n"); return; }
    mob = ML->query_mob_by_name(name);
    if (!mob) { write("No such monster: " + name + "\n"); return; }

    mob_hp = mob["hp"];
    p_hp   = query("hp");
    p_atk  = calc_atk();
    p_def  = calc_def();
    rounds = 0;

    write("A wild " + mob["name"] + " (Lv." + mob["level"] + ") appears!\n");

    while (mob_hp > 0 && p_hp > 0 && rounds < 50) {
        int dmg = p_atk - mob["def"];
        int mdmg;
        if (dmg < 1) dmg = 1;
        mob_hp -= dmg;
        write("You hit " + mob["name"] + " for " + dmg + " dmg.");
        if (mob_hp <= 0) { write("\n"); break; }
        mdmg = mob["atk"] - p_def;
        if (mdmg < 1) mdmg = 1;
        p_hp -= mdmg;
        write("  " + mob["name"] + " hits you for " + mdmg + " dmg.\n");
        rounds++;
    }

    if (mob_hp <= 0) {
        write("You defeated the " + mob["name"] + "!\n");
        gain_exp(mob["base_exp"], mob["job_exp"]);
    } else {
        write("You were defeated! You barely escape with your life.\n");
        p_hp = query("max_hp") / 10;
    }
    if (p_hp < 1) p_hp = 1;
    set("hp", p_hp);
}

// ===== Experience / Leveling =====
void gain_exp(int base, int job)
{
    set("base_exp", query("base_exp") + base);
    set("job_exp", query("job_exp") + job);
    write("You gained " + base + " base EXP and " + job + " job EXP.\n");
    check_level_up();
}

void check_level_up()
{
    int lvl = query("base_level");
    int need = lvl * 50;
    while (query("base_exp") >= need) {
        set("base_exp", query("base_exp") - need);
        lvl++;
        set("base_level", lvl);
        need = lvl * 50;
        write("*** LEVEL UP! You reached base level " + lvl + "! ***\n");
        recompute_max();
        set("hp", query("max_hp"));
        set("sp", query("max_sp"));
    }
}

// ===== Utility =====
void do_rest()
{
    set("hp", query("max_hp"));
    set("sp", query("max_sp"));
    write("You rest and recover fully.\n");
}

string query_save_file()
{
    return "/data/players/" + query("id");
}

void show_stats()
{
    write("=== " + name() + " the " + query("job_name") + " ===\n");
    write("Base Lv: " + query("base_level") + "   Job Lv: " + query("job_level") + "\n");
    write("Base EXP: " + query("base_exp") + "   Job EXP: " + query("job_exp") + "\n");
    write("HP: " + query("hp") + "/" + query("max_hp") + "   SP: " + query("sp") + "/" + query("max_sp") + "\n");
    write("ATK: " + calc_atk() + "   DEF: " + calc_def() + "\n");
    write("\nAttributes:\n");
    write("STR: " + query_attr("str") + "  AGI: " + query_attr("agi") + "  VIT: " + query_attr("vit") + "\n");
    write("INT: " + query_attr("int") + "  DEX: " + query_attr("dex") + "  LUK: " + query_attr("luk") + "\n");
}

void list_jobs()
{
    object JL = find_object("/std/loader/job_loader");
    mapping jobs;
    int *ids;
    int i;
    if (!JL) { write("Job system unavailable.\n"); return; }
    jobs = JL->query_all_jobs();
    ids = keys(jobs);
    write("Available jobs (rAthena job IDs):\n");
    for (i = 0; i < sizeof(ids); i++)
        write("  " + ids[i] + " - " + jobs[ids[i]]["name"] + "\n");
    write("Use: job <id>\n");
}

void list_mobs()
{
    object ML = find_object("/std/loader/mob_loader");
    mapping mobs;
    int *ids;
    int i;
    if (!ML) { write("Monster system unavailable.\n"); return; }
    mobs = ML->query_all_mobs();
    ids = keys(mobs);
    write("Nearby monsters (rAthena mob IDs):\n");
    for (i = 0; i < sizeof(ids); i++) {
        mapping m = mobs[ids[i]];
        write("  " + m["name"] + " (Lv." + m["level"] + ", HP:" + m["hp"] + ", EXP:" + m["base_exp"] + ")\n");
    }
    write("Use: kill <name>\n");
}
