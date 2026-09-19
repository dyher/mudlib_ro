// adm/daemons/logind.c - ES2 style login daemon
// Handles authentication and calls exec() to transfer interactive status

#define USER_OB "/std/char"
#define START_ROOM "/d/world/prontera"

static void get_account(string arg, object ob);
static void get_password(string arg, object ob);
static void create_new_user(string name, object ob);

void logon(object ob)
{
    seteuid(getuid());
    write("\n== Neolith RO (ES2 Architecture) ==\n");
    write("Account: ");
    input_to("get_account", ob);
}

static void get_account(string arg, object ob)
{
    if (!arg || arg == "") {
        write("Account: ");
        input_to("get_account", ob);
        return;
    }
    arg = lower_case(arg);
    ob->set("id", arg);
    
    // Check if account exists
    if (file_size(ob->query_save_file() + ".o") > 0) {
        write("Password: ");
        input_to("get_password", 1, ob);
    } else {
        write("New account. Choose password: ");
        input_to("new_password", 1, ob);
    }
}

static void new_password(string pass, object ob)
{
    if (strlen(pass) < 4) {
        write("Password too short (min 4 chars): ");
        input_to("new_password", 1, ob);
        return;
    }
    ob->set("password", crypt(pass, 0));
    write("Create character name: ");
    input_to("create_char", ob);
}

static void create_char(string name, object ob)
{
    if (!name || name == "") {
        write("Character name: ");
        input_to("create_char", ob);
        return;
    }
    ob->set("name", name);
    enter_world(ob);
}

static void get_password(string pass, object ob)
{
    string stored;
    seteuid(getuid());
    ob->restore();
    stored = ob->query("password");
    if (crypt(pass, stored) != stored) {
        write("Wrong password.\n");
        destruct(ob);
        return;
    }
    enter_world(ob);
}

void enter_world(object ob)
{
    object user, room;
    string err;
    
    seteuid(getuid());
    err = catch(user = new(USER_OB));
    if (err || !user) {
        write("Failed to create character.\n");
        destruct(ob);
        return;
    }
    
    seteuid(getuid(ob));
    export_uid(user);
    seteuid(getuid());
    
    user->set("id", ob->query("id"));
    user->set("name", ob->query("name"));
    
    // Link the two
    user->set_link(ob);
    ob->set_body(user);
    
    // *** THE MAGIC: transfer interactive status from login to user ***
    exec(user, ob);
    
    // Setup user (enable commands, heart beat, etc)
    user->setup();
    
    // Try to restore saved data
    if (!user->restore()) {
        // New character - set defaults
        user->set("base_level", 1);
        user->set("job", 0);
        user->set("max_hp", 80);
        user->set("hp", 80);
        user->set("max_sp", 20);
        user->set("sp", 20);
        user->set("str", 1);
        user->set("agi", 1);
        user->set("vit", 1);
        user->set("int", 1);
        user->set("dex", 1);
        user->set("luk", 1);
        user->set("stat_points", 20);
        user->set("zeny", 100);
    }
    
    write("Welcome, " + user->query("name") + "!\n\n");
    
    // Move to start room
    err = catch(room = load_object(START_ROOM));
    if (room) {
        user->move(room);
    } else {
        write("Failed to load start room.\n");
        destruct(ob);
        destruct(user);
    }
}
