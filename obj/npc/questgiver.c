inherit "/std/npc";
void create()
{
    set_name("Quest Giver", ({ "quest giver", "questgiver" }));
    set("script", "/npc/questgiver.txt");
}
void on_talk(object who)
{
    find_object("/std/system/npcscript")->run_script(who, query("script"));
}
