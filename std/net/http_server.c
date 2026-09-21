// /std/net/http_server.c
// 一個通用的、非阻塞的、狀態機驅動的 HTTP Server 基類

// 【關鍵】前向宣告 (Forward Declarations) - 解決 LPC 單次掃描的 Undefined function 問題
void parse_http();
mapping parse_headers(string str);
void handle_http(mapping req);
void send_http_response(int status, string body);
void close_connection();

static string in_buf;

void create() {
    in_buf = "";
}

void logon() {
    // HTTP 連線不需要歡迎詞，保持靜默
}

// 當 PORT_ASCII 收到數據時自動呼叫
void process_input(string data) {
    // 【關鍵修復】PORT_ASCII 會吃掉 \n，我們必須手動加回來以還原 HTTP 的 \r\n
    in_buf += data + "\n";
    parse_http();
}

void parse_http() {
    int header_end;
    string headers_str;
    mapping req;
    int content_length;
    int body_start;
    
    // 【修正】使用 LPC 原生的 strsrch 尋找 Header 結尾 (\r\n\r\n)
    header_end = strsrch(in_buf, "\r\n\r\n");
    if (header_end == -1) {
        return; // Header 還沒收完，等待下一次 process_input
    }
    
    headers_str = in_buf[0 .. header_end - 1];
    req = parse_headers(headers_str);
    
    content_length = req["headers"]["content-length"] ? to_int(req["headers"]["content-length"]) : 0;
    body_start = header_end + 4;
    
    if (sizeof(in_buf) < body_start + content_length) {
        return; // Body 還沒收完
    }
    
    if (content_length > 0) {
        req["body"] = in_buf[body_start .. body_start + content_length - 1];
    } else {
        req["body"] = "";
    }
    
    // 清理已處理的緩衝區
    if (sizeof(in_buf) > body_start + content_length) {
        in_buf = in_buf[body_start + content_length .. sizeof(in_buf) - 1];
    } else {
        in_buf = "";
    }
    
    debug_message("HTTP: Received " + req["method"] + " " + req["path"] + "\n");
    
    // 呼叫業務邏輯
    handle_http(req);
}

mapping parse_headers(string str) {
    mapping req = ([ "method": "", "path": "", "version": "", "headers": ([]) ]);
    string *lines = explode(str, "\r\n");
    string *request_line;
    string *header_pair;
    int i;
    
    if (sizeof(lines) == 0) return req;
    
    request_line = explode(lines[0], " ");
    if (sizeof(request_line) >= 3) {
        req["method"] = request_line[0];
        req["path"] = request_line[1];
        req["version"] = request_line[2];
    }
    
    for (i = 1; i < sizeof(lines); i++) {
        // 使用 strsrch 尋找 ": " 避免 explode 切斷值裡的冒號
        int colon_pos = strsrch(lines[i], ": ");
        if (colon_pos != -1) {
            string key = lower_case(lines[i][0 .. colon_pos - 1]);
            string val = lines[i][colon_pos + 2 .. sizeof(lines[i]) - 1];
            req["headers"][key] = val;
        }
    }
    
    return req;
}

void handle_http(mapping req) {
    send_http_response(404, "{\"error\": \"Not Found\"}");
}

void send_http_response(int status, string body) {
    string status_text;
    string response;
    
    if (status == 200) status_text = "OK";
    else if (status == 404) status_text = "Not Found";
    else status_text = "Internal Server Error";
    
    response = "HTTP/1.1 " + status + " " + status_text + "\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + sizeof(body) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;
    
    write(response);
    call_out("close_connection", 1);
}

void close_connection() {
    destruct(this_object());
}
