// std/char.c - ES2 style character object
// This becomes the interactive object after exec()

// Inherit the full RO player functionality
inherit "/std/object/player";

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


// Multiplayer functions



