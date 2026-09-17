// adm/obj/login.c — session: auth -> character -> game loop

// Forward prototypes
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
        write("Password: ");
        input_to("get_password");
    } else {
        write("New account. Choose password: ");
        input_to("new_password");
    }
}

void new_password(string line)
{
    if (!line || line == "") { write("Password: "); input_to("new_password"); return; }
    A()->create_account(acc_name, line);
    write("Create character name: ");
    input_to("new_char");
}

void get_password(string line)
{
    string ch;
    if (!A()->check_account(acc_name, line)) {
        write("Wrong password.\nAccount: ");
        input_to("get_account");
        return;
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
    if (!player) {
        write("ERROR: Cannot create player object.\n");
        destruct(this_object());
        return;
    }
    player->restore();  // ES2 save.c restore
    write("\nWelcome, " + ch + "!\n");
    game_prompt();
}

void game_prompt()
{
    write("> ");
    input_to("game_input");
}

void game_input(string line)
{
    string cmd, arg;
    if (!line || line == "") { game_prompt(); return; }
    if (sscanf(line, "%s %s", cmd, arg) != 2) { cmd = line; arg = ""; }

    if (cmd == "look") { write("You are in Prontera Square. (placeholder)\n"); }
    else if (cmd == "jobs") { player->list_jobs(); }
    else if (cmd == "monsters" || cmd == "mobs") { player->list_mobs(); }
    else if (cmd == "kill" || cmd == "attack") { player->kill_mob(arg); }
    else if (cmd == "rest") { player->do_rest(); }
    else if (cmd == "job") { player->choose_job(to_int(arg)); }
    else if (cmd == "stats") { player->show_stats(); }
    else if (cmd == "save") { player->save(); write("Saved.\n"); }
    else if (cmd == "iteminfo") { call_other("/cmds/iteminfo", "main", arg); }
    else if (cmd == "quit") { player->save(); write("Bye!\n"); destruct(this_object()); return; }
    else { write("Huh? (jobs/job/monsters/kill/rest/stats/save/quit)\n"); }

    game_prompt();
}

void net_dead() { destruct(this_object()); }
