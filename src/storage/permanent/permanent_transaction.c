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
 
#include <sandesha2_permanent_transaction.h>
#include <sandesha2_transaction.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_rm_bean.h>
#include <sandesha2_storage_mgr.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <platforms/axis2_platform_auto_sense.h>

typedef struct sandesha2_permanent_transaction_impl 
    sandesha2_permanent_transaction_impl_t;

/** 
 * @brief Sandesha Permanent Transaction Struct Impl
 *   Sandesha2 Permanent Transaction 
 */ 
struct sandesha2_permanent_transaction_impl
{
    sandesha2_transaction_t trans;
    sandesha2_storage_mgr_t *storage_mgr;
    axis2_array_list_t *enlisted_beans;
    axis2_thread_mutex_t *mutex;
};

#define SANDESHA2_INTF_TO_IMPL(trans) \
    ((sandesha2_permanent_transaction_impl_t *) trans)

static axis2_status_t
sandesha2_permanent_transaction_free(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env);

static axis2_bool_t
sandesha2_permanent_transaction_is_active(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env);

static void
sandesha2_permanent_transaction_commit(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env);

static void
sandesha2_permanent_transaction_rollback(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env);

static void
sandesha2_permanent_transaction_enlist(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *rm_bean);

static void 
sandesha2_permanent_transaction_release_locks(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env);

static const sandesha2_transaction_ops_t transaction_ops = 
{
    sandesha2_permanent_transaction_free,
    sandesha2_permanent_transaction_is_active,
    sandesha2_permanent_transaction_commit,
    sandesha2_permanent_transaction_rollback,
    sandesha2_permanent_transaction_enlist
};

AXIS2_EXTERN sandesha2_transaction_t* AXIS2_CALL
sandesha2_permanent_transaction_create(
    const axis2_env_t *env,
    sandesha2_storage_mgr_t *storage_mgr)
{
    sandesha2_permanent_transaction_impl_t *trans_impl = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    
    trans_impl =  (sandesha2_permanent_transaction_impl_t *)AXIS2_MALLOC 
        (env->allocator, sizeof(sandesha2_permanent_transaction_impl_t));

    trans_impl->storage_mgr = storage_mgr;
    trans_impl->enlisted_beans = axis2_array_list_create(env, 0);
    trans_impl->mutex = axis2_thread_mutex_create(env->allocator,
        AXIS2_THREAD_MUTEX_DEFAULT);
    trans_impl->trans.ops = &transaction_ops;

    return &(trans_impl->trans);
}

static axis2_status_t
sandesha2_permanent_transaction_free(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env)
{
    sandesha2_permanent_transaction_impl_t *trans_impl = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    trans_impl = SANDESHA2_INTF_TO_IMPL(trans);

    if(trans_impl->mutex)
    {
        axis2_thread_mutex_destroy(trans_impl->mutex);
        trans_impl->mutex = NULL;
    } 
    if(trans_impl->enlisted_beans)
    {
        AXIS2_ARRAY_LIST_FREE(trans_impl->enlisted_beans, env);
        trans_impl->enlisted_beans = NULL;
    }
    if(trans_impl)
    {
        AXIS2_FREE(env->allocator, trans_impl);
        trans_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

static axis2_bool_t
sandesha2_permanent_transaction_is_active(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env)
{
    sandesha2_permanent_transaction_impl_t *trans_impl = NULL;
    trans_impl = SANDESHA2_INTF_TO_IMPL(trans);
    int size = 0;
    if(trans_impl->enlisted_beans)
        size = AXIS2_ARRAY_LIST_SIZE(trans_impl->enlisted_beans, env);
    if(size > 0)
        return AXIS2_TRUE;
    else
        return AXIS2_FALSE;
}

static void
sandesha2_permanent_transaction_commit(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env)
{
    sandesha2_permanent_transaction_release_locks(trans, env);
}

static void
sandesha2_permanent_transaction_rollback(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env)
{
    sandesha2_permanent_transaction_release_locks(trans, env);
}

static void 
sandesha2_permanent_transaction_release_locks(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env)
{
    sandesha2_permanent_transaction_impl_t *trans_impl = NULL;
    int i = 0, size = 0;
    trans_impl = SANDESHA2_INTF_TO_IMPL(trans);
    if(trans_impl->enlisted_beans)
        size = AXIS2_ARRAY_LIST_SIZE(trans_impl->enlisted_beans, env);
    for(i = 0; i < size; i++)
    {
        sandesha2_rm_bean_t *rm_bean_l = NULL;
        sandesha2_rm_bean_t *rm_bean = (sandesha2_rm_bean_t *) 
            AXIS2_ARRAY_LIST_GET(trans_impl->enlisted_beans, env, i);
        rm_bean_l = sandesha2_rm_bean_get_base(rm_bean, env);
        axis2_thread_mutex_lock(trans_impl->mutex);
        sandesha2_rm_bean_set_transaction(rm_bean_l, env, NULL);
        axis2_thread_mutex_unlock(trans_impl->mutex);
    }
    AXIS2_ARRAY_LIST_FREE(trans_impl->enlisted_beans, env);
    trans_impl->enlisted_beans = NULL;
}
   
static void
sandesha2_permanent_transaction_enlist(
    sandesha2_transaction_t *trans,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *rm_bean)
{
    sandesha2_rm_bean_t *rm_bean_l = NULL;
    sandesha2_permanent_transaction_impl_t *trans_impl = NULL;
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Start:sandesha2_permanent_transaction_enlist");
    trans_impl = SANDESHA2_INTF_TO_IMPL(trans);
    rm_bean_l = sandesha2_rm_bean_get_base(rm_bean, env);
    if(rm_bean)
    {
        sandesha2_transaction_t *other = NULL;
        axis2_thread_mutex_lock(trans_impl->mutex);
        other = sandesha2_rm_bean_get_transaction(rm_bean_l, env);
        /*while(other && other != trans)
        {
            int size = 0;
            if(trans_impl->enlisted_beans)
                size = AXIS2_ARRAY_LIST_SIZE(trans_impl->enlisted_beans, env);
            AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "size:%d", size);
            if(size > 0)
            {
                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Possible deadlock");
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_POSSIBLE_DEADLOCK, 
                    AXIS2_FAILURE);
            }
            AXIS2_SLEEP(6);
            other = sandesha2_rm_bean_get_transaction(rm_bean_l, env);
        }*/
        if(!other)
        {
            sandesha2_rm_bean_set_transaction(rm_bean_l, env, trans);
            AXIS2_ARRAY_LIST_ADD(trans_impl->enlisted_beans, env, rm_bean);
        }   
        axis2_thread_mutex_unlock(trans_impl->mutex);
    }    
    AXIS2_LOG_INFO(env->log, 
        "[sandesha2]Exit:sandesha2_permanent_transaction_enlist");
}
