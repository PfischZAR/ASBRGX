#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operations.h"

void print_usage() {
    printf("Использование:\n");
    printf("  ./sed_editor <файл> '<команда>'\n");
    printf("\nКоманды с регулярными выражениями:\n");
    printf("  s/pattern/replacement/ - замена по регулярному выражению\n");
    printf("  /pattern/d - удаление строк по регулярному выражению\n");
    printf("\nПримеры:\n");
    printf("  ./sed_editor file.txt 's/^/prefix_/'\n");
    printf("  ./sed_editor file.txt 's/$/_suffix/'\n");
    printf("  ./sed_editor file.txt 's/[0-9]+/NUM/'\n");
    printf("  ./sed_editor file.txt '/error/d'\n");
    printf("  ./sed_editor file.txt '/^\\s*$/d' (удалить пустые строки)\n");
    printf("\nСпециальные символы в регулярных выражениях:\n");
    printf("  ^ - начало строки\n");
    printf("  $ - конец строки\n");
    printf("  . - любой символ\n");
    printf("  * - 0 или более повторений\n");
    printf("  + - 1 или более повторений\n");
    printf("  [] - класс символов\n");
    printf("  \\s - пробельный символ\n");
    printf("  \\d - цифра\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage();
        return 1;
    }

    const char *filename = argv[1];
    const char *command = argv[2];

    int result = process_file_with_regex(filename, command);
    
    if (result == 0) {
        printf("Команда '%s' успешно выполнена для файла '%s'\n", command, filename);
    } else {
        fprintf(stderr, "Ошибка выполнения команды '%s' для файла '%s'\n", command, filename);
        return 1;
    }

    return 0;
}