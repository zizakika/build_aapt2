#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

int main(int argc, char *argv[]) {
    // Đường dẫn TUYỆT ĐỐI tới file AAPT2 thật của bạn (hãy sửa lại cho đúng cấu hình máy bạn)
    #ifdef _WIN32
    char *real_aapt2 = "C:\\Desktop\\aapt2_fix\\aapt2_real.exe";
    #else
    char *real_aapt2 = "/data/data/com.tom.rv2ide/files/home/.androidide/a/aapt2";
    #endif

    // Tạo mảng tham số mới để lọc cờ
    char **new_argv = malloc((argc + 1) * sizeof(char *));
    int new_argc = 0;

    // Tham số đầu tiên luôn là đường dẫn thực thi
    new_argv[new_argc++] = real_aapt2;

    // Duyệt và lọc bỏ cờ lỗi --collapse-resource-names
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--collapse-resource-names") == 0) {
            // Bỏ qua cờ này, không nạp vào mảng mới
            continue;
        }
        new_argv[new_argc++] = argv[i];
    }
    new_argv[new_argc] = NULL; // Kết thúc mảng bằng NULL theo quy định của exec

    // Khởi chạy file AAPT2 thật và chuyển tiếp toàn bộ luồng dữ liệu (I/O) nguyên bản
    #ifdef _WIN32
    intptr_t status = _execv(real_aapt2, (const char * const *)new_argv);
    return (int)status;
    #else
    execv(real_aapt2, new_argv);
    // Nếu execv chạy thành công, đoạn code phía dưới sẽ không bao giờ bị nấc tới
    perror("Execv failed");
    return 1;
    #endif
}
