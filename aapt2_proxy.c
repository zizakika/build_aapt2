#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;

int main(int argc, char *argv[]) {
    // 1. Tự động lấy đường dẫn thư mục chứa file proxy này
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        return 1;
    }
    exe_path[len] = '\0';

    char *last_slash = strrchr(exe_path, '/');
    if (last_slash != NULL) {
        *(last_slash + 1) = '\0';
    } else {
        strcpy(exe_path, "./");
    }

    // 2. Ghép tên file AAPT2 thật nằm cùng thư mục
    char real_aapt2[2048];
    snprintf(real_aapt2, sizeof(real_aapt2), "%saapt2_", exe_path);

    // 3. Tạo mảng tham số mới cho tiến trình con
    char **new_argv = malloc((argc + 1) * sizeof(char *));
    if (new_argv == NULL) return 1;

    int new_argc = 0;
    new_argv[new_argc++] = real_aapt2; // Tham số đầu tiên là đường dẫn file chạy thật

    // Lọc bỏ cờ lỗi --collapse-resource-names
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--collapse-resource-names") == 0) {
            continue;
        }
        new_argv[new_argc++] = argv[i];
    }
    new_argv[new_argc] = NULL;

    // 4. Khởi chạy tiến trình bằng posix_spawn thay vì execv hay system()
    pid_t pid;
    int status = posix_spawn(&pid, real_aapt2, NULL, NULL, new_argv, environ);
    
    free(new_argv);

    if (status != 0) {
        // Khởi chạy thất bại
        return 1;
    }

    // 5. Chờ tiến trình con hoàn thành và lấy đúng mã thoát (Exit Code)
    if (waitpid(pid, &status, 0) != -1) {
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }

    return 1;
}
