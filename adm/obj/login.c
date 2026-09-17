// adm/obj/login.c — session: auth -> character -> game loop

void enter_game(string ch);
void game_prompt();
void game_input(string line);

object player;
string acc_name;

object A() { return find_object("/std/system/accountd"); }

void logon()
{
    write("\n== Neolith RO (ES2 Architecture) ==\n");
    write("Account: ");
    input_to("get_account");
}

void get_account(string line)
{
    if (!line || line == "") { write("Account: "); input_to("get_account"); return; }
    acc_name = line;
    if (A()->exists_account(acc_name)) {
        write("Password: "); input_to("get_password");
    } else {
        write("New account. Choose password: "); input_to("new_password");
    }
}

void new_password(string line)
{
    if (!line || line == "") { write("Password: "); input_to("new_password"); return; }
    A()->create_account(acc_name, line);
    write("Create character name: "); input_to("new_char");
}

void get_password(string line)
{
    string ch;
    if (!A()->check_account(acc_name, line)) {
        write("Wrong password.\nAccount: "); input_to("get_account"); return;
    }
    ch = A()->query_char(acc_name);
    if (ch) enter_game(ch);
    else { write("Create character name: "); input_to("new_char"); }
}

void new_char(string line)
{
    if (!line || line == "") { write("Character name: "); input_to("new_char"); return; }
    A()->set_char(acc_name, line);
    enter_game(line);
}

void enter_game(string ch)
{
    player = call_other("/adm/obj/master", "create_player_for", ch);
    if (!player) { write("ERROR: Cannot create player.\n"); destruct(this_object()); return; }
    player->restore();
    debug_message("[login] restore done, base_level=" + player->query("base_level") + " skills=" + sizeof(player->query("skills") || ([])) + "\n");
    write("\nWelcome, " + ch + "!\n");
    {
        object start_room = call_other("/adm/obj/master", "load_room", "/d/world/prontera");
        if (start_room) player->move_to_room(start_room);
    }
    write("\n");
    player->look_room();
    game_prompt();
}

void game_prompt()
{
    write("> ");
    input_to("game_input");
}

// 健壯指令解析：cmd = 第一個詞, arg = 其餘全部
void game_input(string line)
{
    string cmd, arg;
    int sp;
    if (!line || line == "") { game_prompt(); return; }
    sp = strsrch(line, " ");
    if (sp == -1) { cmd = line; arg = ""; }
    else { cmd = line[0..sp-1]; arg = line[sp+1..]; }

    if (cmd == "look" || cmd == "l") player->look_room();
    else if (cmd == "move" || cmd == "go") player->move_player(arg);
    else if (cmd == "north" || cmd == "n") player->move_player("north");
    else if (cmd == "south" || cmd == "s") player->move_player("south");
    else if (cmd == "east" || cmd == "e") player->move_player("east");
    else if (cmd == "west" || cmd == "w") player->move_player("west");
    else if (cmd == "jobs") player->list_jobs();
    else if (cmd == "job") player->choose_job(to_int(arg));
    else if (cmd == "monsters" || cmd == "mobs") player->list_mobs();
    else if (cmd == "kill" || cmd == "attack") player->kill_mob(arg);
    else if (cmd == "rest") player->do_rest();
    else if (cmd == "stat") player->allocate_stat(arg);
    else if (cmd == "stats") player->show_stats();
    else if (cmd == "skills") player->list_skills();
    else if (cmd == "inv" || cmd == "inventory" || cmd == "i") player->list_inventory();
    else if (cmd == "equip") player->equip_item(arg);
    else if (cmd == "unequip") player->unequip_slot(arg);
    else if (cmd == "use") player->use_item(arg);
    else if (cmd == "learn") player->learn_skill(arg);
    else if (cmd == "cast") {
        string sk, tgt;
        int s2 = strsrch(arg, " ");
        if (s2 == -1) { sk = arg; tgt = ""; }
        else { sk = arg[0..s2-1]; tgt = arg[s2+1..]; }
        player->cast_skill(sk, tgt);
    }
    else if (cmd == "save") { player->save(); write("Saved.\n"); }
    else if (cmd == "iteminfo") call_other("/cmds/iteminfo", "main", arg);
    else if (cmd == "quit") { player->save(); write("Bye!\n"); destruct(this_object()); return; }
    else write("Huh? (jobs/job/stat/skills/learn/cast/monsters/kill/rest/stats/save/quit)\n");

    game_prompt();
}

void net_dead() { destruct(this_object()); }
