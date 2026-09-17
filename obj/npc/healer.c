// obj/npc/healer.c — Sister the healer (calls game_lib unified functions)
inherit "/std/npc";

void create()
{
    set_name("Sister", ({ "sister", "healer" }));
    set("short", "Sister");
    set("long", "A kind healer who can restore your health and spirit.");
}

void on_talk(object who)
{
    object GL = find_object("/std/system/game_lib");
    int heal_hp = who->query("max_hp") - who->query("hp");
    int heal_sp = who->query("max_sp") - who->query("sp");
    write("Sister: May the light of Odin heal you, traveler.\n");
    if (heal_hp <= 0 && heal_sp <= 0) {
        write("Sister: You are already in perfect health.\n");
        return;
    }
    if (heal_hp > 0) GL->apply_heal(who, heal_hp);
    if (heal_sp > 0) GL->apply_sp(who, heal_sp);
    write("You have been fully healed!\n");
}
