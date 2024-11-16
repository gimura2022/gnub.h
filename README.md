# gnub.h
Small build system like nub.h

## Quick start
1. Clone gnub.h to you project
2. Write build script like this:
```c
#include "../gnub.h"

int main(int argc, char* argv[])
{
    struct gnub__cmd_arr cmds = {0};
	gnub__append_command(&cmds, "cc", "gnub.c");

	gnub__execute_commands(&cmds);
	gnub__free_commands(&cmds);

	return 0;
}
```
3. Compile (`cc gnub.c -o gnub`) and run (`./gnub`)!
