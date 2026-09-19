// std/object/player.c — RO player (ES2 features + combat/skills/stats)

#include <attribute.h>

inherit "/feature/dbase";
inherit "/feature/attribute";
inherit "/feature/name";
inherit "/feature/save";

// Forward prototypes
void choose_job(int job_id);
void recompute_max();
int calc_atk();
int calc_def();
void gain_exp(int base, int job);
void check_level_up();
void kill_mob(string name);
int allocate_stat(string stat);
int calc_hit();
int calc_flee();
int calc_crit();
int has_skill(int id);
int learn_skill(string name);
void cast_skill(string arg);
void skill_attack_mob(mapping sk, string target);
void add_item(int id, int amount);
int remove_item(int id, int amount);
void loot_mob(int mob_id);
int weapon_atk();
int armor_def();

// ===== Stats (HP/SP) =====
void recompute_max()
{
    int maxhp = 40 + query_attr("vit") * 5 + query("job_hp_bonus", 1) + query("base_level") * 3;
    int maxsp = 10 + query_attr("int") * 3 + query("job_sp_bonus", 1) + query("base_level") * 1;
    set("max_hp", maxhp);
    set("max_sp", maxsp);
    if (query("hp", 1) > maxhp) set("hp", maxhp);
    if (query("sp", 1) > maxsp) set("sp", maxsp);
}

void setup(string n)
{
    set_name(n, ({ "player", "user", n }));
    init_attribute(([ "str":1, "agi":1, "vit":1, "int":1, "dex":1, "luk":1 ]));
    set("base_level", 1);
    set("job_level", 1);
    set("base_exp", 0);
    set("job_exp", 0);
    set("stat_points", 20);
    set("skills", ([]));
    set("inventory", ([]));
    set("equipped", ([]));
    set("zeny", 500);
    choose_job(0);   // start as Novice
}

void choose_job(int job_id)
{
    object JL = find_object("/std/loader/job_loader");
    mapping j;
    if (!JL) { write("Job system unavailable.\n"); return; }
    j = JL->query_job(job_id);
    if (!j) { write("Unknown job ID.\n"); return; }
    set("job", job_id);
    set("job_name", j["name"]);
    set("job_hp_bonus", j["base_hp"]);
    set("job_sp_bonus", j["base_sp"]);
    recompute_max();
    set("hp", query("max_hp"));
    set("sp", query("max_sp"));
}

// ===== Stat Points =====
int query_stat_points() { return query("stat_points", 1); }

int stat_cost(string stat)
{
    int val = query_attr(stat, 1);
    if (val < 10) return 2;
    if (val < 20) return 3;
    if (val < 30) return 4;
    if (val < 40) return 5;
    return 6;
}

int allocate_stat(string stat)
{
    int cost, val;
    if (member_array(stat, ({ "str","agi","vit","int","dex","luk" })) < 0) {
        write("Unknown attribute: " + stat + "\n");
        return 0;
    }
    val = query_attr(stat, 1);
    if (val >= ATTRVAL_MAX) {
        write("Your " + stat + " is already at max.\n");
        return 0;
    }
    cost = stat_cost(stat);
    if (query_stat_points() < cost) {
        write("Not enough stat points (need " + cost + ").\n");
        return 0;
    }
    set_attr(stat, val + 1);
    set("stat_points", query_stat_points() - cost);
    recompute_max();
    write("Your " + stat + " is now " + query_attr(stat) + ". ");
    write("(cost " + cost + ", " + query_stat_points() + " left)\n");
    return 1;
}

// ===== Combat =====
int calc_atk()
{
    return 10 + query_attr("str") * 2 + query_attr("dex") + query("base_level") * 2 + weapon_atk();
}

int calc_def()
{
    return query_attr("vit") / 2 + query("base_level") + armor_def();
}

int calc_hit()
{
    return query_attr("dex") + query("base_level");
}

int calc_flee()
{
    return query_attr("agi") + query("base_level");
}

int calc_crit()
{
    return query_attr("luk") / 3 + 1;
}

