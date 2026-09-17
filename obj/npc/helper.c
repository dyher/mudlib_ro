// obj/npc/helper.c — interactive NPC driven by rAthena script
inherit "/std/npc";

void create()
{
    set_name("Helper", ({ "helper" }));
    set("script", "/npc/helper.txt");
}

void on_talk(object who)
{
    find_object("/std/system/npcscript")->run_script(who, query("script"));
}
