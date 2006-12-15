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
 
#include <sandesha2_permanent_create_seq_mgr.h>
#include <sandesha2_permanent_bean_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_error.h>
#include <sandesha2_rm_bean.h>
#include <sandesha2_sqls.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <axis2_types.h>

/** 
 * @brief Sandesha2 Permanent Create Sequence Manager Struct Impl
 *   Sandesha2 Permanent Create Sequence Manager
 */ 
typedef struct sandesha2_permanent_create_seq_mgr
{
    sandesha2_create_seq_mgr_t seq_mgr;
    sandesha2_permanent_bean_mgr_t *bean_mgr;

}sandesha2_permanent_create_seq_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(seq_mgr) \
    ((sandesha2_permanent_create_seq_mgr_t *) seq_mgr)

static int 
sandesha2_create_seq_find_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    int i;
    sandesha2_bean_mgr_args_t *args = (sandesha2_bean_mgr_args_t *) not_used;
    axis2_array_list_t *data_list = (axis2_array_list_t *) args->data;
    const axis2_env_t *env = args->env;
    for(i = 0; i < argc; i++)
    {
        sandesha2_create_seq_bean_t *bean = NULL;
        if(0 == AXIS2_STRCMP(col_name[i], "create_seq_msg_id"))
            sandesha2_create_seq_bean_set_create_seq_msg_id(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "internal_seq_id"))
            sandesha2_create_seq_bean_set_internal_seq_id(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "seq_id"))
            sandesha2_create_seq_bean_set_seq_id(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "create_seq_msg_store_key"))
            sandesha2_create_seq_bean_set_create_seq_msg_store_key(bean, env, 
                argv[i] ? argv[i] : "NULL");
        if(0 == AXIS2_STRCMP(col_name[i], "ref_msg_store_key"))
            sandesha2_create_seq_bean_set_ref_msg_store_key(bean, env, 
                argv[i] ? argv[i] : "NULL");
        axis2_array_list_add(data_list, env, bean);
    }
    return 0;
}

