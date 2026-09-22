// /std/net/http_client.c
// 現代化非同步 HTTP Client 基類 (底層由 C++ libcurl 驅動)

mapping requests;

void create() {
    requests = ([]);
}

// 發起 GET 請求
void http_get(string url, string callback_func) {
    int id = start_http_request(url, "GET", "");
    requests[id] = callback_func;
    call_out("poll_results", 1);
}

// 發起 POST 請求
void http_post(string url, string body, string callback_func) {
    int id = start_http_request(url, "POST", body);
    requests[id] = callback_func;
    call_out("poll_results", 1);
}

// 輪詢結果 (不阻塞主線程)
void poll_results() {
    int has_pending = 0;
    foreach(int id, string func in requests) {
        mixed res = poll_http_result(id);
        if (res) {
            int status = res[0];
            string body = res[1];
            // 觸發回調函數
            call_other(this_object(), func, status, body);
            map_delete(requests, id);
        } else {
            has_pending = 1;
        }
    }
    if (has_pending) call_out("poll_results", 1);
}
