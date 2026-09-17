// obj/npc/gift.c — NPC driven by a rAthena script file
inherit "/std/npc";

void create()
{
    set_name("Gift Giver", ({ "gift", "giver" }));
    set("script", "/npc/gift.txt");
}

void on_talk(object who)
{
    find_object("/std/system/npcscript")->run_script(who, query("script"));
}
