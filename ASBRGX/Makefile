# Переменные
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -D_GNU_SOURCE
TARGET = sed_editor
SOURCES = main.c operations.c
OBJECTS = $(SOURCES:.c=.o)

# Цель по умолчанию
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Компиляция .c файлов в .o файлы
%.o: %.c operations.h
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(TARGET) $(OBJECTS) test.txt temp_* 2>/dev/null || true

# Тестирование регулярных выражений
test: $(TARGET)
	@echo "=== Тестирование sed_editor с регулярными выражениями ==="
	
	@echo "Создаем тестовый файл..."
	@printf "line1\nline2\n123 apple\n456 banana\n789 cherry\nempty line\n\nlast line\n" > test.txt
	
	@echo ""
	@echo "1. Тест: Добавление префикса в начало каждой строки"
	@echo "Файл до:"
	@cat test.txt
	@echo ""
	@echo "Выполняем: 's/^/>>> /'"
	@./$(TARGET) test.txt 's/^/>>> /'
	@echo "Файл после:"
	@cat test.txt
	
	@echo ""
	@echo ""
	@echo "2. Тест: Добавление суффикса в конец каждой строки"
	@echo "Выполняем: 's/$$/ [end]/'"
	@./$(TARGET) test.txt 's/$/ [end]/'
	@echo "Файл после:"
	@cat test.txt
	
	@echo ""
	@echo ""
	@echo "3. Тест: Замена всех чисел на 'NUMBER'"
	@echo "Выполняем: 's/[0-9]+/NUMBER/'"
	@./$(TARGET) test.txt 's/[0-9]+/NUMBER/'
	@echo "Файл после:"
	@cat test.txt
	
	@echo ""
	@echo ""
	@echo "4. Тест: Удаление пустых строк"
	@echo "Выполняем: '/^$$/d'"
	@./$(TARGET) test.txt '/^$/d'
	@echo "Файл после:"
	@cat test.txt
	
	@echo ""
	@echo ""
	@echo "5. Тест: Удаление строк, содержащих 'apple' или 'banana'"
	@echo "Выполняем: '/apple\\|banana/d'"
	@./$(TARGET) test.txt '/apple\|banana/d'
	@echo "Файл после:"
	@cat test.txt
	
	@echo ""
	@echo "=== Тестирование завершено ==="
	@rm -f test.txt

# Тестирование базовых примеров из задания
test-basic: $(TARGET)
	@echo "=== Базовые тесты из задания ==="
	
	@echo "Создаем тестовый файл..."
	@printf "first line\nsecond line\nthird line\n" > input.txt
	
	@echo ""
	@echo "1. Добавление суффикса: ./sed_editor input.txt 's/$$/suffix/'"
	@./$(TARGET) input.txt 's/$/suffix/'
	@cat input.txt
	
	@echo ""
	@echo "2. Замена текста: ./sed_editor input.txt 's/line/LINE/'"
	@./$(TARGET) input.txt 's/line/LINE/'
	@cat input.txt
	
	@echo ""
	@echo "3. Удаление строк: ./sed_editor input.txt '/second/d'"
	@./$(TARGET) input.txt '/second/d'
	@cat input.txt
	
	@echo ""
	@echo "4. Добавление префикса: ./sed_editor input.txt 's/^/prefix/'"
	@./$(TARGET) input.txt 's/^/prefix/'
	@cat input.txt
	
	@rm -f input.txt
	@echo ""
	@echo "=== Базовые тесты завершены ==="

.PHONY: all clean test test-basic