void kill_mob(string name)
{
    object room = environment(this_object());
    object ML = find_object("/std/loader/mob_loader");
    mapping mob;
    if (room && room->query("no_combat")) {
        write("You can't fight in this safe area.\n");
        return;
    }
    int mob_hp, p_hp, p_atk, p_def, rounds;
    if (!ML) { write("Monster system unavailable.\n"); return; }
    mob = ML->query_mob_by_name(name);
    if (!mob) { write("No such monster: " + name + "\n"); return; }
    {
        int *room_mobs = room ? room->query("mobs") : 0;
        if (pointerp(room_mobs) && member_array(mob["id"], room_mobs) < 0) {
            write("There is no " + mob["name"] + " here.\n");
            return;
        }
    }
    mob_hp = mob["hp"];
    p_hp = query("hp");
    p_atk = calc_atk();
    p_def = calc_def();
    rounds = 0;
    write("A wild " + mob["name"] + " (Lv." + mob["level"] + ") appears!\n");
    while (mob_hp > 0 && p_hp > 0 && rounds < 50) {
        int dmg = p_atk - mob["def"];
        int mdmg;
        if (dmg < 1) dmg = 1;
        mob_hp -= dmg;
        write("You hit " + mob["name"] + " for " + dmg + " dmg.");
        if (mob_hp <= 0) { write("\n"); break; }
        mdmg = mob["atk"] - p_def;
        if (mdmg < 1) mdmg = 1;
        p_hp -= mdmg;
        write("  " + mob["name"] + " hits you for " + mdmg + " dmg.\n");
        rounds++;
    }
    if (mob_hp <= 0) {
        int z = mob["zeny_min"] + random(mob["zeny_max"] - mob["zeny_min"] + 1);
        write("You defeated the " + mob["name"] + "!\n");
        gain_exp(mob["base_exp"], mob["job_exp"]);
        if (z > 0) {
            find_object("/std/system/game_lib")->apply_zeny(this_object(), z);
            write("  You got " + z + " Zeny.\n");
        }
        loot_mob(mob["id"]);
    } else {
        write("You were defeated!\n");
        p_hp = query("max_hp") / 10;
    }
    if (p_hp < 1) p_hp = 1;
    set("hp", p_hp);
}

// ===== Inventory / Equipment / Items =====
int query_item_amount(int id)
{
    mapping inv = query("inventory");
    if (!mapp(inv)) return 0;
    return inv[id] ? inv[id] : 0;
}

void add_item(int id, int amount)
{
    mapping inv = query("inventory");
    if (!mapp(inv)) inv = ([]);
    inv[id] = (inv[id] ? inv[id] : 0) + amount;
    set("inventory", inv);
}

int remove_item(int id, int amount)
{
    mapping inv = query("inventory");
    if (!mapp(inv) || !inv[id] || inv[id] < amount) return 0;
    inv[id] -= amount;
    if (inv[id] <= 0) map_delete(inv, id);
    set("inventory", inv);
    return 1;
}

void loot_mob(int mob_id)
{
    object DR = find_object("/std/loader/drop_loader");
    object DL = find_object("/std/loader/db_loader");
    mapping *drops;
    int i;
    if (!DR) return;
    drops = DR->query_drops(mob_id);
    if (!pointerp(drops)) return;
    for (i = 0; i < sizeof(drops); i++) {
        mapping d = drops[i];
        if (random(10000) < d["rate"]) {
            add_item(d["item_id"], 1);
            mapping item = DL->query_item(d["item_id"]);
            write("  Dropped: " + (item ? item["name"] : "an item") + "\n");
        }
    }
}

int weapon_atk()
{
    object DL = find_object("/std/loader/db_loader");
    mapping eq = query("equipped");
    mapping item;
    if (!mapp(eq) || !eq["weapon"]) return 0;
    item = DL->query_item(eq["weapon"]);
    return item ? item["atk"] : 0;
}

int armor_def()
{
    object DL = find_object("/std/loader/db_loader");
    mapping eq = query("equipped");
    mapping item;
    if (!mapp(eq) || !eq["armor"]) return 0;
    item = DL->query_item(eq["armor"]);
    return item ? item["def"] : 0;
}

