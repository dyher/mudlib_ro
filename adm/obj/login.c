// adm/obj/login.c — session: auth -> character -> game loop

// Forward prototypes
void enter_game(string ch);
void game_prompt();
void game_input(string line);

object player;
string acc_name;

object A() {
    object acc = find_object("/std/system/accountd");
    if (!acc) debug_message("[login] accountd NOT FOUND!\n");
    return acc;
}

void logon()
{
    debug_message("[login] logon() called\n");
    write("\n== Neolith RO ==\n");
    write("Account: ");
    input_to("get_account");
}

void get_account(string line)
{
    debug_message("[login] get_account: " + line + "\n");
    if (!line || line == "") { write("Account: "); input_to("get_account"); return; }
    acc_name = line;  // lower_case removed for compat
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
    debug_message("[login] new_password\n");
    if (!line || line == "") { write("Password: "); input_to("new_password"); return; }
    A()->create_account(acc_name, line);
    write("Create character name: ");
    input_to("new_char");
}

void get_password(string line)
{
    string ch;
    debug_message("[login] get_password\n");
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
    debug_message("[login] new_char: " + line + "\n");
    if (!line || line == "") { write("Character name: "); input_to("new_char"); return; }
    A()->set_char(acc_name, line);
    enter_game(line);
}

void enter_game(string ch)
{
    debug_message("[login] enter_game: " + ch + "\n");
    player = call_other("/adm/obj/master", "create_player_for", ch);
    if (!player) {
        debug_message("[login] ERROR: master->create_player_for() failed!\n");
        write("ERROR: Cannot create player object.\n");
        destruct(this_object());
        return;
    }
    debug_message("[login] player created: " + sprintf("%O", player) + "\n");
    player->restore();
    write("\nWelcome, " + ch + "!\n");
    game_prompt();
}

void game_prompt()
{
    debug_message("[login] game_prompt\n");
    write("> ");
    input_to("game_input");
}

void game_input(string line)
{
    string cmd, arg;
    debug_message("[login] game_input: " + line + "\n");
    if (!line || line == "") { game_prompt(); return; }
    if (sscanf(line, "%s %s", cmd, arg) != 2) { cmd = line; arg = ""; }

    if (cmd == "look") { player->look(); }
    else if (cmd == "stats") { player->show_stats(); }
    else if (cmd == "save") { player->save(); write("Saved.\n"); }
    else if (cmd == "iteminfo") { call_other("/cmds/iteminfo", "main", arg); }
    else if (cmd == "quit") { player->save(); write("Bye!\n"); destruct(this_object()); return; }
    else { write("Huh? (try: look/stats/save/iteminfo/quit)\n"); }

    game_prompt();
}

void net_dead() { destruct(this_object()); }
