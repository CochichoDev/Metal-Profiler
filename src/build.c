#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cJSON.h"
#include "global.h"
#include "common.h"
#include "utils.h"

#define DEBUG

struct build_inst {
    char path[128];
    u8 *params;
    size_t params_size;
};

struct build_cmd {
    struct build_inst *inst;
    size_t inst_size;
    struct build_cmd *next;
};

struct deploy_cmd {
    char src[128];
    char dest[128];
    struct deploy_cmd *next;
};

static struct {
    char cc[64];
    char ld[64];
    char ar[64];
    char objcopy[64];
    char deploy_base[128];
    struct build_cmd *bhead;
    struct deploy_cmd *dhead;
} build_conf;

static void copyFile(const char *src, const char *dest) {
    int from_fd = open(src, O_RDONLY);
    if (from_fd == -1) {
        perror("Error opening file descriptor for src file");
        return;
    }

    int to_fd = open(dest, O_WRONLY | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);
    if (to_fd == -1) {
        perror("Error opening file descriptor for dest file");
        goto END_COPY1;
    }
    char buf[4096];
    size_t rbytes = 0, wbytes = 0;

    while ((rbytes = read(from_fd, buf, sizeof(buf)))) {
        wbytes = write(to_fd, buf, rbytes);

        if (rbytes != wbytes) {
            fprintf(stderr, "Error: Something unexpected happened while copying file %s to %s\n", src, dest);
            perror("Error");
            goto END_COPY;
        }
    }
END_COPY:
    close(to_fd);
END_COPY1:
    close(from_fd);
}

static void catPropDefine(char *str, PROP *prop) {
    char buf[128];
    strcpy(buf, "-D");
    char arg[64];
    switch (prop->PTYPE) {
        case pINT:
        case pCHAR:
            sprintf(arg, "%d", prop->iINIT);
            break;
        case pDOUBLE:
            sprintf(arg, "%lf", prop->fINIT);
            break;
        case pSTR:
            sprintf(arg, "%s", prop->sINIT);
            break;
        case pBOOL:
            if (!prop->iINIT) return;
            strcat(buf, prop->NAME);
            goto JOIN;
    }

    strcat(buf, prop->NAME);
    strcat(buf, "=");
    strcat(buf, arg);

JOIN:
    strcat(str, buf);
    strcat(str, " ");

}

static void destroyBuildConf() {
    struct build_cmd *build_ptr = build_conf.bhead;
    while (build_ptr != NULL) {
        build_conf.bhead = build_ptr->next;
        for (size_t i = 0; i < build_ptr->inst_size; i++) {
            free(build_ptr->inst[i].params);
        }
        free(build_ptr->inst);
        free(build_ptr);
        build_ptr = build_conf.bhead;
    }
    struct deploy_cmd *deploy_ptr = build_conf.dhead;
    while (deploy_ptr != NULL) {
        build_conf.dhead = deploy_ptr->next;
        free(deploy_ptr);
        deploy_ptr = build_conf.dhead;
    }

}

