#include <stdio.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>

#define gnub_impl
#include "../gnub.h"

static const char* cflags     = "-std=c99 -Wall -Wpedantic";
static const char* cflags_rel = "-O3";
static const char* cflags_deb = "-O0 -g";

static const char* cppflags            = "-I include";
static const char* cppflags_libhello   = "-I ./hello/include";
static const char* cppflags_libgoodbye = "-I ./goodbye/include";

static const char* ldflags_libhello   = "-L./hello -l:libhello.a";
static const char* ldflags_libgoodbye = "-L./goodbye -l:libgoodbye.a";

static char* cc = "cc";
static char* ar = "ar";

static void die(const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	vfprintf(stderr, msg, args);

	va_end(args);

	exit(-1);
}

static void recompile_self(char* argv[])
{
	struct gnub__cmd_arr cmds = {0};

	char output_file[32] = {0};
	strcpy(output_file, argv[0]);
	strcat(output_file, ".new");

	gnub__append_command(&cmds, cc, cflags_deb, "-o", output_file, "gnub.c");

	if (!gnub__recompile_self(&cmds, output_file, argv)) 
		die("Error: can't recompile build script\n");

	gnub__free_commands(&cmds);
}

int main(int argc, char* argv[])
{
	cc = getenv("CC") == NULL ? cc : getenv("CC");
	ar = getenv("AR") == NULL ? ar : getenv("AR");

	recompile_self(argv);

	gnub__compile_subproject("hello", argv);
	gnub__compile_subproject("goodbye", argv);

	const char* cflags_add = cflags_rel;
	if (argc == 2) {
		if (strcpy(argv[1], "release") == 0) cflags_add = cflags_rel;
		else if (strcpy(argv[1], "debug") == 0) cflags_add = cflags_deb;
		else die("Error: unrecognized option %s\n", argv[1]);
	}

	struct gnub__cmd_arr compile_commands = {0};

	char objects[GNUB_FIND_C_FILES_MAX_FILES][2][GNUB_MAX_FILE_NAME] = {0};
	size_t count;
	gnub__find_c_files("src/", objects, &count);

	for (size_t i = 0; i < count; i++) {
		gnub__append_command(&compile_commands, cc, cflags, cflags_add, cppflags_libhello,
				cppflags_libgoodbye, cppflags, "-c", "-o", objects[i][1], objects[i][0],
				ldflags_libhello, ldflags_libgoodbye);
	}

	char ldflags[1024] = {0};
	strcat(ldflags, ldflags_libgoodbye);
	strcat(ldflags, " ");
	strcat(ldflags, ldflags_libhello);

	gnub__create_executable(&compile_commands, cc, "example.80-86", ldflags, objects, count);

	gnub__execute_commands(&compile_commands);
	gnub__free_commands(&compile_commands);

	return 0;
}