void list_inventory()
{
    object DL = find_object("/std/loader/db_loader");
    mapping inv = query("inventory");
    mapping eq = query("equipped");
    int *ids;
    int i;
    if (!mapp(inv) || sizeof(inv) == 0) write("Your inventory is empty.\n");
    else {
        ids = keys(inv);
        write("Inventory:\n");
        for (i = 0; i < sizeof(ids); i++) {
            mapping item = DL->query_item(ids[i]);
            write("  " + (item ? item["name"] : "#" + ids[i]) + " x" + inv[ids[i]] + "\n");
        }
    }
    if (mapp(eq) && (eq["weapon"] || eq["armor"])) {
        write("Equipped:\n");
        if (eq["weapon"]) { mapping w = DL->query_item(eq["weapon"]); write("  Weapon: " + (w ? w["name"] : "?") + "\n"); }
        if (eq["armor"])  { mapping a = DL->query_item(eq["armor"]);  write("  Armor: "  + (a ? a["name"] : "?") + "\n"); }
    }
}

void equip_item(string name)
{
    object DL = find_object("/std/loader/db_loader");
    mapping item = DL->query_item_by_name(name);
    mapping eq;
    string slot;
    if (!item) { write("No such item: " + name + "\n"); return; }
    if (item["type"] != 4 && item["type"] != 5) { write(item["name"] + " is not equippable.\n"); return; }
    if (query_item_amount(item["id"]) < 1) { write("You don't have a " + item["name"] + ".\n"); return; }
    slot = (item["type"] == 4) ? "weapon" : "armor";
    eq = query("equipped");
    if (!mapp(eq)) eq = ([]);
    if (eq[slot]) add_item(eq[slot], 1);   // 歸還舊裝備
    remove_item(item["id"], 1);
    eq[slot] = item["id"];
    set("equipped", eq);
    write("You equipped " + item["name"] + ".\n");
}

void unequip_slot(string slot)
{
    mapping eq = query("equipped");
    if (!mapp(eq) || !eq[slot]) { write("Nothing equipped in " + slot + ".\n"); return; }
    add_item(eq[slot], 1);
    map_delete(eq, slot);
    set("equipped", eq);
    write("You unequipped " + slot + ".\n");
}

void use_item(string name)
{
    object DL = find_object("/std/loader/db_loader");
    object GL = find_object("/std/system/game_lib");
    mapping item = DL->query_item_by_name(name);
    int heal;
    if (!item) { write("No such item: " + name + "\n"); return; }
    if (item["type"] != 0) { write("You can't use " + item["name"] + " like that.\n"); return; }
    if (query_item_amount(item["id"]) < 1) { write("You don't have a " + item["name"] + ".\n"); return; }
    remove_item(item["id"], 1);
    heal = query("max_hp") * 30 / 100;
    GL->apply_heal(this_object(), heal);   // ← 統一函數庫
    write("You used " + item["name"] + " and recovered " + heal + " HP.\n");
}



// ===== Shop / Zeny =====
void list_shop(string shop_key)
{
    object SP = find_object("/std/loader/shop_loader");
    object DL = find_object("/std/loader/db_loader");
    mapping *items;
    int i;
    if (!SP) { write("Shop system unavailable.\n"); return; }
    items = SP->query_shop(shop_key);
    if (!pointerp(items) || sizeof(items) == 0) { write("Nothing to sell here.\n"); return; }
    write("=== " + capitalize(shop_key) + " Shop ===\n");
    write("Your Zeny: " + query("zeny") + "z\n");
    for (i = 0; i < sizeof(items); i++) {
        mapping s_item = items[i];
        mapping item = DL->query_item(s_item["item_id"]);
        int price = s_item["price"] ? s_item["price"] : (item ? item["buy"] : 0);
        if (item)
            write("  " + item["name"] + " - " + price + "z\n");
    }
    write("Use: buy <item> [amount], sell <item> [amount]\n");
}

// 解析 "物品名 [數量]" -> ({ name, amount })，從右往左找最後的數字
mixed *parse_item_arg(string arg)
{
    string name, tail;
    int amount = 1;
    int sp, i, allnum;
    if (!arg || arg == "") return ({ "", 1 });
    name = arg;
    sp = -1;
    for (i = strlen(arg) - 1; i >= 0; i--) {
        if (arg[i] == ' ') { sp = i; break; }
    }
    if (sp > 0) {
        tail = arg[sp+1..];
        allnum = (strlen(tail) > 0);
        for (i = 0; i < strlen(tail); i++) {
            if (tail[i] < '0' || tail[i] > '9') { allnum = 0; break; }
        }
        if (allnum) {
            amount = to_int(tail);
            name = arg[0..sp-1];
        }
    }
    if (amount < 1) amount = 1;
    return ({ name, amount });
}