static err parseBuildConfig(const char *path) {
    FILE *build_json = fopen(path, "r");
    if (build_json == NULL) {
        fprintf(stderr, "Error: Could not open build config for selected project\n");
        perror("Code");
        return -1;
    }

    char *json_buffer;
    fseek(build_json, 0, SEEK_END);
    size_t json_size = ftell(build_json);
    json_buffer = malloc(json_size + 1);
    bzero(json_buffer, json_size);
    fseek(build_json, 0, SEEK_SET);

    fread(json_buffer, sizeof(char), json_size, build_json);
    fclose(build_json);

    json_buffer[json_size] = '\0';
    cJSON *parse_data = cJSON_Parse(json_buffer);
    if (parse_data == NULL) {
        const char *cjson_err = cJSON_GetErrorPtr();
        if (cjson_err != NULL) {
            fprintf(stderr, "Error parsing build json config: %s\n", cjson_err);
        }
        return -1;
    }
    free(json_buffer);

    const cJSON *cc = cJSON_GetObjectItem(parse_data, "CC");
    if (cJSON_IsString(cc)) {
        strcpy(build_conf.cc, cc->valuestring);
        #ifdef DEBUG
        printf("CC: %s\n", build_conf.cc);
        #endif
    }
    const cJSON *ld = cJSON_GetObjectItem(parse_data, "LD");
    if (cJSON_IsString(ld)) {
        strcpy(build_conf.ld, ld->valuestring);
        #ifdef DEBUG
        printf("LD: %s\n", build_conf.ld);
        #endif
    }
    const cJSON *ar = cJSON_GetObjectItem(parse_data, "AR");
    if (cJSON_IsString(ar)) {
        strcpy(build_conf.ar, ar->valuestring);
        #ifdef DEBUG
        printf("AR: %s\n", build_conf.ar);
        #endif
    }
    const cJSON *objcopy = cJSON_GetObjectItem(parse_data, "OBJCOPY");
    if (cJSON_IsString(objcopy)) {
        strcpy(build_conf.objcopy, objcopy->valuestring);
        #ifdef DEBUG
        printf("OBJCOPY: %s\n", build_conf.objcopy);
        #endif
    }

    const cJSON *build = cJSON_GetObjectItem(parse_data, "BUILD");
    if (cJSON_IsArray(build)) {
        struct build_cmd *bcmd_ptr = build_conf.bhead;
        const cJSON *build_cmd;
        cJSON_ArrayForEach(build_cmd, build) {
            if (cJSON_IsArray(build_cmd)) {
                // Initialize bhead
                if (bcmd_ptr == NULL) {
                    build_conf.bhead = malloc(sizeof(struct build_cmd));
                    bcmd_ptr = build_conf.bhead;
                } else if (bcmd_ptr->next == NULL) {
                    bcmd_ptr->next = malloc(sizeof(struct build_cmd));
                    bcmd_ptr = bcmd_ptr->next;
                }
                bzero(bcmd_ptr, sizeof(struct build_cmd));

                const cJSON *build_inst;
                cJSON_ArrayForEach(build_inst, build_cmd) {
                    bcmd_ptr->inst = realloc(bcmd_ptr->inst, 
                                             (bcmd_ptr->inst_size+1) * sizeof(struct build_inst));

                    bzero(bcmd_ptr->inst + bcmd_ptr->inst_size, sizeof(struct build_inst));
                    const cJSON *bpath = cJSON_GetObjectItem(build_inst, "path"); 
                    if (cJSON_IsString(bpath)) {
                        strcpy(bcmd_ptr->inst[bcmd_ptr->inst_size].path, bpath->valuestring);
                        #ifdef DEBUG
                        printf("path: %s\n", bcmd_ptr->inst[bcmd_ptr->inst_size].path);
                        #endif
                    }
                    const cJSON *id_array = cJSON_GetObjectItem(build_inst, "id"); 
                    if (cJSON_IsArray(id_array)) {
                        const cJSON *id;
                        #ifdef DEBUG
                        printf("id:");
                        #endif
                        cJSON_ArrayForEach(id, id_array) {
                            if (cJSON_IsNumber(id)) {
                                bcmd_ptr->inst[bcmd_ptr->inst_size].params = realloc(bcmd_ptr->inst[bcmd_ptr->inst_size].params, 
                                                        (bcmd_ptr->inst[bcmd_ptr->inst_size].params_size+1) * sizeof(u8));
                                bcmd_ptr->inst[bcmd_ptr->inst_size].params[bcmd_ptr->inst[bcmd_ptr->inst_size].params_size] = id->valueint;
                                #ifdef DEBUG
                                printf("%u ", bcmd_ptr->inst[bcmd_ptr->inst_size].params[bcmd_ptr->inst[bcmd_ptr->inst_size].params_size]);
                                #endif
                                bcmd_ptr->inst[bcmd_ptr->inst_size].params_size++;
                            }
                        }
                        #ifdef DEBUG
                        printf("\n");
                        #endif
                    }
                    bcmd_ptr->inst_size++;
                }
            }
        }
    }

    const cJSON *deploy = cJSON_GetObjectItem(parse_data, "DEPLOY");
    if (cJSON_IsArray(deploy)) {
        struct deploy_cmd *dcmd_ptr = build_conf.dhead;
        const cJSON *deploy_cmd;
        cJSON_ArrayForEach(deploy_cmd, deploy) {
            // Initialize dhead
            if (dcmd_ptr == NULL) {
                build_conf.dhead = malloc(sizeof(struct deploy_cmd));
                dcmd_ptr = build_conf.dhead;
            } else if (dcmd_ptr->next == NULL) {
                dcmd_ptr->next = malloc(sizeof(struct deploy_cmd));
                dcmd_ptr = dcmd_ptr->next;
            }
            bzero(dcmd_ptr, sizeof(struct deploy_cmd));

            const cJSON *src = cJSON_GetObjectItem(deploy_cmd, "src"); 
            if (cJSON_IsString(src)) {
                strcpy(dcmd_ptr->src, src->valuestring);
                #ifdef DEBUG
                printf("src: %s\n", dcmd_ptr->src);
                #endif
            }
            const cJSON *dest = cJSON_GetObjectItem(deploy_cmd, "dest"); 
            if (cJSON_IsString(dest)) {
                strcpy(dcmd_ptr->dest, dest->valuestring);
                #ifdef DEBUG
                printf("dest: %s\n", dcmd_ptr->dest);
                #endif
            }
        }
    }
    cJSON_Delete(parse_data);

    return 0;
}

