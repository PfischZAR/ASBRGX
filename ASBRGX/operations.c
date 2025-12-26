#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include "operations.h"

#define MAX_LINE 4096
#define REGEX_FLAGS (REG_EXTENDED)

static int process_replace_regex(const char *filename, const char *pattern, const char *replacement) {
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
        unlink(temp_filename);
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
        unlink(temp_filename);
        return -1;
    }

    char line[MAX_LINE];
    
    while (fgets(line, sizeof(line), file)) {
        char output_line[MAX_LINE * 2] = {0};
        
        // Специальная обработка для ^ (начало строки)
        if (strcmp(pattern, "^") == 0) {
            snprintf(output_line, sizeof(output_line), "%s%s", replacement, line);
            fputs(output_line, temp);
            continue;
        }
        
        // Специальная обработка для $ (конец строки)
        if (strcmp(pattern, "$") == 0) {
            size_t len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
                snprintf(output_line, sizeof(output_line), "%s%s\n", line, replacement);
            } else {
                snprintf(output_line, sizeof(output_line), "%s%s", line, replacement);
            }
            fputs(output_line, temp);
            continue;
        }
        
        // Общий случай: заменяем первое вхождение
        regmatch_t match;
        if (regexec(&regex, line, 1, &match, 0) == 0) {
            // Копируем часть до совпадения
            strncat(output_line, line, match.rm_so);
            
            // Добавляем замену
            strcat(output_line, replacement);
            
            // Добавляем часть после совпадения
            strcat(output_line, line + match.rm_eo);
            
            fputs(output_line, temp);
        } else {
            // Если совпадений нет, копируем строку как есть
            fputs(line, temp);
        }
    }

    regfree(&regex);
    fclose(file);
    fclose(temp);

    if (rename(temp_filename, filename) != 0) {
        perror("Ошибка замены файла");
        unlink(temp_filename);
        return -1;
    }

    return 0;
}

static int process_delete_regex(const char *filename, const char *pattern) {
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
        unlink(temp_filename);
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
        unlink(temp_filename);
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
        unlink(temp_filename);
        return -1;
    }

    return 0;
}

int process_file_with_regex(const char *filename, const char *command) {
    // Проверяем длину команды
    if (strlen(command) < 3) {
        fprintf(stderr, "Неверный формат команды: %s\n", command);
        fprintf(stderr, "Формат: s/pattern/replacement/ или /pattern/d\n");
        return -1;
    }
    
    // Команда замены: s/pattern/replacement/
    if (command[0] == 's' && command[1] == '/') {
        // Находим разделители
        const char *pattern_start = command + 2;
        const char *pattern_end = strchr(pattern_start, '/');
        
        if (!pattern_end) {
            fprintf(stderr, "Неверный формат команды замены: %s\n", command);
            fprintf(stderr, "Формат: s/pattern/replacement/\n");
            return -1;
        }
        
        const char *replacement_start = pattern_end + 1;
        const char *replacement_end = strchr(replacement_start, '/');
        
        if (!replacement_end) {
            fprintf(stderr, "Неверный формат команды замены: %s\n", command);
            fprintf(stderr, "Формат: s/pattern/replacement/\n");
            return -1;
        }
        
        // Извлекаем pattern и replacement
        size_t pattern_len = pattern_end - pattern_start;
        size_t replacement_len = replacement_end - replacement_start;
        
        char *pattern = malloc(pattern_len + 1);
        char *replacement = malloc(replacement_len + 1);
        
        if (!pattern || !replacement) {
            perror("Ошибка выделения памяти");
            free(pattern);
            free(replacement);
            return -1;
        }
        
        strncpy(pattern, pattern_start, pattern_len);
        pattern[pattern_len] = '\0';
        
        strncpy(replacement, replacement_start, replacement_len);
        replacement[replacement_len] = '\0';
        
        int result = process_replace_regex(filename, pattern, replacement);
        
        free(pattern);
        free(replacement);
        return result;
    }
    // Команда удаления: /pattern/d
    else if (command[0] == '/' && command[strlen(command) - 1] == 'd') {
        size_t pattern_len = strlen(command) - 2; // минус первый '/' и последний 'd'
        
        // Проверяем, что есть хотя бы один символ в pattern
        if (pattern_len == 0) {
            fprintf(stderr, "Пустой шаблон в команде удаления: %s\n", command);
            return -1;
        }
        
        char *pattern = malloc(pattern_len + 1);
        if (!pattern) {
            perror("Ошибка выделения памяти");
            return -1;
        }
        
        strncpy(pattern, command + 1, pattern_len);
        pattern[pattern_len] = '\0';
        
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