// std/system/npcscript.c — rAthena NPC script interpreter
// Executes rAthena-format NPC scripts by mapping commands onto game_lib.
// This is the bridge that lets rAthena .txt scripts drive our LPC engine.

private string trim(string s);
private string unquote(string s);
private void exec_line(object player, string line);
void run_script(object player, string path);

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

private void exec_line(object player, string line)
{
    string cmd, args;
    line = trim(line);
    if (line == "") return;
    if (sscanf(line, "%s %s", cmd, args) != 2) { cmd = line; args = ""; }
    cmd = lower_case(cmd);

    if (cmd == "mes") {
        write(unquote(args) + "\n");
    } else if (cmd == "getitem") {
        string *parts = explode(args, ",");
        int id = to_int(parts[0]);
        int amount = (sizeof(parts) > 1) ? to_int(parts[1]) : 1;
        object DL = find_object("/std/loader/db_loader");
        mapping item = DL->query_item(id);
        player->add_item(id, amount);
        if (item) write("You received " + amount + "x " + item["name"] + ".\n");
    } else if (cmd == "heal") {
        string *parts = explode(args, ",");
        int hp = to_int(parts[0]);
        int sp = (sizeof(parts) > 1) ? to_int(parts[1]) : 0;
        object GL = find_object("/std/system/game_lib");
        if (hp > 0) GL->apply_heal(player, hp);
        if (sp > 0) GL->apply_sp(player, sp);
        write("You feel refreshed.\n");
    } else if (cmd == "zeny" || cmd == "getzeny") {
        find_object("/std/system/game_lib")->apply_zeny(player, to_int(args));
    } else if (cmd == "close" || cmd == "next") {
        /* end of dialog (inline for now) */
    }
}

void run_script(object player, string path)
{
    string content, body;
    int bstart, bend, i;
    string *cmds;
    content = read_file(path);
    if (!content) { write("(script missing)\n"); return; }
    bstart = strsrch(content, "{");
    bend = -1;
    for (i = strlen(content) - 1; i >= 0; i--) {
        if (content[i] == 125) { bend = i; break; }
    }
    if (bstart < 0 || bend < 0 || bend <= bstart) { write("(bad script)\n"); return; }
    body = content[bstart+1..bend-1];
    cmds = explode(body, ";");
    for (i = 0; i < sizeof(cmds); i++)
        exec_line(player, cmds[i]);
}