/*
 * ARGS:
 *      - arg: Array of 2 strings (variadic argument, needs to end with NULL):
 *              1 - Name of the directory where the makefile is (from build path) 
 *              2 - Arguments to be used on the variable CFLAGS
 */
static err callMakefileVA(const char **arg, ...) {
    pid_t *pids = NULL;
    size_t num = 0;
    char path[128];

    strcpy(path, SELECTED_ARCH.path);
    strcat(path, "/project/build/");

    va_list va;
    va_start(va, arg);
    const char **arg_va = arg;
    while (arg_va != NULL) {
        pids = realloc(pids, (++num) * sizeof(pids));
        char query[512] = "AFLAGS=";
        strcat(query, arg_va[1]);

        char fpath[512];
        strcpy(fpath, path);
        strcat(fpath, arg_va[0]);

        puts(path);
        puts(query);

        pids[num-1] = RUN_PROCESS_IMAGE(NULL, "/bin/make", "make", "-C", fpath, "clean", "all", query, NULL);

        arg_va = va_arg(va, const char **);
    }
    // Get every directory in the project
    
    for (size_t i = 0; i < num; i++) {
        waitpid(pids[i], NULL, 0);
    }

    free(pids);

    return 0;
}

/*
 * ARGS:
 *      - arg: Array of arrays of 2 strings:
 *              1 - Name of the directory where the makefile is (from build path) 
 *              2 - Arguments to be used on the variable CFLAGS
 */
static err callMakefile(const char (*arg)[2][256], size_t num) {
    pid_t *pids = malloc(num * sizeof(pid_t));

    char compiler[64], loader[64], archiver[64], objcopy[64];
    sprintf(compiler, "CC=%s", build_conf.cc);
    sprintf(loader, "LD=%s", build_conf.ld);
    sprintf(archiver, "AR=%s", build_conf.ar);
    sprintf(objcopy, "OBJCOPY=%s", build_conf.objcopy);
    for (size_t i = 0; i < num; i++) {
        char query[512];
        strcpy(query, "AFLAGS=");
        puts(arg[i][1]);
        strcat(query, arg[i][1]);

        puts(arg[i][0]);
        puts(query);

        pids[i] = RUN_PROCESS_IMAGE(NULL, "/bin/make", "make", "-C", arg[i][0], "clean", "all", 
                                    compiler, loader, archiver, objcopy, query, NULL);
    
    }
    for (size_t i = 0; i < num; i++) {
        waitpid(pids[i], NULL, 0);
    }

    free(pids);

    return 0;
}

