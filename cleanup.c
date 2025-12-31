#include "hsh.h"

extern int g_env_is_ours;

void cleanup_env(void)
{
	int i;

	if (!g_env_is_ours || !environ)
		return;

	for (i = 0; environ[i]; i++)
		free(environ[i]);

	free(environ);
	environ = NULL;
	g_env_is_ours = 0;
}
