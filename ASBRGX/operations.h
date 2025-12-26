#ifndef OPERATIONS_H
#define OPERATIONS_H

int process_file_with_regex(const char *filename, const char *command);
int process_replace_regex(const char *filename, const char *pattern, const char *replacement);
int process_delete_regex(const char *filename, const char *pattern);

#endif