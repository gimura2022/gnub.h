#include "../gnub.h"

int main(int argc, char* argv[])
{
	struct gnub__cmd_arr cmds = {0};
	gnub__append_command(&cmds, "cc", "gnub.c");

	gnub__execute_commands(&cmds);
	gnub__free_commands(&cmds);

	return 0;
}
