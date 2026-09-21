// obj/ro/login.c - RO Protocol Handler (Final Perfect Version)

void handle_packet(int id, buffer pkt);
int get_pkt_len(int id);

static buffer in_buf;

void create() {
    in_buf = allocate_buffer(0);
}

// 解決 ES2 架構的 logon() 呼叫錯誤
void logon() {
    // RO 客戶端不需要歡迎詞，保持靜默
}

int get_pkt_len(int id) {
    switch(id) {
        case 0x0064: return 55;
        case 0x0072: return 19;
        case 0x00A4: return 6;
        case 0x00F3: return 10;
        default: return 0;
    }
}

void process_input(buffer data) {
    int pkt_len, id;
    buffer pkt;
    
    in_buf = in_buf + data;

    while(sizeof(in_buf) >= 2) {
        id = in_buf[0] | (in_buf[1] << 8);
        pkt_len = get_pkt_len(id);
        
        if (pkt_len == 0) {
            debug_message("RO: Unknown packet 0x" + sprintf("%04X", id) + "\n");
            return;
        }
        
        if (sizeof(in_buf) < pkt_len) break; 
        
        pkt = in_buf[0 .. pkt_len - 1];
        handle_packet(id, pkt);
        
        if (sizeof(in_buf) > pkt_len) {
            in_buf = in_buf[pkt_len .. sizeof(in_buf) - 1];
        } else {
            in_buf = allocate_buffer(0);
        }
    }
}

void handle_packet(int id, buffer pkt) {
    string user, pass;
    int i;
    
    debug_message("RO: [OK] Parsed Packet 0x" + sprintf("%04X", id) + " (Len: " + sizeof(pkt) + ")\n");
    
    if (id == 0x0064) {
        user = "";
        pass = "";
        for(i = 2; i < 26; i++) { if(pkt[i]==0) break; user += sprintf("%c", pkt[i]); }
        for(i = 26; i < 50; i++) { if(pkt[i]==0) break; pass += sprintf("%c", pkt[i]); }
        debug_message("RO: Login -> " + user + " / " + pass + "\n");
    }
}
