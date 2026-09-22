inherit "/std/net/http_client";

void create() {
    ::create();
    debug_message("HTTP_CLIENT: Sending async GET to httpbin.org...\n");
    http_get("https://httpbin.org/get", "on_response");
}

void on_response(int status, string body) {
    debug_message("HTTP_CLIENT: ✅ Response received! Status: " + status + "\n");
    debug_message("HTTP_CLIENT: Body preview: " + body[0..100] + "...\n");
}
