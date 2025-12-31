#include "hsh.h"

extern int g_env_cloned;
extern char **g_env_alloc;

/**
 * cleanup_env - free cloned environment
 */
void cleanup_env(void)
{
	size_t i = 0;

	if (!g_env_cloned || !g_env_alloc)
		return;

	while (g_env_alloc[i])
	{
		free(g_env_alloc[i]);
		i++;
	}
	free(g_env_alloc);

	g_env_alloc = NULL;
	g_env_cloned = 0;
}
