// obj/npc/gambler.c — 賭場 (純 LPC 玩法示範)
// 展示：不用 rAthena 腳本，純 LPC 就能創建新玩法
inherit "/std/npc";

void create()
{
    set_name("Gambler", ({ "gambler" }));
    set("short", "Gambler");
    set("long", "A mysterious gambler offering fortune and ruin.");
}

void on_talk(object who)
{
    object GL = find_object("/std/system/game_lib");
    int roll, zeny;

    zeny = GL->query_zeny(who);
    if (zeny < 50) {
        write("Gambler: \"Come back when you have 50 Zeny, friend.\"\n");
        return;
    }

    GL->apply_zeny(who, -50);
    write("\nGambler: \"Feeling lucky? Let's roll the dice!\"\n");

    roll = random(1000);

    if (roll < 700) {
        write("  ... [Snake Eyes] ...\n");
        write("Gambler: \"Hah! The house wins again!\"\n");
        write("  You lost 50z.\n");
    } else if (roll < 900) {
        GL->apply_zeny(who, 150);
        write("  ... [Pair of Sevens] ...\n");
        write("Gambler: \"Not bad, not bad at all!\"\n");
        write("  You won 150z! (Net profit: +100z)\n");
    } else if (roll < 980) {
        GL->apply_zeny(who, 500);
        write("  ... [Triple Crowns] ...\n");
        write("Gambler: \"Impressive! Take your winnings!\"\n");
        write("  You won 500z! (Net profit: +450z)\n");
    } else {
        GL->apply_zeny(who, 2000);
        who->add_item(501, 5);
        write("  ... [*** JACKPOT ***] ...\n");
        write("Gambler: \"INCREDIBLE! The legendary jackpot!\"\n");
        write("  You won 2000z + 5x Red Potion! (Net profit: +1950z)\n");
    }
    write("\n");
}
