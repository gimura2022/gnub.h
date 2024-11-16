#include "../gnub.h"

int main(int argc, char* argv[])
{
	struct gnub__cmd_arr self_compile = {0};
	gnub__compile_c(&self_compile, "cc", "-std=c99 -Wall -g", "", "", "examples/gnub_recompile_self.c",
			"gnub.new");
	gnub__recompile_self(&self_compile, "gnub.new", argv);
	gnub__free_commands(&self_compile);

	printf("Recompile sucess\n");
	
	struct gnub__cmd_arr arr = {0};
	gnub__append_command(&arr, "echo", "Hello, world!");
	gnub__execute_commands(&arr);
	gnub__free_commands(&arr);
	
	return 0;
}
