// /std/net/websocket_server.c
// 現代化 WebSocket Server 基類 (支援 H5/跨平台 App 直連)

#define STATE_HTTP 0
#define STATE_WS   1

static string in_buf;
static int ws_state;

// 前向宣告
void parse_http_handshake();
void parse_ws_frame();
void on_ws_message(string msg);
void send_ws(string msg);

void create() {
    in_buf = "";
    ws_state = STATE_HTTP;
}

void logon() {}

void process_input(mixed data) {
    string str_data;
    if (typeof(data) == "buffer") {
        str_data = "";
        // 將 buffer 逐字元轉為 string，確保 0x00 和二進制數據不丟失
        for(int i=0; i<sizeof(data); i++) {
            str_data += sprintf("%c", data[i]);
        }
    } else {
        str_data = data + "\n"; // 如果是 ascii port 則補回換行
    }
    
    in_buf += str_data;
    if (ws_state == STATE_HTTP) {
        parse_http_handshake();
    } else {
        parse_ws_frame();
    }
}

void parse_http_handshake() {
    int header_end = strsrch(in_buf, "\r\n\r\n");
    if (header_end == -1) return;
    
    string headers_str = in_buf[0 .. header_end - 1];
    string *lines = explode(headers_str, "\r\n");
    string ws_key = "";
    
    foreach(string line in lines) {
        int pos = strsrch(line, ": ");
        if (pos != -1) {
            string k = lower_case(line[0 .. pos - 1]);
            string v = line[pos + 2 .. sizeof(line) - 1];
            if (k == "sec-websocket-key") ws_key = v;
        }
    }
    
    if (ws_key == "") {
        write("HTTP/1.1 400 Bad Request\r\n\r\n");
        destruct(this_object());
        return;
    }
    
    // 呼叫我們剛做好的底層 OpenSSL Efun！
    string accept = websocket_accept(ws_key);
    
    string res = "HTTP/1.1 101 Switching Protocols\r\n";
    res += "Upgrade: websocket\r\n";
    res += "Connection: Upgrade\r\n";
    res += "Sec-WebSocket-Accept: " + accept + "\r\n\r\n";
    
    write(res);
    ws_state = STATE_WS;
    in_buf = in_buf[header_end + 4 .. sizeof(in_buf) - 1];
    
    debug_message("WS: Handshake successful!\n");
    if (sizeof(in_buf) > 0) parse_ws_frame();
}

void parse_ws_frame() {
    while (sizeof(in_buf) >= 2) {
        int b0 = to_int(in_buf[0]);
        int b1 = to_int(in_buf[1]);
        int opcode = b0 & 0x0F;
        int is_masked = b1 & 0x80;
        int payload_len = b1 & 0x7F;
        int offset = 2;
        
        if (payload_len == 126) {
            if (sizeof(in_buf) < 4) return;
            payload_len = (to_int(in_buf[2]) << 8) | to_int(in_buf[3]);
            offset = 4;
        } else if (payload_len == 127) {
            // 簡化處理：跳過 8 bytes 長度 (H5 遊戲極少單次發送 >64KB)
            if (sizeof(in_buf) < 10) return;
            offset = 10; 
        }
        
        string mask_key = "";
        if (is_masked) {
            if (sizeof(in_buf) < offset + 4 + payload_len) return;
            mask_key = in_buf[offset .. offset + 3];
            offset += 4;
        } else {
            if (sizeof(in_buf) < offset + payload_len) return;
        }
        
        string payload = in_buf[offset .. offset + payload_len - 1];
        
        // XOR 解密 (客戶端發來的數據必須被 Mask)
        if (is_masked && sizeof(mask_key) == 4) {
            string decoded = "";
            for (int i = 0; i < sizeof(payload); i++) {
                decoded += sprintf("%c", to_int(payload[i]) ^ to_int(mask_key[i % 4]));
            }
            payload = decoded;
        }
        
        in_buf = in_buf[offset + payload_len .. sizeof(in_buf) - 1];
        
        if (opcode == 0x1) { // Text Frame
            on_ws_message(payload);
        } else if (opcode == 0x8) { // Close Frame
            destruct(this_object());
            return;
        }
    }
}

// 發送 WS 訊息 (伺服器發送不需要 Mask)
void send_ws(string msg) {
    int len = sizeof(msg);
    string header = "";
    header += sprintf("%c", 0x81); // FIN=1, Opcode=1 (Text)
    if (len < 126) {
        header += sprintf("%c", len);
    } else if (len < 65536) {
        header += sprintf("%c", 126);
        header += sprintf("%c", (len >> 8) & 0xFF);
        header += sprintf("%c", len & 0xFF);
    }
    write(header + msg);
}

// 子類實作業務邏輯
void on_ws_message(string msg) {
    send_ws("Echo: " + msg);
}
