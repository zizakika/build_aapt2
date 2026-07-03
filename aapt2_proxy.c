#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Đường dẫn TUYỆT ĐỐI tới file AAPT2 thật của bạn
    char *real_aapt2 = "/data/data/com.tom.rv2ide/files/home/.androidide/a/aapt2";

    // Khởi tạo một chuỗi buffer lớn để chứa toàn bộ câu lệnh
    // Thường tổng độ dài câu lệnh AAPT2 không quá 65535 ký tự
    char *cmd = malloc(128000 * sizeof(char));
    if (cmd == NULL) return 1;

    // Bắt đầu chuỗi bằng đường dẫn file thật, bọc nháy kép để tránh khoảng trắng
    sprintf(cmd, "\"%s\"", real_aapt2);

    // Duyệt và lọc bỏ cờ lỗi --collapse-resource-names
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--collapse-resource-names") == 0) {
            continue; // Bỏ qua cờ gây lỗi nuốt file .arsc
        }
        
        // Nối tiếp các tham số khác vào chuỗi lệnh, bọc nháy kép từng tham số cho an toàn
        strcat(cmd, " \"");
        strcat(cmd, argv[i]);
        strcat(cmd, "\"");
    }

    // Thực thi chuỗi lệnh thông qua Shell tiêu chuẩn của hệ điều hành
    // Hàm này an toàn tuyệt đối trên mọi kiến trúc ARM64/x86 vì không dùng trực tiếp execv syscall
    int result = system(cmd);
    
    free(cmd);
    
    // Trả về đúng mã kết quả của AAPT2 thật để Gradle không bị báo lỗi xanh/đỏ sai
    if (result == -1) return 1;
    return WEXITSTATUS(result);
}
