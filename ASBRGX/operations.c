#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include "operations.h"

#define MAX_LINE 4096
#define MAX_MATCHES 10
#define REGEX_FLAGS (REG_EXTENDED)

int process_replace_regex(const char *filename, const char *pattern, const char *replacement) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка открытия файла");
        return -1;
    }

    char temp_filename[] = "temp_XXXXXX";
    int fd = mkstemp(temp_filename);
    if (fd == -1) {
        perror("Ошибка создания временного файла");
        fclose(file);
        return -1;
    }

    FILE *temp = fdopen(fd, "w");
    if (!temp) {
        perror("Ошибка открытия временного файла");
        fclose(file);
        close(fd);
        return -1;
    }

    // Компилируем регулярное выражение
    regex_t regex;
    int ret = regcomp(&regex, pattern, REGEX_FLAGS);
    if (ret != 0) {
        char error_msg[256];
        regerror(ret, &regex, error_msg, sizeof(error_msg));
        fprintf(stderr, "Ошибка компиляции регулярного выражения: %s\n", error_msg);
        fclose(file);
        fclose(temp);
        remove(temp_filename);
        return -1;
    }

    char line[MAX_LINE];
    regmatch_t matches[MAX_MATCHES];
    
    while (fgets(line, sizeof(line), file)) {
        char *current_pos = line;
        char result[MAX_LINE * 2] = {0};
        char *result_ptr = result;
        
        // Ищем все совпадения в строке
        while (regexec(&regex, current_pos, MAX_MATCHES, matches, 0) == 0) {
            // Копируем текст до совпадения
            size_t len_before = matches[0].rm_so;
            strncat(result, current_pos, len_before);
            
            // Добавляем замену
            strcat(result, replacement);
            
            // Перемещаем указатель
            current_pos += matches[0].rm_eo;
        }
        
        // Копируем остаток строки
        strcat(result, current_pos);
        
        fputs(result, temp);
    }

    regfree(&regex);
    fclose(file);
    fclose(temp);

    if (rename(temp_filename, filename) != 0) {
        perror("Ошибка замены файла");
        remove(temp_filename);
        return -1;
    }

    return 0;
}

int process_delete_regex(const char *filename, const char *pattern) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка открытия файла");
        return -1;
    }

    char temp_filename[] = "temp_XXXXXX";
    int fd = mkstemp(temp_filename);
    if (fd == -1) {
        perror("Ошибка создания временного файла");
        fclose(file);
        return -1;
    }

    FILE *temp = fdopen(fd, "w");
    if (!temp) {
        perror("Ошибка открытия временного файла");
        fclose(file);
        close(fd);
        return -1;
    }

    // Компилируем регулярное выражение
    regex_t regex;
    int ret = regcomp(&regex, pattern, REGEX_FLAGS);
    if (ret != 0) {
        char error_msg[256];
        regerror(ret, &regex, error_msg, sizeof(error_msg));
        fprintf(stderr, "Ошибка компиляции регулярного выражения: %s\n", error_msg);
        fclose(file);
        fclose(temp);
        remove(temp_filename);
        return -1;
    }

    char line[MAX_LINE];
    
    while (fgets(line, sizeof(line), file)) {
        // Если строка НЕ соответствует регулярному выражению, сохраняем её
        if (regexec(&regex, line, 0, NULL, 0) != 0) {
            fputs(line, temp);
        }
    }

    regfree(&regex);
    fclose(file);
    fclose(temp);

    if (rename(temp_filename, filename) != 0) {
        perror("Ошибка замены файла");
        remove(temp_filename);
        return -1;
    }

    return 0;
}

int process_file_with_regex(const char *filename, const char *command) {
    // Определяем тип команды
    if (strlen(command) < 3) {
        fprintf(stderr, "Неверный формат команды: %s\n", command);
        return -1;
    }
    
    // Команда замены: s/pattern/replacement/
    if (command[0] == 's' && command[1] == '/') {
        const char *first_slash = strchr(command + 2, '/');
        if (!first_slash) {
            fprintf(stderr, "Неверный формат команды замены: %s\n", command);
            return -1;
        }
        
        const char *second_slash = strchr(first_slash + 1, '/');
        if (!second_slash) {
            fprintf(stderr, "Неверный формат команды замены: %s\n", command);
            return -1;
        }
        
        // Извлекаем pattern и replacement
        size_t pattern_len = first_slash - (command + 2);
        size_t replacement_len = second_slash - (first_slash + 1);
        
        char *pattern = malloc(pattern_len + 1);
        char *replacement = malloc(replacement_len + 1);
        
        if (!pattern || !replacement) {
            perror("Ошибка выделения памяти");
            free(pattern);
            free(replacement);
            return -1;
        }
        
        strncpy(pattern, command + 2, pattern_len);
        pattern[pattern_len] = '\0';
        
        strncpy(replacement, first_slash + 1, replacement_len);
        replacement[replacement_len] = '\0';
        
        printf("Выполняем замену: pattern='%s', replacement='%s'\n", pattern, replacement);
        
        int result = process_replace_regex(filename, pattern, replacement);
        
        free(pattern);
        free(replacement);
        return result;
    }
    // Команда удаления: /pattern/d
    else if (command[0] == '/' && command[strlen(command) - 1] == 'd') {
        size_t pattern_len = strlen(command) - 2; // минус первый '/' и последний 'd'
        
        char *pattern = malloc(pattern_len + 1);
        if (!pattern) {
            perror("Ошибка выделения памяти");
            return -1;
        }
        
        strncpy(pattern, command + 1, pattern_len);
        pattern[pattern_len] = '\0';
        
        printf("Удаляем строки с pattern='%s'\n", pattern);
        
        int result = process_delete_regex(filename, pattern);
        
        free(pattern);
        return result;
    }
    else {
        fprintf(stderr, "Неизвестный формат команды: %s\n", command);
        fprintf(stderr, "Поддерживаемые форматы:\n");
        fprintf(stderr, "  s/pattern/replacement/ - замена по регулярному выражению\n");
        fprintf(stderr, "  /pattern/d - удаление строк по регулярному выражению\n");
        return -1;
    }
}