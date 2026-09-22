// 強制宣告底層 Efun
mixed sqlite3_exec(int, string);

inherit "/std/net/http_server";

void handle_http(mapping req) {
    if (req["method"] == "POST" && req["path"] == "/api/register") {
        string body = req["body"];
        string user = "", pass = "";
        if (sscanf(body, "%*s\"user\":\"%s\"%*s", user) && sscanf(body, "%*s\"pass\":\"%s\"%*s", pass)) {
            if (call_other("/std/system/accountd", "register_account", user, pass)) {
                send_http_response(200, "{\"status\":\"success\", \"msg\":\"Account created\"}");
            } else {
                send_http_response(400, "{\"status\":\"error\", \"msg\":\"User exists or failed\"}");
            }
        } else {
            send_http_response(400, "{\"error\":\"Invalid JSON\"}");
        }
    } else if (req["method"] == "GET" && req["path"] == "/api/status") {
        send_http_response(200, "{\"status\": \"online\", \"server\": \"Lithos\"}");
    } else {
        ::handle_http(req);
    }
}
