// std/object/player.c — RO player (ES2 features + combat/skills/stats)

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
int allocate_stat(string stat);
int has_skill(int id);
int learn_skill(string name);
void cast_skill(string skillname, string target);
void skill_attack_mob(mapping sk, string target);

// ===== Stats (HP/SP) =====
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
    set("stat_points", 20);
    set("skills", ([]));
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

// ===== Stat Points =====
int query_stat_points() { return query("stat_points", 1); }

int stat_cost(string stat)
{
    int val = query_attr(stat, 1);
    if (val < 10) return 2;
    if (val < 20) return 3;
    if (val < 30) return 4;
    if (val < 40) return 5;
    return 6;
}

int allocate_stat(string stat)
{
    int cost, val;
    if (member_array(stat, ({ "str","agi","vit","int","dex","luk" })) < 0) {
        write("Unknown attribute: " + stat + "\n");
        return 0;
    }
    val = query_attr(stat, 1);
    if (val >= ATTRVAL_MAX) {
        write("Your " + stat + " is already at max.\n");
        return 0;
    }
    cost = stat_cost(stat);
    if (query_stat_points() < cost) {
        write("Not enough stat points (need " + cost + ").\n");
        return 0;
    }
    set_attr(stat, val + 1);
    set("stat_points", query_stat_points() - cost);
    recompute_max();
    write("Your " + stat + " is now " + query_attr(stat) + ". ");
    write("(cost " + cost + ", " + query_stat_points() + " left)\n");
    return 1;
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
    p_hp = query("hp");
    p_atk = calc_atk();
    p_def = calc_def();
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
        write("You were defeated!\n");
        p_hp = query("max_hp") / 10;
    }
    if (p_hp < 1) p_hp = 1;
    set("hp", p_hp);
}

// ===== Skills =====
int has_skill(int id)
{
    mapping learned = query("skills");
    return mapp(learned) && learned[id];
}

int learn_skill(string name)
{
    object SL = find_object("/std/loader/skill_loader");
    mapping sk, learned;
    if (!SL) { write("Skill system unavailable.\n"); return 0; }
    sk = SL->query_skill_by_name(name);
    if (!sk) { write("Unknown skill: " + name + "\n"); return 0; }
    if (sk["job_id"] != query("job")) {
        write("Your job (" + query("job_name") + ") cannot learn " + sk["name"] + ".\n");
        return 0;
    }
    learned = query("skills");
    if (!mapp(learned)) learned = ([]);
    if (learned[sk["id"]]) { write("You already know " + sk["name"] + ".\n"); return 0; }
    learned[sk["id"]] = 1;
    set("skills", learned);
    write("You learned " + sk["name"] + "!\n");
    return 1;
}

void cast_skill(string skillname, string target)
{
    object SL = find_object("/std/loader/skill_loader");
    object GL = find_object("/std/system/game_lib");
    mapping sk;
    if (!SL) { write("Skill system unavailable.\n"); return; }
    sk = SL->query_skill_by_name(skillname);
    if (!sk) { write("Unknown skill: " + skillname + "\n"); return; }
    if (!has_skill(sk["id"])) { write("You haven't learned " + sk["name"] + ".\n"); return; }
    if (query("sp", 1) < sk["sp_cost"]) { write("Not enough SP.\n"); return; }
    set("sp", query("sp", 1) - sk["sp_cost"]);

    if (sk["type"] == "heal") {
        int amount = sk["power"] + query_attr("int") * 4;
        GL->apply_heal(this_object(), amount);   // ← 呼叫統一函數庫
        write("You cast " + sk["name"] + " and recover " + amount + " HP.\n");
    } else if (sk["type"] == "attack") {
        skill_attack_mob(sk, target);
    }
}

void skill_attack_mob(mapping sk, string target)
{
    object ML = find_object("/std/loader/mob_loader");
    mapping mob;
    int mob_hp, p_hp, sk_dmg, rounds;
    if (!ML) return;
    mob = ML->query_mob_by_name(target);
    if (!mob) { write("No such monster: " + target + "\n"); return; }

    sk_dmg = calc_atk() * sk["power"] / 100;
    write("You cast " + sk["name"] + " on " + mob["name"] + " for " + sk_dmg + " dmg!\n");
    mob_hp = mob["hp"] - sk_dmg;
    p_hp = query("hp");
    rounds = 0;

    while (mob_hp > 0 && p_hp > 0 && rounds < 50) {
        int dmg = calc_atk() - mob["def"];
        int mdmg;
        if (dmg < 1) dmg = 1;
        mob_hp -= dmg;
        write("You hit " + mob["name"] + " for " + dmg + " dmg.");
        if (mob_hp <= 0) { write("\n"); break; }
        mdmg = mob["atk"] - calc_def();
        if (mdmg < 1) mdmg = 1;
        p_hp -= mdmg;
        write("  " + mob["name"] + " hits you for " + mdmg + " dmg.\n");
        rounds++;
    }
    if (mob_hp <= 0) {
        write("You defeated the " + mob["name"] + "!\n");
        gain_exp(mob["base_exp"], mob["job_exp"]);
    } else {
        write("You were defeated!\n");
        p_hp = query("max_hp") / 10;
    }
    if (p_hp < 1) p_hp = 1;
    set("hp", p_hp);
}

void list_skills()
{
    object SL = find_object("/std/loader/skill_loader");
    mapping skills, learned;
    int *ids, i;
    if (!SL) { write("Skill system unavailable.\n"); return; }
    skills = SL->query_all_skills();
    learned = query("skills");
    if (!mapp(learned)) learned = ([]);
    ids = keys(skills);
    write("Skills (your job: " + query("job_name") + "):\n");
    for (i = 0; i < sizeof(ids); i++) {
        mapping s = skills[ids[i]];
        string status = learned[s["id"]] ? "[learned]" :
                        (s["job_id"] == query("job") ? "[learnable]" : "[other job]");
        write("  " + s["name"] + " (" + s["type"] + ", SP:" + s["sp_cost"] + ") " + status + "\n");
    }
    write("Use: learn <name>, cast <name> [target]\n");
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
        set("stat_points", query("stat_points", 1) + 5);
        write("*** LEVEL UP! Base level " + lvl + "! (+5 stat points) ***\n");
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
    mapping learned = query("skills");
    write("=== " + name() + " the " + query("job_name") + " ===\n");
    write("Base Lv: " + query("base_level") + "   Job Lv: " + query("job_level") + "\n");
    write("Base EXP: " + query("base_exp") + "   Job EXP: " + query("job_exp") + "\n");
    write("Stat Points: " + query_stat_points() + "   Skills: " + (mapp(learned) ? sizeof(learned) : 0) + "\n");
    write("HP: " + query("hp") + "/" + query("max_hp") + "   SP: " + query("sp") + "/" + query("max_sp") + "\n");
    write("ATK: " + calc_atk() + "   DEF: " + calc_def() + "\n");
    write("\nAttributes (use 'stat <name>'):\n");
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
