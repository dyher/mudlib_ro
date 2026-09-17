// d/world/prontera.c — Prontera capital (safe zone)
inherit "/std/room";

void create()
{
    set("short", "Prontera");
    set("long", "The capital city of the Rune-Midgarts Kingdom. Peaceful and safe.");
    set("exits", ([ "north": "/d/world/prt_fild08" ]));
    set("no_combat", 1);
    set("npcs", ([ "sister": "/obj/npc/healer", "gift": "/obj/npc/gift", "helper": "/obj/npc/helper", "collector": "/obj/npc/collector" ]));
}
