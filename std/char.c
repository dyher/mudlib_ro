// std/char.c - FINAL PERFECT ES2 character object
static object my_link;
static mapping my_data = ([]);

void look_room();
void move_player(string dir);
void say_to_room(string msg);
void process_input(string line);
int move(mixed dest);

mixed set(string prop, mixed value) { my_data[prop] = value; return value; }
mixed query(string prop) { return my_data[prop]; }

string name() { return my_data["name"] || "Unknown"; }
void set_name(string n) { my_data["name"] = n; }

void set_link(object link) { my_link = link; }
object query_link() { return my_link; }

string query_save_file() {
    string id = my_data["id"];
    if (!stringp(id)) return 0;
    return "/data/users/" + id[0..0] + "/" + id;
}

void assure_file(string file) {
    string dir = file[0..strsrch(file, "/", -1) - 1];
    if (file_size(dir) != -2) {
        int i, n;
        string *parts = explode(dir, "/");
        string current = "";
        n = sizeof(parts);
        for (i = 0; i < n; i++) {
            current += "/" + parts[i];
            if (file_size(current) != -2) mkdir(current);
        }
    }
}

int save() {
    string file = query_save_file();
    if (!file) return 0;
    assure_file(file + ".o");
    return save_object(file);
}

int restore() {
    string file = query_save_file();
    if (!file) return 0;
    if (file_size(file + ".o") < 0) return 0;
    return restore_object(file);
}

void receive_message(string type, string str) {
    if (!interactive(this_object())) return;
    if (str) receive(str);
}

void setup() {
    write("> ");
    input_to("process_input");
}

void net_dead() {
    if (my_link) { destruct(my_link); my_link = 0; }
}

int move(mixed dest) {
    int res = 0;
    if (stringp(dest)) {
        object ob = load_object(dest);
        if (ob) { move_object(ob); res = 1; }
    } else if (objectp(dest)) {
        move_object(dest);
        res = 1;
    }
    if (res) {
        look_room();
        write("> ");
    }
    return res;
}

void look_room() {
    object room = environment(this_object());
    mapping exits;
    string *dirs;
    int i;
    if (!room) { write("You are in the void.\n"); return; }
    write("[" + room->query("short") + "]\n");
    write(room->query("long") + "\n");
    exits = room->query("exits");
    if (mapp(exits)) {
        dirs = keys(exits);
        write("Exits:");
        for (i = 0; i < sizeof(dirs); i++) write(" " + dirs[i]);
        write("\n");
    }
    object *inv = all_inventory(room);
    string *others = ({});
    for (i = 0; i < sizeof(inv); i++) {
        if (objectp(inv[i]) && inv[i] != this_object() && inv[i]->is_player())
            others += ({ inv[i]->name() });
    }
    if (sizeof(others) > 0)
        write("Players here: " + implode(others, ", ") + "\n");
}

void move_player(string dir) {
    object room = environment(this_object());
    mapping exits;
    string dest;
    object new_room;
    if (!room) return;
    exits = room->query("exits");
    if (!mapp(exits) || undefinedp(exits[dir])) {
        write("You can't go that way.\n");
        return;
    }
    dest = exits[dir];
    new_room = load_object(dest);
    if (!new_room) { write("Failed to load room.\n"); return; }
    move_object(new_room);
    look_room();
    write("> ");
}

void say_to_room(string msg) {
    object room = environment(this_object());
    object *inv;
    int i;
    if (!room) return;
    write("You say: " + msg + "\n");
    inv = all_inventory(room);
    for (i = 0; i < sizeof(inv); i++) {
        if (objectp(inv[i]) && inv[i] != this_object() && inv[i]->is_player()) {
            message("say", name() + " says: " + msg + "\n", inv[i]);
        }
    }
}

int is_player() { return 1; }

void process_input(string line) {
    string cmd, arg;
    if (!line || line == "") {
        write("> ");
        input_to("process_input");
        return;
    }
    if (sscanf(line, "%s %s", cmd, arg) != 2) { cmd = line; arg = ""; }
    cmd = lower_case(cmd);

    if (cmd == "quit") {
        save();
        write("Bye!\n");
        if (my_link) destruct(my_link);
        destruct(this_object());
        return;
    }
    else if (cmd == "look" || cmd == "l") look_room();
    else if (cmd == "say" || cmd == "'") {
        if (arg != "") say_to_room(arg);
        else write("Say what?\n");
    }
    else if (cmd == "north" || cmd == "n") move_player("north");
    else if (cmd == "south" || cmd == "s") move_player("south");
    else if (cmd == "east"  || cmd == "e") move_player("east");
    else if (cmd == "west"  || cmd == "w") move_player("west");
    else if (cmd == "stats") write("=== " + name() + " ===\nLevel: " + query("base_level") + "\nHP: " + query("hp") + "/" + query("max_hp") + "\n");
    else if (cmd == "save") { save(); write("Saved.\n"); }
    else write("Huh? (look/say/quit/stats/save/north/south/east/west)\n");

    write("> ");
    input_to("process_input");
}
