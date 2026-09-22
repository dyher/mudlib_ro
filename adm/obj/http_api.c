// 強制宣告底層 Efun
string sqlite3_query(int, string);

inherit "/std/net/http_server";
void handle_http(mapping req) {
    if (req["method"] == "POST" && req["path"] == "/api/register") {
        string body = req["body"];
        string user = "", pass = "";
        // 極簡易 JSON 解析 (僅供測試)
        if (sscanf(body, "%*s\"user\":\"%s\"%*s", user) && sscanf(body, "%*s\"pass\":\"%s\"%*s", pass)) {
            if (call_other("/std/system/accountd", "register_account", user, pass)) {
                send_http_response(200, "{\"status\":\"success\", \"msg\":\"Account created\"}");
            } else {
                send_http_response(400, "{\"status\":\"error\", \"msg\":\"User exists or failed\"}");
            }
        } else {
            send_http_response(400, "{\"error\":\"Invalid JSON\"}");
        }
    } else {
        ::handle_http(req);
    }
}
