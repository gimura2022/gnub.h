/* include part */

#ifndef _gnub_h
#define _gnub_h

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define GNUB_MAX_CMD_PART_LENGHT 32
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

void _gnub__append_command(struct gnub__cmd_arr* arr, const size_t count, const char** parts);
void _gnub__execute_command(struct _gnub__cmd* cmd);
void _gnub__free_command(struct _gnub__cmd* cmd);

void gnub__execute_commands(struct gnub__cmd_arr* cmds);
void gnub__free_commands(struct gnub__cmd_arr* cmds);

#define gnub__append_command(arr, ...) ({ const char* __parts[] = {__VA_ARGS__}; \
		_gnub__append_command(arr, array_lenght(__parts), __parts); })

#endif

/* implementation part */

#ifndef _gnub_impl
#define _gnub_impl

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
		arr->end = arr->end->next;
	}

	arr->end->start = (struct _gnub__cmd_part*) malloc(sizeof(struct _gnub__cmd_part));
	arr->end->end = arr->end->start;

	for (size_t i = 0; i < count; i++) {
		memcpy(arr->end->end->str, parts[i], strlen(parts[i]));

		arr->end->end->next = (struct _gnub__cmd_part*) malloc(sizeof(struct _gnub__cmd_part));
		arr->end->end = arr->end->end->next;
	}
}

void _gnub__execute_command(struct _gnub__cmd* cmd)
{
	char out_command[GNUB_MAX_CMD_PART_LENGHT];
	memset(out_command, '\0', sizeof(out_command));

	struct _gnub__cmd_part* part = cmd->start;
	while (part != NULL) {
		strcat(out_command, part->str);	
		strcat(out_command, " ");

		part = part->next;
	}

	system(out_command);
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

void gnub__execute_commands(struct gnub__cmd_arr* cmds)
{
	struct _gnub__cmd* cmd = cmds->start;
	while (cmd != NULL) {
		_gnub__execute_command(cmd);	
		cmd = cmd->next;
	}
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

#endif
