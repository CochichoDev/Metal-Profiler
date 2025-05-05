#pragma once

#include "types.h"
#include "common.h"

err CALL_MAKEFILES(CONFIG *config);
err DEPLOY_FILES(const char *dest);
void destroyBuildConf();
/*
 * Arg: Selected config
 *      Array to be changed (Needs to have the size of all cores
 * Return: Length of array (Equal to number of available cores) or 0 in case of error
 */
size_t activeCores(CONFIG *config, s8 *core_state);
/*
 * Arg: Core ID
 *      Char Array to be changed with the path (should be long enough)
 * Return: Length of the path string
 */
size_t getBinPath(s8 core_id, char *path);
