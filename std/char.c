// std/char.c - ES2 style character object
// This becomes the interactive object after exec()

inherit "/feature/name";
inherit "/feature/save";
inherit "/feature/move";

static int is_character;
static object my_link;  // reference to the login object

void create()
{
    is_character = 1;
}

int is_player() { return 1; }

// ES2 style: driver's message() efun calls this to deliver messages
void receive_message(string type, string str)
{
    if (!interactive(this_object())) return;
    if (str) receive(str);
}
int is_character() { return 1; }

void set_link(object link)
{
    if (geteuid(previous_object()) != "Root") return 0;
    my_link = link;
}

object query_link() { return my_link; }

nomask string query_save_file()
{
    string id = query("id");
    if (!stringp(id)) return 0;
    return "/data/users/" + id[0..0] + "/" + id;
}

void setup()
{
    seteuid(getuid());
    enable_player();
    init_command();
}

// Override net_dead for disconnect handling
void net_dead()
{
    write("Connection lost.\n");
    // Could implement link-dead mode here
    destruct(this_object());
    if (my_link) destruct(my_link);
}

// Command handler
void process_input(string line)
{
    string cmd, arg;
    
    if (!line || line == "") return;
    
    if (sscanf(line, "%s %s", cmd, arg) != 2) {
        cmd = line;
        arg = "";
    }
    cmd = lower_case(cmd);
    
    if (cmd == "quit") {
        save();
        write("Bye!\n");
        if (my_link) destruct(my_link);
        destruct(this_object());
        return;
    }
    else if (cmd == "look" || cmd == "l") {
        look_room();
    }
    else if (cmd == "north" || cmd == "n") {
        move_player("north");
    }
    else if (cmd == "south" || cmd == "s") {
        move_player("south");
    }
    else if (cmd == "east" || cmd == "e") {
        move_player("east");
    }
    else if (cmd == "west" || cmd == "w") {
        move_player("west");
    }
    else if (cmd == "stats") {
        show_stats();
    }
    else if (cmd == "save") {
        save();
        write("Saved.\n");
    }
    else if (cmd == "say" || cmd == "'") {
        if (arg != "") say_to_room(arg);
        else write("Say what?\n");
    }
    else {
        write("Huh? (look/stats/save/quit/say/north/south/east/west)\n");
    }
}

void show_stats()
{
    write("=== " + name() + " ===\n");
    write("Level: " + query("base_level") + "\n");
    write("HP: " + query("hp") + "/" + query("max_hp") + "\n");
    write("SP: " + query("sp") + "/" + query("max_sp") + "\n");
    write("Zeny: " + query("zeny") + "\n");
}

// Multiplayer functions
string *list_players_in_room()
{
    object room = environment(this_object());
    object *inv;
    string *names = ({});
    int i;
    if (!room) return names;
    inv = all_inventory(room);
    for (i = 0; i < sizeof(inv); i++) {
        if (objectp(inv[i]) && inv[i] != this_object() && inv[i]->is_player())
            names += ({ inv[i]->name() });
    }
    return names;
}

void say_to_room(string msg)
{
    object room = environment(this_object());
    object *inv;
    int i;
    if (!room) return;
    write("You say: " + msg + "\n");
    inv = all_inventory(room);
    for (i = 0; i < sizeof(inv); i++) {
        if (objectp(inv[i]) && inv[i] != this_object() && inv[i]->is_player()) {
            // ES2 style: use message() efun which calls receive_message()
            message("say", name() + " says: " + msg + "\n", inv[i]);
        }
    }
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
    
    {
        string *others = list_players_in_room();
        if (sizeof(others) > 0)
            write("Players here: " + implode(others, ", ") + "\n");
    }
    
    // Show NPCs
    mapping npcs = room->query("npcs");
    if (mapp(npcs) && sizeof(npcs) > 0) {
        string *nnames = keys(npcs);
        write("NPCs: " + implode(nnames, ", ") + "\n");
    }
}

void move_player(string dir)
{
    object room = environment(this_object());
    mapping exits;
    string dest;
    object new_room;
    string err;
    
    if (!room) return;
    exits = room->query("exits");
    if (!mapp(exits) || undefinedp(exits[dir])) {
        write("You can't go that way.\n");
        return;
    }
    dest = exits[dir];
    err = catch(new_room = load_object(dest));
    if (err || !new_room) {
        write("Failed to load room.\n");
        return;
    }
    move_object(new_room);
    look_room();
}