err CALL_MAKEFILES(CONFIG *config) {
    char config_path[256];
    strcpy(config_path, SELECTED_ARCH.path);
    strcat(config_path, "/project/build.json");

    #ifdef DEBUG
    printf("Parsing config at: %s\n", config_path);
    #endif

    parseBuildConfig(config_path);

    struct build_cmd *build_ptr = build_conf.bhead;
    while (build_ptr != NULL) {
        char (*make_args)[2][256] = malloc(sizeof(char) * 2 * 256 * build_ptr->inst_size);
        const COMP *comp;

        for (size_t inst = 0; inst < build_ptr->inst_size; inst++) {
            sprintf(make_args[inst][0], "%s/project/%s", SELECTED_ARCH.path, build_ptr->inst[inst].path);
            make_args[inst][1][0] = 0;           // Important for the string related functions

            // Add information about cache size (important for default bench)
            char cache_str[32], num_buf[32];
            for (size_t cache_idx = 0; cache_idx < SELECTED_ARCH.desc.CACHE_LVL; cache_idx++) {
                sprintf(cache_str, "-DL%ld=", cache_idx+1);
                itos(SELECTED_ARCH.desc.CACHES[cache_idx].SIZE/SELECTED_ARCH.desc.CACHES[cache_idx].SHARED_NUM, num_buf);
                strcat(cache_str, num_buf);
                strcat(cache_str, " ");
                strcat(make_args[inst][1], cache_str);
            }
            strcat(make_args[inst][1], "-DDDR=");
            itos(SELECTED_ARCH.desc.CACHES[SELECTED_ARCH.desc.CACHE_LVL-1].SIZE << 1, num_buf);
            strcat(make_args[inst][1], num_buf);
            strcat(make_args[inst][1], " ");
            
            for (size_t id_i = 0; id_i < build_ptr->inst[inst].params_size; id_i++) {
                if (build_ptr->inst[inst].params[id_i] == config->VICTIM_ID) {
                    strcat(make_args[inst][1], "-DVICTIM ");
                }
                if (GET_COMP_BY_ID(config, build_ptr->inst[inst].params[id_i], &comp) != -1) {
                    for (size_t prop_i = 0; prop_i < comp->PBUFFER->NUM; prop_i++) {
                        catPropDefine(make_args[inst][1], comp->PBUFFER->PROPS + prop_i);
                    }
                }
            }

            #ifdef TARGET_SIZE_DIVISOR
            strcat(make_args[inst][1], " -DDIV="STR_VALUE(TARGET_SIZE_DIVISOR));
            #endif
            #ifdef DEBUG
            printf("Component at %s has flags: %s\n", make_args[inst][0], make_args[inst][1]);
            #endif
        }

        callMakefile(make_args, build_ptr->inst_size);

        free(make_args);
        build_ptr = build_ptr->next;
    }

    destroyBuildConf();
    return 0;
}

err DEPLOY_FILES(const char *dest) {
    char src_path[512], dest_path[512];
    char config_path[256];
    strcpy(config_path, SELECTED_ARCH.path);
    strcat(config_path, "/project/build.json");

    #ifdef DEBUG
    printf("Parsing config at: %s\n", config_path);
    #endif

    parseBuildConfig(config_path);
    
    struct deploy_cmd *deploy_ptr = build_conf.dhead;
    while (deploy_ptr != NULL) {
        sprintf(dest_path, "%s/%s", dest, deploy_ptr->dest);
        sprintf(src_path, "%s/project/%s", SELECTED_ARCH.path, deploy_ptr->src);

        #ifdef DEBUG
        printf("Copying file in %s to %s\n", src_path, dest_path);
        #endif

        copyFile(src_path, dest_path);

        deploy_ptr = deploy_ptr->next;
    }

    destroyBuildConf();
    return 0;
}
