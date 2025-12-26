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
	rm -f $(TARGET) $(OBJECTS) *.txt temp_* 2>/dev/null || true

# Тестирование
test: $(TARGET)
	@echo "=== Тестирование sed_editor ==="
	
	@echo "1. Создаем тестовый файл..."
	@printf "first line\nsecond line\n123 number\n456 another\nlast line\n" > test.txt
	
	@echo ""
	@echo "2. Тест: Добавление префикса"
	@echo "Команда: ./sed_editor test.txt 's/^/>>> /'"
	@./$(TARGET) test.txt 's/^/>>> /'
	@cat test.txt
	
	@echo ""
	@echo "3. Тест: Добавление суффикса"
	@echo "Команда: ./sed_editor test.txt 's/\$$/ [END]/'"
	@./$(TARGET) test.txt 's/$$/ [END]/'
	@cat test.txt
	
	@echo ""
	@echo "4. Тест: Замена чисел"
	@echo "Команда: ./sed_editor test.txt 's/[0-9]+/NUMBER/'"
	@./$(TARGET) test.txt 's/[0-9]+/NUMBER/'
	@cat test.txt
	
	@echo ""
	@echo "5. Тест: Удаление строк с 'another'"
	@echo "Команда: ./sed_editor test.txt '/another/d'"
	@./$(TARGET) test.txt '/another/d'
	@cat test.txt
	
	@rm -f test.txt
	@echo ""
	@echo "=== Тестирование завершено ==="

# Быстрый тест
quick-test: $(TARGET)
	@echo "=== Быстрый тест ==="
	@echo "test line" > qtest.txt
	@echo "another line" >> qtest.txt
	@echo "Исходный файл:"
	@cat qtest.txt
	@echo ""
	@echo "После добавления префикса:"
	@./$(TARGET) qtest.txt 's/^/PREFIX /' && cat qtest.txt || echo "Ошибка"
	@rm -f qtest.txt

.PHONY: all clean test quick-test