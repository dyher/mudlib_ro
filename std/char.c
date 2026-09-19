// std/char.c - ES2 character object inheriting full RO functionality from player.c
inherit "/std/object/player";

static object my_link;

// Forward declarations
void process_input(string line);
void say_to_room(string msg);

// --- ES2 Link handling ---
void set_link(object link) { my_link = link; }
object query_link() { return my_link; }

// --- ES2 Message Interface ---
void receive_message(string type, string str) {
    if (!interactive(this_object())) return;
    if (str) receive(str);
}

// --- ES2 Setup (called by logind after exec) ---
void setup() {
    seteuid(getuid());
    write("> ");
    input_to("process_input");
}

// --- Player identification ---
int is_player() { return 1; }

// --- Move (called by logind to enter world) ---
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

// --- Multiplayer chat (ES2 message efun) ---
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

// --- Net dead handling ---
void net_dead() {
    if (my_link) { destruct(my_link); my_link = 0; }
}

// --- Command Processor ---
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
    else if (cmd == "stats") show_stats();
    else if (cmd == "save") { save(); write("Saved.\n"); }
    else if (cmd == "kill") {
        if (arg != "") kill_mob(arg);
        else write("Kill what?\n");
    }
    else if (cmd == "talk") {
        if (arg != "") talk_npc(arg);
        else write("Talk to whom?\n");
    }
    else if (cmd == "jobs" || cmd == "job") list_jobs();
    else if (cmd == "skills" || cmd == "skill") list_skills();
    else if (cmd == "learn") {
        if (arg != "") learn_skill(arg);
        else write("Learn what?\n");
    }
    else if (cmd == "cast") {
        if (arg != "") cast_skill(arg);
        else write("Cast what?\n");
    }
    else if (cmd == "monsters") list_mobs();
    else if (cmd == "rest") do_rest();
    else if (cmd == "stat") {
        if (arg != "") allocate_stat(arg);
        else write("Allocate which stat? (str/agi/vit/int/dex/luk)\n");
    }
    else write("Huh? (look/stats/save/quit/say/kill/talk/jobs/skills/learn/cast/monsters/rest/stat)\n");

    write("> ");
    input_to("process_input");
}
