// obj/ro/login.c - RO Login Server Handler (極簡測試版)

void create() {
    debug_message("RO: obj/ro/login object created!\n");
}

// 當 Binary 端口收到數據時，Driver 會自動呼叫此函數
void process_input(buffer data) {
    int data_len = sizeof(data);
    int i;
    int header;
    string username;
    string password;
    
    debug_message("RO: process_input called! Received " + data_len + " bytes.\n");
    
    // 檢查是否至少有 2 bytes 的 Header
    if (data_len >= 2) {
        // 讀取 Little-Endian 的 2 bytes Header
        header = data[0] | (data[1] << 8);
        
        // 判斷是否為 0x0064 (CA_LOGIN) 且長度足夠 (55 bytes)
        if (header == 0x0064 && data_len >= 55) {
            username = "";
            password = "";
            
            // 提取 Username (Bytes 2..25)
            for (i = 2; i < 26; i++) {
                if (data[i] == 0) break;
                username += sprintf("%c", data[i]);
            }
            
            // 提取 Password (Bytes 26..49)
            for (i = 26; i < 50; i++) {
                if (data[i] == 0) break;
                password += sprintf("%c", data[i]);
            }
            
            debug_message("RO: ✅ SUCCESS! Parsed CA_LOGIN -> User: " + username + " / Pass: " + password + "\n");
        } else {
            debug_message("RO: Unknown packet: 0x" + sprintf("%04X", header) + " (Len: " + data_len + ")\n");
        }
    }
}
