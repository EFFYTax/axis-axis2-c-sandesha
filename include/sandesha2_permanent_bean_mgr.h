/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SANDESHA2_PERMANENT_BEAN_MGR_H
#define SANDESHA2_PERMANENT_BEAN_MGR_H

/**
 * @file sandesha2_permanent_bean_mgr.h
 * @brief Sandesha In Memory Bean Manager Interface
 */

#include <axis2_allocator.h>
#include <axis2_env.h>
#include <axis2_error.h>
#include <axis2_string.h>
#include <axis2_utils.h>
#include <axis2_array_list.h>
#include <sandesha2_rm_bean.h>
#include <sandesha2_msg_store_bean.h>
#include <sqlite3.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sandesha2_bean_mgr_args sandesha2_bean_mgr_args_t;
typedef struct sandesha2_permanent_bean_mgr sandesha2_permanent_bean_mgr_t;
typedef struct sandesha2_permanent_bean_mgr_ops sandesha2_permanent_bean_mgr_ops_t;
struct sandesha2_storage_mgr;
struct axis2_conf_ctx;

AXIS2_DECLARE_DATA struct sandesha2_permanent_bean_mgr_ops
{
    axis2_bool_t (AXIS2_CALL *
            match) (
                sandesha2_permanent_bean_mgr_t *bean_mgr,
                const axis2_env_t *env,
                sandesha2_rm_bean_t *bean,
                sandesha2_rm_bean_t *candidate);
};

AXIS2_DECLARE_DATA struct sandesha2_permanent_bean_mgr
{
    sandesha2_permanent_bean_mgr_ops_t ops;
};

struct sandesha2_bean_mgr_args
{
    const axis2_env_t *env;
    void *data;
};

AXIS2_EXTERN sandesha2_permanent_bean_mgr_t * AXIS2_CALL
sandesha2_permanent_bean_mgr_create(
    const axis2_env_t *env,
    struct sandesha2_storage_mgr *storage_mgr,
    struct axis2_conf_ctx *conf_ctx,
    axis2_char_t *key);

void AXIS2_CALL
sandesha2_permanent_bean_mgr_free(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_insert(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve,
    axis2_char_t *sql_stmt_update,
    axis2_char_t *sql_stmt_insert);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_remove(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve,
    axis2_char_t *sql_stmt_remove);

sandesha2_rm_bean_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_retrieve(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    int (*retrieve_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_update(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*update_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_retrieve_old_bean,
    axis2_char_t *sql_stmt_update);

axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_find(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*find_func)(void *, int, char **, char **),
    int (*count_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_find,
    axis2_char_t *sql_stmt_count);

sandesha2_rm_bean_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_find_unique(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    int (*find_func)(void *, int, char **, char **),
    int (*count_func)(void *, int, char **, char **),
    axis2_char_t *sql_stmt_find,
    axis2_char_t *sql_stmt_count);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_match(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

sandesha2_msg_store_bean_t *AXIS2_CALL
sandesha2_permanent_bean_mgr_retrieve_msg_store_bean(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_insert_msg_store_bean(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    sandesha2_msg_store_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_permanent_bean_mgr_remove_msg_store_bean(
    sandesha2_permanent_bean_mgr_t *bean_mgr,
    const axis2_env_t *env,
    axis2_char_t *key);

int
sandesha2_permanent_bean_mgr_busy_handler(
    sqlite3* dbconn,
    char *sql_stmt,
    int (*callback_func)(void *, int, char **, char **),
    void *arg,
    char **error_msg,
    int rc);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SANDESHA2_PERMANENT_BEAN_MGR_H */