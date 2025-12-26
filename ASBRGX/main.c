#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operations.h"

void print_usage() {
    printf("Упрощенный потоковый редактор sed с поддержкой регулярных выражений\n");
    printf("Использование:\n");
    printf("  ./sed_editor <файл> '<команда>'\n");
    printf("\nКоманды:\n");
    printf("  s/pattern/replacement/  - замена первого вхождения pattern на replacement\n");
    printf("  /pattern/d              - удаление строк, содержащих pattern\n");
    printf("\nСпециальные символы в регулярных выражениях:\n");
    printf("  ^     - начало строки\n");
    printf("  $     - конец строки\n");
    printf("  .     - любой символ\n");
    printf("  *     - 0 или более повторений предыдущего символа\n");
    printf("  +     - 1 или более повторений предыдущего символа\n");
    printf("  []    - класс символов (например: [a-z], [0-9])\n");
    printf("  |     - ИЛИ (например: a|b)\n");
    printf("\nПримеры:\n");
    printf("  ./sed_editor file.txt 's/^/>>> /'          - добавить '>>> ' в начало каждой строки\n");
    printf("  ./sed_editor file.txt 's/$/ END/'          - добавить ' END' в конец каждой строки\n");
    printf("  ./sed_editor file.txt 's/[0-9]+/NUM/'      - заменить числа на 'NUM'\n");
    printf("  ./sed_editor file.txt '/^$/d'             - удалить пустые строки\n");
    printf("  ./sed_editor file.txt '/error/d'          - удалить строки с 'error'\n");
    printf("  ./sed_editor config.txt '/^#/d'           - удалить комментарии (строки, начинающиеся с #)\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage();
        return 1;
    }

    const char *filename = argv[1];
    const char *command = argv[2];

    // Проверяем существование файла
    FILE *test_file = fopen(filename, "r");
    if (!test_file) {
        fprintf(stderr, "Ошибка: файл '%s' не существует или недоступен для чтения\n", filename);
        return 1;
    }
    fclose(test_file);

    int result = process_file_with_regex(filename, command);
    
    if (result == 0) {
        printf("Команда успешно выполнена: %s\n", command);
    } else {
        fprintf(stderr, "Ошибка выполнения команды: %s\n", command);
        return 1;
    }

    return 0;
}