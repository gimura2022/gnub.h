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

static char cflags_out[512];
static char ldflags_out[512];
static char cppflags_out[512];

static char*** argv_ptr;

static void die(const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	vfprintf(stderr, msg, args);

	va_end(args);

	exit(-1);
}

static void compile(void)
{
	strcat(cflags_out, cflags);

	strcat(ldflags_out, ldflags_libgoodbye);
	strcat(ldflags_out, ldflags_libhello);

	strcat(cppflags_out, cppflags);
	strcat(cppflags_out, cppflags_libgoodbye);
	strcat(cppflags_out, cppflags_libhello);

	gnub__compile_subproject("hello", *argv_ptr);
	gnub__compile_subproject("goodbye", *argv_ptr);

	struct gnub__cmd_arr compile_commands = {0};

	char objects[GNUB_FIND_C_FILES_MAX_FILES][2][GNUB_MAX_FILE_NAME] = {0};
	size_t count;
	gnub__find_c_files("src/", objects, &count);

	for (size_t i = 0; i < count; i++) {
		gnub__append_command(&compile_commands, cc, cflags_out, cppflags_out, "-c", "-o",
				objects[i][1], objects[i][0], ldflags_out);
	}

	gnub__create_executable(&compile_commands, cc, "example.x80-86", ldflags_out, objects, count);

	gnub__execute_commands(&compile_commands);
	gnub__free_commands(&compile_commands);
}

static void debug(void)
{
	strcpy(cflags_out, cflags_deb);
	compile();
}

static void release(void)
{
	strcpy(cflags_out, cflags_rel);
	compile();
}

int main(int argc, char* argv[])
{
	argv_ptr = &argv;

	cc = gnub__get_env_variable("CC", cc);
	ar = gnub__get_env_variable("AR", ar);

	gnub__recompile_self(argv);

	gnub__add_target("release", release);
	gnub__add_target("debug", debug);

	const char* default_targets[] = { "release" };
	gnub__run_targets(argc, argv, default_targets, array_lenght(default_targets));

	return 0;
}