void buy_item(string arg)
{
    object SP = find_object("/std/loader/shop_loader");
    object DL = find_object("/std/loader/db_loader");
    object GL = find_object("/std/system/game_lib");
    mixed *pa = parse_item_arg(arg);
    string name = pa[0];
    int amount = pa[1];
    mapping *items;
    mapping item;
    int price, total, i;
    items = SP->query_shop("merchant");
    if (!pointerp(items)) { write("Shop unavailable.\n"); return; }
    item = DL->query_item_by_name(name);
    if (!item) { write("No such item: " + name + "\n"); return; }
    price = item["buy"];
    for (i = 0; i < sizeof(items); i++) {
        if (items[i]["item_id"] == item["id"]) {
            if (items[i]["price"]) price = items[i]["price"];
            break;
        }
    }
    total = price * amount;
    if (!GL->can_pay(this_object(), total)) {
        write("Not enough Zeny (need " + total + "z, have " + GL->query_zeny(this_object()) + "z).\n");
        return;
    }
    GL->apply_zeny(this_object(), -total);
    add_item(item["id"], amount);
    write("Bought " + amount + "x " + item["name"] + " for " + total + "z.\n");
}

void sell_item(string arg)
{
    object DL = find_object("/std/loader/db_loader");
    object GL = find_object("/std/system/game_lib");
    mixed *pa = parse_item_arg(arg);
    string name = pa[0];
    int amount = pa[1];
    mapping item;
    int price, total;
    item = DL->query_item_by_name(name);
    if (!item) { write("No such item: " + name + "\n"); return; }
    if (query_item_amount(item["id"]) < amount) {
        write("You don't have that many.\n"); return;
    }
    price = item["sell"];
    total = price * amount;
    remove_item(item["id"], amount);
    GL->apply_zeny(this_object(), total);
    write("Sold " + amount + "x " + item["name"] + " for " + total + "z.\n");
}

// ===== Skills =====
int has_skill(int id)
{
    mapping learned = query("skills");
    return mapp(learned) && learned[id];
}

int learn_skill(string name)
{
    object SL = find_object("/std/loader/skill_loader");
    mapping sk, learned;
    if (!SL) { write("Skill system unavailable.\n"); return 0; }
    sk = SL->query_skill_by_name(name);
    if (!sk) { write("Unknown skill: " + name + "\n"); return 0; }
    if (sk["job_id"] != query("job")) {
        write("Your job (" + query("job_name") + ") cannot learn " + sk["name"] + ".\n");
        return 0;
    }
    learned = query("skills");
    if (!mapp(learned)) learned = ([]);
    if (learned[sk["id"]]) { write("You already know " + sk["name"] + ".\n"); return 0; }
    learned[sk["id"]] = 1;
    set("skills", learned);
    write("You learned " + sk["name"] + "!\n");
    return 1;
}

