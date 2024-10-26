#ifndef SP_PROJECT_CONFIG_H
#define SP_PROJECT_CONFIG_H
#include <SP/config.h>
#ifndef SP_WORKSPACE
#define SP_WORKSPACE "C:/sandbox/PlaygroundPacman"
#endif
#define SP_CAT(a, b) a b
#define SP_PATH(location) SP_CAT(SP_WORKSPACE, location)

//	write your setup here!!
typedef long long pm_id_t;
#endif
