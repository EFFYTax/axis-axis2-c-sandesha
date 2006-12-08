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
 
#include <sandesha2_inmemory_create_seq_mgr.h>
#include <sandesha2_inmemory_bean_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_error.h>
#include <sandesha2_rm_bean.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>

/** 
 * @brief Sandesha2 Inmemory Create Sequence Manager Struct Impl
 *   Sandesha2 Inmemory Create Sequence Manager
 */ 
typedef struct sandesha2_inmemory_create_seq_mgr
{
    sandesha2_create_seq_mgr_t seq_mgr;
    sandesha2_inmemory_bean_mgr_t *bean_mgr;
    axis2_hash_t *table;
    axis2_thread_mutex_t *mutex;

}sandesha2_inmemory_create_seq_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(seq_mgr) \
    ((sandesha2_inmemory_create_seq_mgr_t *) seq_mgr)

static axis2_status_t AXIS2_CALL 
sandesha2_inmemory_create_seq_mgr_free(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *envv);

static axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_insert(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean);

static axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_remove(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id);

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_retrieve(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id);

static axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_update(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean);

static axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_find(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean);

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_find_unique(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean);

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_match(
    sandesha2_inmemory_bean_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *bean,
    sandesha2_rm_bean_t *candidate);

static const sandesha2_create_seq_mgr_ops_t create_seq_mgr_ops = 
{
    sandesha2_inmemory_create_seq_mgr_free,
    sandesha2_inmemory_create_seq_mgr_insert,
    sandesha2_inmemory_create_seq_mgr_remove,
    sandesha2_inmemory_create_seq_mgr_retrieve,
    sandesha2_inmemory_create_seq_mgr_update,
    sandesha2_inmemory_create_seq_mgr_find,
    sandesha2_inmemory_create_seq_mgr_find_unique,
};

AXIS2_EXTERN sandesha2_create_seq_mgr_t * AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_create(
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr,
    axis2_ctx_t *ctx)
{
    sandesha2_inmemory_create_seq_mgr_t *seq_mgr_impl = NULL;
    axis2_property_t *property = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    seq_mgr_impl = AXIS2_MALLOC(env->allocator, 
                    sizeof(sandesha2_inmemory_create_seq_mgr_t));

    seq_mgr_impl->table = NULL;
    seq_mgr_impl->mutex = NULL;

    seq_mgr_impl->mutex = axis2_thread_mutex_create(env->allocator, 
            AXIS2_THREAD_MUTEX_DEFAULT);
    if(!seq_mgr_impl->mutex) 
    {
        sandesha2_inmemory_create_seq_mgr_free(&(seq_mgr_impl->seq_mgr), env);
        return NULL;
    }

    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            SANDESHA2_BEAN_MAP_CREATE_SEQUECE, AXIS2_FALSE);
    if(NULL != property)
        seq_mgr_impl->table = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!seq_mgr_impl->table)
    {
        axis2_property_t *property = NULL;

        property = axis2_property_create(env);
        seq_mgr_impl->table = axis2_hash_make(env);
        if(!property || !seq_mgr_impl->table)
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return NULL;
        }
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, seq_mgr_impl->table);
        AXIS2_PROPERTY_SET_FREE_FUNC(property, env, axis2_hash_free_void_arg);
        AXIS2_CTX_SET_PROPERTY(ctx, env, SANDESHA2_BEAN_MAP_CREATE_SEQUECE, 
                property, AXIS2_FALSE);
    }
    seq_mgr_impl->bean_mgr = sandesha2_inmemory_bean_mgr_create(env, 
        storage_mgr, ctx, SANDESHA2_BEAN_MAP_CREATE_SEQUECE);
    seq_mgr_impl->bean_mgr->ops.match = sandesha2_inmemory_create_seq_mgr_match;
    seq_mgr_impl->seq_mgr.ops = create_seq_mgr_ops;
    return &(seq_mgr_impl->seq_mgr);
}

static axis2_status_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_free(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env)
{
    sandesha2_inmemory_create_seq_mgr_t *seq_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);

    if(seq_mgr_impl->mutex)
    {
        axis2_thread_mutex_destroy(seq_mgr_impl->mutex);
        seq_mgr_impl->mutex = NULL;
    }
    if(seq_mgr_impl->bean_mgr)
    {
        sandesha2_inmemory_bean_mgr_free(seq_mgr_impl->bean_mgr, env);
        seq_mgr_impl->bean_mgr = NULL;
    }
    if(seq_mgr_impl->table)
    {
        axis2_hash_free(seq_mgr_impl->table, env);
        seq_mgr_impl->table = NULL;
    }
    if(seq_mgr_impl)
    {
        AXIS2_FREE(env->allocator, seq_mgr_impl);
        seq_mgr_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

static axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_insert(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean)
{
    sandesha2_inmemory_create_seq_mgr_t *seq_mgr_impl = NULL;
    axis2_char_t *msg_id = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);

    msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id(bean, env);
    return sandesha2_inmemory_bean_mgr_insert(seq_mgr_impl->bean_mgr, env, 
        msg_id, (sandesha2_rm_bean_t *) bean);
}

static axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_remove(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id)
{
    sandesha2_inmemory_create_seq_mgr_t *seq_mgr_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);
    return sandesha2_inmemory_bean_mgr_remove(seq_mgr_impl->bean_mgr, env, 
        msg_id);
}

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_retrieve(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    axis2_char_t *msg_id)
{
    sandesha2_inmemory_create_seq_mgr_t *seq_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, msg_id, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);
    return (sandesha2_create_seq_bean_t *) sandesha2_inmemory_bean_mgr_retrieve(
        seq_mgr_impl->bean_mgr, env, msg_id);
}

static axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_update(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean)
{
    sandesha2_inmemory_create_seq_mgr_t *seq_mgr_impl = NULL;
    axis2_char_t *msg_id = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);

    msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id(bean, env);
    if(!msg_id)
    {
        return AXIS2_FALSE;
    }
    return sandesha2_inmemory_bean_mgr_update(seq_mgr_impl->bean_mgr, env, 
        msg_id, (sandesha2_rm_bean_t *) bean);
}

static axis2_array_list_t *AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_find(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean)
{
    sandesha2_inmemory_create_seq_mgr_t *seq_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);
    return sandesha2_inmemory_bean_mgr_find(seq_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean);
}

static sandesha2_create_seq_bean_t *AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_find_unique(
    sandesha2_create_seq_mgr_t *seq_mgr,
    const axis2_env_t *env,
    sandesha2_create_seq_bean_t *bean)
{
    sandesha2_inmemory_create_seq_mgr_t *seq_mgr_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    AXIS2_PARAM_CHECK(env->error, bean, AXIS2_FALSE);
    seq_mgr_impl = SANDESHA2_INTF_TO_IMPL(seq_mgr);
    
    return (sandesha2_create_seq_bean_t *) 
        sandesha2_inmemory_bean_mgr_find_unique(seq_mgr_impl->bean_mgr, env, 
        (sandesha2_rm_bean_t *) bean);
}

axis2_bool_t AXIS2_CALL
sandesha2_inmemory_create_seq_mgr_match(
    sandesha2_inmemory_bean_mgr_t *seq_mgr,
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
    msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id(
        (sandesha2_create_seq_bean_t *) bean, env);
    temp_msg_id = sandesha2_create_seq_bean_get_create_seq_msg_id(
        (sandesha2_create_seq_bean_t *) candidate, env);
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
    return equal;
}
