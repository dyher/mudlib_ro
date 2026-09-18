// feature/multiplayer.c — 多人互動功能 (Neolith ES2 Architecture)

string name();

int is_player() { return 1; }

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
    object login;
    int i;
    if (!room) return;
    write("You say: " + msg + "\n");
    inv = all_inventory(room);
    for (i = 0; i < sizeof(inv); i++) {
        if (objectp(inv[i]) && inv[i] != this_object() && inv[i]->is_player()) {
            login = inv[i]->query("login_obj");
            if (objectp(login)) {
                tell_object(login, name() + " says: " + msg + "\n");
            }
        }
    }
}