void cast_skill(string arg)
{
    object SL = find_object("/std/loader/skill_loader");
    object GL = find_object("/std/system/game_lib");
    mapping sk;
    mixed *found;
    string tgt_name;
    if (!arg || arg == "") { write("Cast what?
"); return; }
    if (!SL) { write("Skill system unavailable.
"); return; }
    found = SL->find_skill_in_string(arg);
    if (!found) { write("Unknown skill: " + arg + "
"); return; }
    sk = SL->query_skill(found[0]);
    tgt_name = found[1];
    if (!has_skill(sk["id"])) { write("You haven't learned " + sk["name"] + ".
"); return; }
    if (query("sp", 1) < sk["sp_cost"]) { write("Not enough SP.
"); return; }
    set("sp", query("sp", 1) - sk["sp_cost"]);
    if (sk["type"] == "heal") {
        int amount = sk["power"] + query_attr("int") * 4;
        GL->apply_heal(this_object(), amount);
        write("You cast " + sk["name"] + " and recover " + amount + " HP.
");
    } else if (sk["type"] == "attack") {
        skill_attack_mob(sk, tgt_name);
    }
}

void skill_attack_mob(mapping sk, string target)
{
    object ML = find_object("/std/loader/mob_loader");
    mapping mob;
    int mob_hp, p_hp, sk_dmg, rounds;
    if (!ML) return;
    mob = ML->query_mob_by_name(target);
    if (!mob) { write("No such monster: " + target + "\n"); return; }

    sk_dmg = calc_atk() * sk["power"] / 100;
    {
        int ele_mult = find_object("/std/system/game_lib")->get_elemental_multiplier(sk["element"], mob["element"]);
        sk_dmg = sk_dmg * ele_mult / 100;
        write("You cast " + sk["name"] + " on " + mob["name"] + " [Ele" + mob["element"] + "] ");
        if (ele_mult >= 150) write("(WEAKNESS! " + ele_mult + "%) ");
        else if (ele_mult <= 75) write("(Resisted " + ele_mult + "%) ");
        write("for " + sk_dmg + " dmg!\n");
    }
    mob_hp = mob["hp"] - sk_dmg;
    p_hp = query("hp");
    rounds = 0;

    while (mob_hp > 0 && p_hp > 0 && rounds < 50) {
        int dmg = calc_atk() - mob["def"];
        int mdmg;
        if (dmg < 1) dmg = 1;
        mob_hp -= dmg;
        write("You hit " + mob["name"] + " for " + dmg + " dmg.");
        if (mob_hp <= 0) { write("\n"); break; }
        mdmg = mob["atk"] - calc_def();
        if (mdmg < 1) mdmg = 1;
        p_hp -= mdmg;
        write("  " + mob["name"] + " hits you for " + mdmg + " dmg.\n");
        rounds++;
    }
    if (mob_hp <= 0) {
        int z = mob["zeny_min"] + random(mob["zeny_max"] - mob["zeny_min"] + 1);
        write("You defeated the " + mob["name"] + "!\n");
        gain_exp(mob["base_exp"], mob["job_exp"]);
        if (z > 0) {
            find_object("/std/system/game_lib")->apply_zeny(this_object(), z);
            write("  You got " + z + " Zeny.\n");
        }
    } else {
        write("You were defeated!\n");
        p_hp = query("max_hp") / 10;
    }
    if (p_hp < 1) p_hp = 1;
    set("hp", p_hp);
}

void list_skills()
{
    object SL = find_object("/std/loader/skill_loader");
    mapping skills, learned;
    int *ids, i;
    if (!SL) { write("Skill system unavailable.\n"); return; }
    skills = SL->query_all_skills();
    learned = query("skills");
    if (!mapp(learned)) learned = ([]);
    ids = keys(skills);
    write("Skills (your job: " + query("job_name") + "):\n");
    for (i = 0; i < sizeof(ids); i++) {
        mapping s = skills[ids[i]];
        string status = learned[s["id"]] ? "[learned]" :
                        (s["job_id"] == query("job") ? "[learnable]" : "[other job]");
        write("  " + s["name"] + " (" + s["type"] + ", SP:" + s["sp_cost"] + ") " + status + "\n");
    }
    write("Use: learn <name>, cast <name> [target]\n");
}

// ===== Experience / Leveling =====
void gain_exp(int base, int job)
{
    set("base_exp", query("base_exp") + base);
    set("job_exp", query("job_exp") + job);
    write("You gained " + base + " base EXP and " + job + " job EXP.\n");
    check_level_up();
}

void check_level_up()
{
    int lvl = query("base_level");
    int need = lvl * 50;
    while (query("base_exp") >= need) {
        set("base_exp", query("base_exp") - need);
        lvl++;
        set("base_level", lvl);
        need = lvl * 50;
        set("stat_points", query("stat_points", 1) + 5);
        write("*** LEVEL UP! Base level " + lvl + "! (+5 stat points) ***\n");
        recompute_max();
        set("hp", query("max_hp"));
        set("sp", query("max_sp"));
    }
}

// ===== Utility =====
void do_rest()
{
    set("hp", query("max_hp"));
    set("sp", query("max_sp"));
    write("You rest and recover fully.\n");
}

string query_save_file()
{
    return "/data/players/" + query("name");
}

void show_stats()
{
    mapping learned = query("skills");
    write("=== " + name() + " the " + query("job_name") + " ===\n");
    write("Base Lv: " + query("base_level") + "   Job Lv: " + query("job_level") + "\n");
    write("Base EXP: " + query("base_exp") + "   Job EXP: " + query("job_exp") + "\n");
    write("Stat Points: " + query_stat_points() + "   Skills: " + (mapp(learned) ? sizeof(learned) : 0) + "\n");
    write("HP: " + query("hp") + "/" + query("max_hp") + "   SP: " + query("sp") + "/" + query("max_sp") + "\n");
    write(sprintf("ATK: %d   DEF: %d   HIT: %d   FLEE: %d   CRIT: %d\n", calc_atk(), calc_def(), calc_hit(), calc_flee(), calc_crit()));
    write("\nAttributes (use 'stat <name>'):\n");
    write("STR: " + query_attr("str") + "  AGI: " + query_attr("agi") + "  VIT: " + query_attr("vit") + "\n");
    write("INT: " + query_attr("int") + "  DEX: " + query_attr("dex") + "  LUK: " + query_attr("luk") + "\n");
}

void list_jobs()
{
    object JL = find_object("/std/loader/job_loader");
    mapping jobs;
    int *ids;
    int i;
    if (!JL) { write("Job system unavailable.\n"); return; }
    jobs = JL->query_all_jobs();
    ids = keys(jobs);
    write("Available jobs (rAthena job IDs):\n");
    for (i = 0; i < sizeof(ids); i++)
        write("  " + ids[i] + " - " + jobs[ids[i]]["name"] + "\n");
    write("Use: job <id>\n");
}

void list_mobs()
{
    object ML = find_object("/std/loader/mob_loader");
    mapping mobs;
    int *ids;
    int i;
    if (!ML) { write("Monster system unavailable.\n"); return; }
    mobs = ML->query_all_mobs();
    ids = keys(mobs);
    write("Nearby monsters (rAthena mob IDs):\n");
    for (i = 0; i < sizeof(ids); i++) {
        mapping m = mobs[ids[i]];
        write("  " + m["name"] + " (Lv." + m["level"] + ", HP:" + m["hp"] + ", EXP:" + m["base_exp"] + ")\n");
    }
    write("Use: kill <name>\n");
}

// ===== Map / Movement =====
void move_to_room(object room)
{
    if (room) move_object(room);
}

void look_room()
{
    object room = environment(this_object());
    mapping exits;
    string *dirs;
    int i;
    if (!room) { write("You are floating in the void.\n"); return; }
    write("[" + room->query("short") + "]\n");
    write(room->query("long") + "\n");
    exits = room->query("exits");
    if (mapp(exits)) {
        dirs = keys(exits);
        write("Exits:");
        for (i = 0; i < sizeof(dirs); i++) write(" " + dirs[i]);
        write("\n");
    }
    if (room) {
        mapping npcs = room->query("npcs");
        if (mapp(npcs) && sizeof(npcs) > 0) {
            string *nnames = keys(npcs);
            string nlist = "";
            int k;
            for (k = 0; k < sizeof(nnames); k++)
                nlist += (nlist == "" ? "" : ", ") + nnames[k];
            write("NPCs: " + nlist + "\n");
        }
    }
    if (room && !room->query("no_combat")) {
        int *mobs = room->query("mobs");
        if (pointerp(mobs) && sizeof(mobs) > 0) {
            object ML = find_object("/std/loader/mob_loader");
            string mnames = "";
            int j;
            for (j = 0; j < sizeof(mobs); j++) {
                mapping m = ML->query_mob(mobs[j]);
                if (m) mnames += (mnames == "" ? "" : ", ") + m["name"];
            }
            write("Monsters: " + mnames + "\n");
        }
    }
}

void move_player(string direction)
{
    object room = environment(this_object());
    object dest;
    mapping exits;
    string dest_path;
    if (!room) { write("You are nowhere.\n"); return; }
    exits = room->query("exits");
    if (!mapp(exits) || !exits[direction]) {
        write("You can't go " + direction + ".\n");
        return;
    }
    dest_path = exits[direction];
    dest = call_other("/adm/obj/master", "load_room", dest_path);
    if (!dest) { write("You can't go there.\n"); return; }
    move_object(dest);
    write("\n");
    look_room();
}

void talk_npc(string name)
{
    object room = environment(this_object());
    object npc;
    mapping npcs;
    string npc_path;
    if (!room) { write("You are nowhere.\n"); return; }
    npcs = room->query("npcs");
    if (!mapp(npcs) || !npcs[lower_case(name)]) {
        write("There is no " + name + " here.\n");
        return;
    }
    npc_path = npcs[lower_case(name)];
    npc = call_other("/adm/obj/master", "load_npc", npc_path);
    if (!npc) { write("You can't talk to " + name + ".\n"); return; }
    npc->on_talk(this_object());
}
