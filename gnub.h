/* include part */

#ifndef _gnub_h
#define _gnub_h

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define GNUB_MAX_CMD_PART_LENGHT 1024
#define GNUB_MAX_CMD_LEN 1024

#define array_lenght(x) sizeof(x) / sizeof(x[0])

struct _gnub__cmd_part {
	struct _gnub__cmd_part* next;
	char str[GNUB_MAX_CMD_PART_LENGHT];
};

struct _gnub__cmd {
	struct _gnub__cmd* next;

	struct _gnub__cmd_part* start;
	struct _gnub__cmd_part* end;
};

struct gnub__cmd_arr {
	struct _gnub__cmd* start;
	struct _gnub__cmd* end;
};

/* private functions */

void _gnub__append_command(struct gnub__cmd_arr* arr, const size_t count, const char** parts);
void _gnub__append_parts_to_last(struct gnub__cmd_arr* arr, const size_t count, const char** parts);
void _gnub__append_parts_by_index(struct gnub__cmd_arr* arr, const size_t count, const char** parts);
void _gnub__append_to_command(struct _gnub__cmd* cmd, const size_t count, const char** parts);

int _gnub__execute_command(struct _gnub__cmd* cmd);
void _gnub__free_command(struct _gnub__cmd* cmd);

bool _gnub__compare_files(const char* file0, const char* file1);

/* public functions */

int gnub__execute_commands(struct gnub__cmd_arr* cmds);
void gnub__free_commands(struct gnub__cmd_arr* cmds);

void gnub__compile_c_object(struct gnub__cmd_arr* arr, const char* cc, const char* cflags,
		const char* cppflags, const char* source, const char* output);
void gnub__compile_c(struct gnub__cmd_arr* arr, const char* cc, const char* cflags, const char* cppflags,
		const char* ldflags, const char* source, const char* output);
void gnub__link_objects(struct gnub__cmd_arr* arr, const char* ld, const char* objects, const char* ldflags,
		const char* output);

bool gnub__recompile_self(struct gnub__cmd_arr* arr, const char* output_file, char* argv[]);

#define _gnub__parts_command(x, arr, ...) ({ const char* __parts[] = {__VA_ARGS__}; \
		x(arr, array_lenght(__parts), __parts); })

#define gnub__append_command(arr, ...) _gnub__parts_command(_gnub__append_command, arr, __VA_ARGS__)
#define gnub__append_parts_to_last(arr, ...) _gnub__parts_command(_gnub__append_parts_to_last, \
		arr, __VA_ARGS__)
#define gnub__append_parts_by_index(arr, ...) _gnub__parts_command(_gnub__append_parts_by_index, \
		arr, __VA_ARGS__)

#endif

/* implementation part */

#ifndef _gnub_impl
#define _gnub_impl

/* private functions */

void _gnub__append_to_command(struct _gnub__cmd* cmd, const size_t count, const char** parts)
{
	for (size_t i = 0; i < count; i++) {
		memcpy(cmd->end->str, parts[i], strlen(parts[i]));

		cmd->end->next = (struct _gnub__cmd_part*) malloc(sizeof(struct _gnub__cmd_part));
		cmd->end = cmd->end->next;
	}
}

void _gnub__append_command(struct gnub__cmd_arr* arr, const size_t count, const char** parts)
{
	bool is_first_cmd = false;

	if (arr->start == NULL) {
		is_first_cmd = true;

		arr->start = (struct _gnub__cmd*) malloc(sizeof(struct _gnub__cmd));
		arr->end   = arr->start;
	}

	if (!is_first_cmd) {
		arr->end->next = (struct _gnub__cmd*) malloc(sizeof(struct _gnub__cmd));
		arr->end       = arr->end->next;
	}

	arr->end->start = (struct _gnub__cmd_part*) malloc(sizeof(struct _gnub__cmd_part));
	arr->end->end   = arr->end->start;

	_gnub__append_to_command(arr->end, count, parts);
}

int _gnub__execute_command(struct _gnub__cmd* cmd)
{
	char out_command[GNUB_MAX_CMD_PART_LENGHT];
	memset(out_command, '\0', sizeof(out_command));

	struct _gnub__cmd_part* part = cmd->start;
	while (part != NULL) {
		strcat(out_command, part->str);	
		strcat(out_command, " ");

		part = part->next;
	}

	return system(out_command);
}

void _gnub__free_command(struct _gnub__cmd* cmd)
{
	struct _gnub__cmd_part* part = cmd->start;
	while (part != NULL) {
		struct _gnub__cmd_part* next = part->next;
		free(part);
		part = next;
	}

	free(cmd);
}

void _gnub__append_parts_to_last(struct gnub__cmd_arr* arr, const size_t count, const char** parts)
{
	_gnub__append_to_command(arr->end, count, parts); // TODO: add check to empty command array
}

void _gnub__append_parts_by_index(struct gnub__cmd_arr* arr, const size_t count, const char** parts)
{
	size_t i               = 0;
	struct _gnub__cmd* cmd = arr->start;
	while (i < count && cmd != NULL) {
		i++;
		cmd = cmd->next;
	}

	_gnub__append_to_command(cmd, count, parts);
}

bool _gnub__compare_files(const char* file0, const char* file1)
{
	FILE* file0_ds = fopen(file0, "rb");
	FILE* file1_ds = fopen(file1, "rb");

	int c0 = fgetc(file0_ds);
	int c1 = fgetc(file1_ds);

	bool is_eq = false;

	while (c0 != EOF && c1 != EOF) {
		if (c0 != c1) goto done;	

		c0 = fgetc(file0_ds);
		c1 = fgetc(file1_ds);
	}

	if (c0 == EOF && c1 == EOF) {
		is_eq = true;
		goto done;
	}

done:
	fclose(file0_ds);
	fclose(file1_ds);

	return is_eq;
}

/* public functions */

int gnub__execute_commands(struct gnub__cmd_arr* cmds)
{
	struct _gnub__cmd* cmd = cmds->start;
	while (cmd != NULL) {
		int code = _gnub__execute_command(cmd);
		if (code != 0) {
			return code;
		}

		cmd = cmd->next;
	}

	return 0;
}

void gnub__free_commands(struct gnub__cmd_arr* cmds)
{
	struct _gnub__cmd* cmd = cmds->start;
	while (cmd != NULL) {
		struct _gnub__cmd* next = cmd->next;
		_gnub__free_command(cmd);
		cmd = next;
	}
}

void gnub__compile_c_object(struct gnub__cmd_arr* arr, const char* cc, const char* cflags,
		const char* cppflags, const char* source, const char* output)
{
	gnub__append_command(arr, cc, cflags, cppflags, "-c", "-o", output, source);
}

void gnub__compile_c(struct gnub__cmd_arr* arr, const char* cc, const char* cflags, const char* cppflags,
		const char* ldflags, const char* source, const char* output)
{
	gnub__append_command(arr, cc, cflags, cppflags, ldflags, "-o", output, source);
}

void gnub__link_objects(struct gnub__cmd_arr* arr, const char* ld, const char* objects, const char* ldflags,
		const char* output)
{
	gnub__append_command(arr, ld, ldflags, "-o", output, objects);
}

bool gnub__recompile_self(struct gnub__cmd_arr* arr, const char* output_file, char* argv[])
{
	if (strcmp(output_file, argv[0]) == 0) return false;
	if (gnub__execute_commands(arr) != 0) return false;
	if (_gnub__compare_files(output_file, argv[0])) return true;

	gnub__free_commands(arr);

	remove(argv[0]);
	rename(output_file, argv[0]);

	execv(argv[0], argv);
	exit(0);
}

#endif
