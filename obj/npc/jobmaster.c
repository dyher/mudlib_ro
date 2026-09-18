inherit "/std/npc";
void create()
{
    set_name("Job Master", ({ "job master", "jobmaster" }));
    set("script", "/npc/jobmaster.txt");
}
void on_talk(object who)
{
    find_object("/std/system/npcscript")->run_script(who, query("script"));
}
