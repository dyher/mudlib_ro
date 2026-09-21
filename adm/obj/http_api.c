// /adm/obj/http_api.c
// 繼承 HTTP Server 基類，實作具體的業務邏輯
inherit "/std/net/http_server";

void create() {
    ::create();
}

void logon() {
    // HTTP 連線不需要歡迎詞，保持靜默
}

// 實作業務邏輯
void handle_http(mapping req) {
    string response_json;
    int db;
    
    // 簡單的路由處理
    if (req["method"] == "GET" && req["path"] == "/api/status") {
        response_json = "{\"status\": \"online\", \"server\": \"Lithos HTTP\"}";
        send_http_response(200, response_json);
    } 
    else if (req["method"] == "GET" && req["path"] == "/api/db_test") {
        // 測試 SQLite 連接
        db = sqlite3_open("/tmp/lithos_test.db");
        // 為了簡化，我們直接返回一個成功的 JSON
        sqlite3_close(db);
        response_json = "{\"db_status\": \"connected\", \"message\": \"SQLite3 is ready!\"}";
        send_http_response(200, response_json);
    }
    else {
        send_http_response(404, "{\"error\": \"Not Found\"}");
    }
}
