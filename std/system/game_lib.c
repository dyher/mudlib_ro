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

void apply_zeny(object target, int amount)
{
    int z;
    if (!target) return;
    z = target->query("zeny");
    if (undefinedp(z)) z = 0;
    z += amount;
    if (z < 0) z = 0;
    target->set("zeny", z);
}

int query_zeny(object target)
{
    int z = target->query("zeny");
    return undefinedp(z) ? 0 : z;
}

int can_pay(object target, int amount)
{
    return query_zeny(target) >= amount;
}

/* rAthena 元素相剋矩陣 (簡化版: 0=Neutral, 1=Water, 2=Earth, 3=Fire, 4=Wind)
 * 回傳傷害百分比 (100 = 100%, 150 = 150%, 50 = 50%)
 */
int get_elemental_multiplier(int atk_ele, int def_ele)
{
    if (atk_ele == def_ele) return 50;          // 同屬性減半
    if (atk_ele == 1 && def_ele == 3) return 150; // 水 克 火
    if (atk_ele == 3 && def_ele == 4) return 150; // 火 克 風
    if (atk_ele == 4 && def_ele == 2) return 150; // 風 克 地
    if (atk_ele == 2 && def_ele == 1) return 150; // 地 克 水
    // 反向被剋 (rAthena 中通常是 75% 或 50%，這裡簡化為 75%)
    if (atk_ele == 3 && def_ele == 1) return 75;  // 火 打 水
    if (atk_ele == 4 && def_ele == 3) return 75;  // 風 打 火
    if (atk_ele == 2 && def_ele == 4) return 75;  // 地 打 風
    if (atk_ele == 1 && def_ele == 2) return 75;  // 水 打 地
    return 100; // 無屬性或無關
}

/* 轉職：LPC 物件和 rAthena 腳本都透過這裡 */
void change_job(object player, int job_id)
{
    player->choose_job(job_id);
}
