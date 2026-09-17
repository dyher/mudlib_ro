// std/object/player.c — RO player (ES2 feature inheritance, rAthena-aligned)

#include <attribute.h>

inherit "/feature/dbase";
inherit "/feature/attribute";
inherit "/feature/name";
inherit "/feature/save";

// Forward prototypes (Neolith requires declaration before use)
void choose_job(int job_id);
void recompute_max();

// 依屬性 + 職業加成重算 max HP/SP
void recompute_max()
{
    int maxhp = 40 + query_attr("vit") * 5 + query("job_hp_bonus", 1);
    int maxsp = 10 + query_attr("int") * 3 + query("job_sp_bonus", 1);
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

// 依 rAthena job ID 選擇職業
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
