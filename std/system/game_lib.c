// std/system/game_lib.c — UNIFIED FUNCTION LIBRARY
// 「輪子只造一次」：LPC 物件和 rAthena 腳本都呼叫這組函數。
// 這些是核心遊戲效果的底層實作。

void apply_damage(object target, int amount)
{
    int hp;
    if (!target) return;
    hp = target->query("hp") - amount;
    if (hp < 0) hp = 0;
    target->set("hp", hp);
}

void apply_heal(object target, int amount)
{
    int hp, max;
    if (!target) return;
    hp = target->query("hp") + amount;
    max = target->query("max_hp");
    if (hp > max) hp = max;
    target->set("hp", hp);
}

void apply_sp(object target, int amount)
{
    int sp, max;
    if (!target) return;
    sp = target->query("sp") + amount;
    max = target->query("max_sp");
    if (sp > max) sp = max;
    target->set("sp", sp);
}
