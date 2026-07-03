#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // 1. Tự động lấy đường dẫn thư mục chứa file proxy này
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    
    if (len == -1) {
        perror("Không thể đọc đường dẫn thực thi");
        return 1;
    }
    exe_path[len] = '\0';

    // Cắt bỏ tên file proxy ở cuối để lấy đường dẫn thư mục cha
    char *last_slash = strrchr(exe_path, '/');
    if (last_slash != NULL) {
        *(last_slash + 1) = '\0'; // Giữ lại dấu '/' ở cuối
    } else {
        strcpy(exe_path, "./");
    }

    // 2. Ghép tên file AAPT2 thật vào cùng thư mục (đặt tên là aapt2_real)
    char real_aapt2[2048];
    snprintf(real_aapt2, sizeof(real_aapt2), "%saapt2_", exe_path);

    // 3. Khởi tạo bộ đệm chứa chuỗi lệnh lệnh (Buffer)
    // Tăng kích thước lên 256KB để thoải mái chứa danh sách tham số cực dài của Gradle
    char *cmd = malloc(262144 * sizeof(char));
    if (cmd == NULL) {
        perror("Không đủ bộ nhớ gán buffer");
        return 1;
    }

    // Bắt đầu chuỗi bằng đường dẫn file thật, bọc nháy kép để chống khoảng trắng
    sprintf(cmd, "\"%s\"", real_aapt2);

    // 4. Duyệt và lọc bỏ cờ lỗi --collapse-resource-names
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--collapse-resource-names") == 0) {
            continue; // Bỏ qua cờ gây lỗi nuốt file .arsc
        }
        
        // Nối tiếp các tham số khác vào chuỗi lệnh, bọc nháy kép từng tham số
        strcat(cmd, " \"");
        strcat(cmd, argv[i]);
        strcat(cmd, "\"");
    }

    // 5. Thực thi lệnh qua Shell hệ thống
    int result = system(cmd);
    free(cmd);
    
    // Trả về đúng mã lỗi (Exit Code) của AAPT2 thật để Gradle kiểm soát tiến trình build
    if (result == -1) return 1;
    return WEXITSTATUS(result);
}
