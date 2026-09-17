// obj/npc/collector.c — quest NPC driven by rAthena script
inherit "/std/npc";

void create()
{
    set_name("Collector", ({ "collector" }));
    set("script", "/npc/collector.txt");
}

void on_talk(object who)
{
    find_object("/std/system/npcscript")->run_script(who, query("script"));
}