static int 
sandesha2_create_seq_retrieve_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    sandesha2_bean_mgr_args_t *args = (sandesha2_bean_mgr_args_t *) not_used;
    const axis2_env_t *env = args->env;
    sandesha2_create_seq_bean_t *bean = (sandesha2_create_seq_bean_t *) args->data;
    sandesha2_create_seq_bean_set_create_seq_msg_id(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_create_seq_bean_set_internal_seq_id(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_create_seq_bean_set_seq_id(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_create_seq_bean_set_create_seq_msg_store_key(bean, env, 
        argv[0] ? argv[0] : "NULL");
    sandesha2_create_seq_bean_set_ref_msg_store_key(bean, env, 
        argv[0] ? argv[0] : "NULL");
    return 0;
}

static int 
sandesha2_create_seq_count_callback(
    void *not_used, 
    int argc, 
    char **argv, 
    char **col_name)
{
    int *count = (int *) not_used;
    *count = AXIS2_ATOI(argv[0]);
    return 0;
}

static void AXIS2_CALL 
sandesha2_permanent_create_seq_mgr_free(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *envv);

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_create_seq_mgr_insert(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean);

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_create_seq_mgr_remove(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id);

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_permanent_create_seq_mgr_retrieve(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id);

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_create_seq_mgr_update(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean);

static axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_create_seq_mgr_find(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean);

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_permanent_create_seq_mgr_find_unique(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean);

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_create_seq_mgr_match(
    sandesha2_permanent_bean_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

static const sandesha2_create_seq_mgr_ops_t create_seq_mgr_ops = 
{
    sandesha2_permanent_create_seq_mgr_free,
    sandesha2_permanent_create_seq_mgr_insert,
    sandesha2_permanent_create_seq_mgr_remove,
    sandesha2_permanent_create_seq_mgr_retrieve,
    sandesha2_permanent_create_seq_mgr_update,
    sandesha2_permanent_create_seq_mgr_find,
    sandesha2_permanent_create_seq_mgr_find_unique,
};

AXIS2_EXTERN sandesha2_create_seq_mgr_t * AXIS2_CALL
sandesha2_permanent_create_seq_mgr_create(
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_ctx_t *ctx)
{
    sandesha2_permanent_create_seq_mgr_t *seq_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    seq_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_create_seq_mgr_t));

    seq_mgr_impl->bean_mgr = sandesha2_permanent_bean_mgr_create(env, 
        storage_mgr, ctx, SANDESHA2_BEAN_MAP_CREATE_SEQUENCE);
    seq_mgr_impl->bean_mgr->ops.match = sandesha2_permanent_create_seq_mgr_match;
    seq_mgr_impl->seq_mgr.ops = create_seq_mgr_ops;
    return &(seq_mgr_impl->seq_mgr);
}

static void AXIS2_CALL
sandesha2_permanent_create_seq_mgr_free(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_create_seq_mgr_t *seq_mgr_impl = NULL;
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);

    if(seq_mgr_impl->bean_mgr)
    {
        sandesha2_permanent_bean_mgr_free(seq_mgr_impl->bean_mgr, env);
        seq_mgr_impl->bean_mgr = NULL;
    }
    if(seq_mgr_impl)
    {
        AXIS2_FREE(env->allocator, seq_mgr_impl);
        seq_mgr_impl = NULL;
    }
}

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_create_seq_mgr_insert(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean)
{
    sandesha2_permanent_create_seq_mgr_t *seq_mgr_impl = NULL;
    axis2_bool_t ret = AXIS2_FALSE;
    axis2_char_t *insert_sql = NULL;
    axis2_char_t *internal_seq_id = NULL;
    axis2_char_t *create_seq_msg_id = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *create_seq_msg_store_key = NULL;
    axis2_char_t *ref_msg_store_key = NULL;

    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_create_seq_mgr_insert");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);

    create_seq_msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id(
        (sandesha2_rm_bean_t *) bean, env);
    internal_seq_id = sandesha2_create_seq_bean_get_internal_seq_id(bean, env);
    seq_id = sandesha2_create_seq_bean_get_seq_id(bean, env);
    create_seq_msg_store_key = 
        sandesha2_create_seq_bean_get_create_seq_msg_store_key(bean, env);
    ref_msg_store_key = 
        sandesha2_create_seq_bean_get_ref_msg_store_key(bean, env);
    insert_sql = axis2_strcat(env, "insert into create_seq(create_seq_msg_id, "\
        "internal_seq_id, seq_id, create_seq_msg_store_key, ref_msg_store_key) "\
        "values('", create_seq_msg_id, "','", internal_seq_id, "','", seq_id, 
        "','", create_seq_msg_store_key, "','", ref_msg_store_key, "');", NULL);
    ret = sandesha2_permanent_bean_mgr_insert(seq_mgr_impl->bean_mgr, env, 
        insert_sql);
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_create_seq_mgr_insert");
    return ret;
}

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_create_seq_mgr_remove(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id)
{
    sandesha2_permanent_create_seq_mgr_t *seq_mgr_impl = NULL;
    axis2_char_t *sql_remove = NULL;
    axis2_char_t *sql_retrieve = NULL;
    sandesha2_create_seq_bean_t *bean = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);
    sql_remove = axis2_strcat(env, 
        "delete from create_seq where create_seq_msg_id='",msg_id, "';", NULL);
    sql_retrieve = axis2_strcat(env, "select create_seq_mg_id, internal_seq_id,"\
        "seq_id, create_seq_msg_store_key, ref_msg_store_key from create_seq "\
        "where create_seq_msg_id='", msg_id,"';", NULL);
    bean = sandesha2_create_seq_bean_create(env);
    return sandesha2_permanent_bean_mgr_remove(seq_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_create_seq_retrieve_callback, 
            sql_retrieve, sql_remove);
}

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_permanent_create_seq_mgr_retrieve(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id)
{
    axis2_char_t *sql_retrieve = NULL;
    sandesha2_create_seq_bean_t *bean = NULL;
    sandesha2_permanent_create_seq_mgr_t *seq_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);
    sql_retrieve = axis2_strcat(env, "select create_seq_mg_id, internal_seq_id,"\
        "seq_id, create_seq_msg_store_key, ref_msg_store_key from create_seq "\
        "where create_seq_msg_id='", msg_id, "';", NULL);
    bean = sandesha2_create_seq_bean_create(env);
    return (sandesha2_create_seq_bean_t *) sandesha2_permanent_bean_mgr_retrieve(
        seq_mgr_impl->bean_mgr, env, (sandesha2_rm_bean_t *) bean, 
            sandesha2_create_seq_retrieve_callback, sql_retrieve);
}

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_create_seq_mgr_update(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean)
{
    sandesha2_permanent_create_seq_mgr_t *seq_mgr_impl = NULL;
    axis2_char_t *sql_update = NULL;
    axis2_char_t *sql_retrieve = NULL;
    axis2_bool_t ret = AXIS2_FALSE;
    sandesha2_create_seq_bean_t *old_bean = NULL;
    axis2_char_t *create_seq_msg_id = NULL;
    axis2_char_t *internal_seq_id = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *create_seq_msg_store_key = NULL;
    axis2_char_t *ref_msg_store_key = NULL;

    AXIS2_LOG_INFO(env->log, 
        "Entry:[sandesha2]sandesha2_permanent_create_seq_mgr_update");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);

    create_seq_msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id(
        (sandesha2_rm_bean_t *) bean, env);
    if(!create_seq_msg_id)
    {
        return AXIS2_FALSE;
    }
    internal_seq_id = sandesha2_create_seq_bean_get_internal_seq_id(bean, env);
    seq_id = sandesha2_create_seq_bean_get_seq_id(bean, env);
    create_seq_msg_store_key = 
        sandesha2_create_seq_bean_get_create_seq_msg_store_key(bean, env);
    ref_msg_store_key = sandesha2_create_seq_bean_get_ref_msg_store_key(bean, 
        env);
    sql_retrieve = axis2_strcat(env, "select create_seq_mg_id, internal_seq_id,"\
        "seq_id, create_seq_msg_store_key, ref_msg_store_key from create_seq "\
        "where create_seq_msg_id='", create_seq_msg_id, "';", NULL);
    sql_update = axis2_strcat(env, "update create_seq set internal_seq_id='", 
        internal_seq_id, "', seq_id='", seq_id, "', create_seq_msg_store_key='", 
            create_seq_msg_store_key, "', ref_msg_store_key='", 
            ref_msg_store_key, "' where create_seq_msg_id='", create_seq_msg_id, 
            "';", NULL);
    old_bean = sandesha2_create_seq_bean_create(env);
    ret = sandesha2_permanent_bean_mgr_update(seq_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) old_bean, sandesha2_create_seq_retrieve_callback, 
        sql_retrieve, sql_update);
    AXIS2_LOG_INFO(env->log, 
        "Exit:[sandesha2]sandesha2_permanent_create_seq_mgr_update:return:%d", 
            ret);
    return ret;
}

