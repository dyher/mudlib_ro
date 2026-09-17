// std/system/npcscript.c — rAthena NPC script interpreter (stateful)
// Supports: mes, next, close, menu/select, goto, getitem, heal, zeny
// Dialog state lives in the player temp dbase; pauses via waiting flag.

private string trim(string s);
private string unquote(string s);
private void exec_loop(object player);
private int exec_line(object player, string line);
private void clear_state(object player);
void run_script(object player, string path);
void handle_input(object player, string line);
int in_dialog(object player);

private string trim(string s)
{
    int start = 0, end = strlen(s) - 1;
    while (start <= end && (s[start]==32 || s[start]==9 || s[start]==10 || s[start]==13)) start++;
    while (end >= start && (s[end]==32 || s[end]==9 || s[end]==10 || s[end]==13)) end--;
    if (start > end) return "";
    return s[start..end];
}

private string unquote(string s)
{
    string q = sprintf("%c", 34);
    if (strlen(s) >= 2 && s[0..0] == q && s[strlen(s)-1..] == q)
        return s[1..strlen(s)-2];
    return s;
}

int in_dialog(object player)
{
    mapping st = player->query_temp("npc_script");
    return mapp(st) && st["waiting"] > 0;
}

private void clear_state(object player)
{
    player->delete_temp("npc_script");
}

void run_script(object player, string path)
{
    string content, body;
    int bstart, bend, i;
    string *lines;
    string *cmds;
    mapping labels;

    content = read_file(path);
    if (!content) { write("(script missing)\n"); return; }
    bstart = strsrch(content, "{");
    bend = -1;
    for (i = strlen(content) - 1; i >= 0; i--) {
        if (content[i] == 125) { bend = i; break; }
    }
    if (bstart < 0 || bend < 0 || bend <= bstart) { write("(bad script)\n"); return; }
    body = content[bstart+1..bend-1];

    cmds = ({});
    labels = ([]);
    lines = explode(body, "\n");
    for (i = 0; i < sizeof(lines); i++) {
        string line = trim(lines[i]);
        if (line == "") continue;
        if (line[strlen(line)-1..] == ":") {
            string lname = trim(line[0..strlen(line)-2]);
            labels[lname] = sizeof(cmds);
            continue;
        }
        if (line[strlen(line)-1..] == ";") line = trim(line[0..strlen(line)-2]);
        if (line != "") cmds += ({ line });
    }

    player->set_temp("npc_script", ([
        "cmds": cmds, "pc": 0, "labels": labels, "waiting": 0, "menu_labels": ({})
    ]));
    exec_loop(player);
}

void handle_input(object player, string line)
{
    mapping st = player->query_temp("npc_script");
    if (!mapp(st)) return;
    if (st["waiting"] == 1) {
        st["waiting"] = 0;
        player->set_temp("npc_script", st);
        exec_loop(player);
    } else if (st["waiting"] == 2) {
        int choice = to_int(line);
        string *opts = st["menu_labels"];
        if (choice >= 1 && choice <= sizeof(opts)) {
            string label = opts[choice-1];
            st["waiting"] = 0;
            if (!undefinedp(st["labels"][label])) st["pc"] = st["labels"][label];
            player->set_temp("npc_script", st);
            exec_loop(player);
        } else {
            write("Please enter a number between 1 and " + sizeof(opts) + ".\n");
        }
    }
}

private void exec_loop(object player)
{
    while (1) {
        mapping st = player->query_temp("npc_script");
        string *cmds;
        int pc, r;
        if (!mapp(st)) return;
        cmds = st["cmds"];
        pc = st["pc"];
        if (pc >= sizeof(cmds)) { clear_state(player); return; }
        st["pc"] = pc + 1;
        player->set_temp("npc_script", st);
        r = exec_line(player, cmds[pc]);
        if (r == 1) return;
        if (r == 2) { clear_state(player); return; }
    }
}

/* returns 0=continue, 1=pause(waiting), 2=end */
private int exec_line(object player, string line)
{
    string cmd, args;
    line = trim(line);
    if (line == "") return 0;
    if (sscanf(line, "%s %s", cmd, args) != 2) { cmd = line; args = ""; }
    cmd = lower_case(cmd);

    if (cmd == "mes") {
        write(unquote(args) + "\n");
        return 0;
    } else if (cmd == "next") {
        mapping st = player->query_temp("npc_script");
        st["waiting"] = 1;
        player->set_temp("npc_script", st);
        write(">> (press enter)\n");
        return 1;
    } else if (cmd == "close") {
        return 2;
    } else if (cmd == "menu" || cmd == "select") {
        string *parts = explode(args, ",");
        string *menu_labels = ({});
        mapping st;
        int i;
        write("\n");
        for (i = 0; i + 1 < sizeof(parts); i += 2) {
            string text = unquote(trim(parts[i]));
            string label = trim(parts[i+1]);
            menu_labels += ({ label });
            write((i/2 + 1) + ". " + text + "\n");
        }
        write("Choose (1-" + sizeof(menu_labels) + "): ");
        st = player->query_temp("npc_script");
        st["waiting"] = 2;
        st["menu_labels"] = menu_labels;
        player->set_temp("npc_script", st);
        return 1;
    } else if (cmd == "goto") {
        string label = trim(args);
        mapping st = player->query_temp("npc_script");
        if (!undefinedp(st["labels"][label])) {
            st["pc"] = st["labels"][label];
            player->set_temp("npc_script", st);
        } else {
            write("(script error: unknown label " + label + ")\n");
        }
        return 0;
    } else if (cmd == "getitem") {
        string *parts = explode(args, ",");
        int id = to_int(parts[0]);
        int amount = (sizeof(parts) > 1) ? to_int(parts[1]) : 1;
        object DL = find_object("/std/loader/db_loader");
        mapping item = DL->query_item(id);
        player->add_item(id, amount);
        if (item) write("You received " + amount + "x " + item["name"] + ".\n");
        return 0;
    } else if (cmd == "heal") {
        string *parts = explode(args, ",");
        int hp = to_int(parts[0]);
        int sp = (sizeof(parts) > 1) ? to_int(parts[1]) : 0;
        object GL = find_object("/std/system/game_lib");
        if (hp > 0) GL->apply_heal(player, hp);
        if (sp > 0) GL->apply_sp(player, sp);
        write("You feel refreshed.\n");
        return 0;
    } else if (cmd == "zeny" || cmd == "getzeny") {
        find_object("/std/system/game_lib")->apply_zeny(player, to_int(args));
        return 0;
    }
    return 0;
}
