// /adm/obj/ws_api.c
inherit "/std/net/websocket_server";

void on_ws_message(string msg) {
    debug_message("WS_API: Received -> " + msg + "\n");
    send_ws("Lithos received: " + msg);
}