static axis2_array_list_t *AXIS2_CALL
sandesha2_permanent_create_seq_mgr_find(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_permanent_create_seq_mgr_t *seq_mgr_impl = NULL;
    axis2_array_list_t *ret = NULL;
    AXIS2_LOG_INFO(env->log, 
        "Entry:[sandesha2]sandesha2_permanent_create_seq_mgr_find");
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr); 
    sql_find = axis2_strcat(env, "select create_seq_msg_id,internal_seq_id,"\
        "seq_id,create_seq_msg_store_key, ref_msg_store_key from create_seq;", 
        NULL);
    sql_count = "select count(*) as no_recs from create_seq;";
    ret = sandesha2_permanent_bean_mgr_find(seq_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean, sandesha2_create_seq_find_callback,
        sandesha2_create_seq_count_callback, sql_find, sql_count);
    AXIS2_LOG_INFO(env->log, 
        "Exit:[sandesha2]sandesha2_permanent_create_seq_mgr_find");
    return ret;
}

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_permanent_create_seq_mgr_find_unique(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean)
{
    axis2_char_t *sql_find = NULL;
    axis2_char_t *sql_count = NULL;
    sandesha2_permanent_create_seq_mgr_t *seq_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);
    sql_find = axis2_strcat(env, "select create_seq_msg_id,internal_seq_id,"\
        "seq_id,create_seq_msg_store_key, ref_msg_store_key from create_seq;", 
        NULL);
    sql_count = "select count(*) as no_recs from create_seq;";
    return (sandesha2_create_seq_bean_t *) sandesha2_permanent_bean_mgr_find_unique(
        seq_mgr_impl->bean_mgr, env, (sandesha2_rm_bean_t *) bean, 
        sandesha2_create_seq_find_callback, sandesha2_create_seq_count_callback, 
        sql_find, sql_count);
}

static axis2_bool_t AXIS2_CALL
sandesha2_permanent_create_seq_mgr_match(
    sandesha2_permanent_bean_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate)
{
    axis2_bool_t equal = AXIS2_TRUE;
    axis2_char_t *msg_id = NULL;
    axis2_char_t *temp_msg_id = NULL;
    axis2_char_t *seq_id = NULL;
    axis2_char_t *temp_seq_id = NULL;
    axis2_char_t *internal_seq_id = NULL;
    axis2_char_t *temp_internal_seq_id = NULL;
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Entry:sandesha2_permanent_create_seq_mgr_match");
    msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id(bean, env);
    temp_msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id(candidate, 
        env);
    if(msg_id && temp_msg_id && 0 != AXIS2_STRCMP(msg_id, temp_msg_id))
    {
        equal = AXIS2_FALSE;
    }
    seq_id = sandesha2_create_seq_bean_get_seq_id(
        (sandesha2_create_seq_bean_t *) bean, env);
    temp_seq_id = sandesha2_create_seq_bean_get_seq_id(
        (sandesha2_create_seq_bean_t *) candidate, env);
    if(seq_id && temp_seq_id && 0 != AXIS2_STRCMP(seq_id, temp_seq_id))
    {
        equal = AXIS2_FALSE;
    }
    internal_seq_id = sandesha2_create_seq_bean_get_internal_seq_id(
        (sandesha2_create_seq_bean_t *) bean, env);
    temp_internal_seq_id = sandesha2_create_seq_bean_get_internal_seq_id(
        (sandesha2_create_seq_bean_t *) candidate, env);
    if(internal_seq_id && temp_internal_seq_id && 0 != AXIS2_STRCMP(
        internal_seq_id, temp_internal_seq_id))
    {
        equal = AXIS2_FALSE;
    }
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_create_seq_mgr_match:equal:%d", 
            equal);
    return equal;
}
