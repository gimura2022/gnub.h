#include "../gnub.h"

int main(int argc, char* argv[])
{
	struct gnub__cmd_arr arr = {0};
	gnub__compile_c_object(&arr, "cc", "-std=c99 -Wall", "", "examples/gnub.c", "out.o");
	gnub__link_objects(&arr, "cc", "out.o", "", "gnub");

	gnub__execute_commands(&arr);
	gnub__free_commands(&arr);

	return 0;
}
