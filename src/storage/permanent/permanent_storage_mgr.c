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
 
#include <sandesha2_storage_mgr.h>
#include <sandesha2_create_seq_mgr.h>
#include <sandesha2_permanent_create_seq_mgr.h>
#include <sandesha2_invoker_mgr.h>
#include <sandesha2_permanent_invoker_mgr.h>
#include <sandesha2_next_msg_mgr.h>
#include <sandesha2_permanent_next_msg_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_permanent_sender_mgr.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_permanent_seq_property_mgr.h>
#include <sandesha2_transaction.h>
#include <sandesha2_permanent_transaction.h>
#include <sandesha2_constants.h>
#include <sandesha2_error.h>
#include <sandesha2_utils.h>
#include <sandesha2_rm_bean.h>
#include <axis2_log.h>
#include <axis2_hash.h>
#include <axis2_thread.h>
#include <axis2_property.h>
#include <axis2_msg_ctx.h>
#include <axis2_uuid_gen.h>
#include <axis2_conf_ctx.h>

/** 
 * @brief Sandesha2 Permanent Storage Manager Struct Impl
 *   Sandesha2 Permanent Storage Manager 
 */ 
typedef struct sandesha2_permanent_storage_mgr
{
	sandesha2_storage_mgr_t storage_mgr;
    axis2_char_t *SANDESHA2_MSG_MAP_KEY;
    sandesha2_create_seq_mgr_t *create_seq_mgr;
    sandesha2_next_msg_mgr_t *next_msg_mgr;
    sandesha2_seq_property_mgr_t *seq_property_mgr;
    sandesha2_sender_mgr_t *sender_mgr;
    sandesha2_invoker_mgr_t *invoker_mgr;
    axis2_conf_ctx_t *conf_ctx;
} sandesha2_permanent_storage_mgr_t;

#define SANDESHA2_INTF_TO_IMPL(trans) \
    ((sandesha2_permanent_storage_mgr_t *) trans)

AXIS2_EXTERN axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free_void_arg(
    void *storage_mgr,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL 
sandesha2_permanent_storage_mgr_free(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *envv);

struct sandesha2_transaction *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_transaction(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

void AXIS2_CALL
sandesha2_permanent_storage_mgr_enlist_bean(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *rm_bean);

sandesha2_create_seq_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_create_seq_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

sandesha2_next_msg_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_next_msg_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

sandesha2_sender_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_retrans_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

sandesha2_seq_property_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_seq_property_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

sandesha2_invoker_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_storage_map_mgr(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_set_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx);

axis2_conf_ctx_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx);
	
axis2_msg_ctx_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx);
		
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx);
			
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_update_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_msg_ctx(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init_storage(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_module_desc_t *module_desc);

axiom_soap_envelope_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_soap_envelope(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axis2_char_t *key);

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_soap_envelope(
    sandesha2_storage_mgr_t *storage,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_env,
    axis2_char_t *key);

static const sandesha2_storage_mgr_ops_t storage_mgr_ops = 
{
    sandesha2_permanent_storage_mgr_free,
    sandesha2_permanent_storage_mgr_free_void_arg,
    sandesha2_permanent_storage_mgr_get_transaction,
    sandesha2_permanent_storage_mgr_enlist_bean,
    sandesha2_permanent_storage_mgr_get_create_seq_mgr,
    sandesha2_permanent_storage_mgr_get_next_msg_mgr,
    sandesha2_permanent_storage_mgr_get_retrans_mgr,
    sandesha2_permanent_storage_mgr_get_seq_property_mgr,
    sandesha2_permanent_storage_mgr_get_storage_map_mgr,
    sandesha2_permanent_storage_mgr_set_ctx,
    sandesha2_permanent_storage_mgr_get_ctx,
    sandesha2_permanent_storage_mgr_init,
    sandesha2_permanent_storage_mgr_retrieve_msg_ctx,
    sandesha2_permanent_storage_mgr_store_msg_ctx,
    sandesha2_permanent_storage_mgr_update_msg_ctx,
    sandesha2_permanent_storage_mgr_remove_msg_ctx,
    sandesha2_permanent_storage_mgr_init_storage,
    sandesha2_permanent_storage_mgr_retrieve_soap_envelope,
    sandesha2_permanent_storage_mgr_store_soap_envelope
};

AXIS2_EXTERN sandesha2_storage_mgr_t * AXIS2_CALL
sandesha2_permanent_storage_mgr_create(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    axis2_ctx_t *ctx = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    storage_mgr_impl = AXIS2_MALLOC(env->allocator, 
        sizeof(sandesha2_permanent_storage_mgr_t));

    storage_mgr_impl->SANDESHA2_MSG_MAP_KEY = AXIS2_STRDUP("Sandesha2MessageMap", 
        env);
    storage_mgr_impl->conf_ctx = conf_ctx;
    ctx = AXIS2_CONF_CTX_GET_BASE(conf_ctx, env);
    storage_mgr_impl->create_seq_mgr = sandesha2_permanent_create_seq_mgr_create(
        env, &(storage_mgr_impl->storage_mgr), ctx);
    storage_mgr_impl->next_msg_mgr = sandesha2_permanent_next_msg_mgr_create(
        env, &(storage_mgr_impl->storage_mgr), ctx);
    storage_mgr_impl->seq_property_mgr = 
        sandesha2_permanent_seq_property_mgr_create(env, 
            &(storage_mgr_impl->storage_mgr), ctx);
    storage_mgr_impl->sender_mgr = sandesha2_permanent_sender_mgr_create(env, 
        &(storage_mgr_impl->storage_mgr), ctx);
    storage_mgr_impl->invoker_mgr = sandesha2_permanent_invoker_mgr_create(env, 
        &(storage_mgr_impl->storage_mgr), ctx);

    storage_mgr_impl->storage_mgr.ops = &storage_mgr_ops;

    return &(storage_mgr_impl->storage_mgr);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free_void_arg(
    void *storage_mgr,
    const axis2_env_t *env)
{
    storage_mgr = (sandesha2_permanent_storage_mgr_t *) storage_mgr;
    return sandesha2_permanent_storage_mgr_free(storage_mgr, env);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_free(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    if(storage_mgr_impl->create_seq_mgr)
    {
        sandesha2_create_seq_mgr_free(storage_mgr_impl->create_seq_mgr, env);
        storage_mgr_impl->create_seq_mgr = NULL;
    }
    if(storage_mgr_impl->next_msg_mgr)
    {
        sandesha2_next_msg_mgr_free(storage_mgr_impl->next_msg_mgr, env);
        storage_mgr_impl->next_msg_mgr = NULL;
    }
    if(storage_mgr_impl->sender_mgr)
    {
        sandesha2_sender_mgr_free(storage_mgr_impl->sender_mgr, env);
        storage_mgr_impl->sender_mgr = NULL;
    }
    if(storage_mgr_impl->seq_property_mgr)
    {
        sandesha2_seq_property_mgr_free(storage_mgr_impl->seq_property_mgr, 
            env);
        storage_mgr_impl->seq_property_mgr = NULL;
    }
    if(storage_mgr_impl->invoker_mgr)
    {
        sandesha2_invoker_mgr_free(storage_mgr_impl->invoker_mgr, env);
        storage_mgr_impl->invoker_mgr = NULL;
    }
    if(storage_mgr_impl->SANDESHA2_MSG_MAP_KEY)
    {
        AXIS2_FREE(env->allocator, storage_mgr_impl->SANDESHA2_MSG_MAP_KEY);
        storage_mgr_impl->SANDESHA2_MSG_MAP_KEY = NULL;
    }

    if(storage_mgr_impl)
    {
        AXIS2_FREE(env->allocator, storage_mgr_impl);
        storage_mgr_impl = NULL;
    }
    return AXIS2_SUCCESS;
}

sandesha2_transaction_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_transaction(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    return sandesha2_permanent_transaction_create(env, storage_mgr);
}

void AXIS2_CALL
sandesha2_permanent_storage_mgr_enlist_bean(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    sandesha2_rm_bean_t *rm_bean)
{
    sandesha2_transaction_t *t = sandesha2_permanent_storage_mgr_get_transaction(
        storage_mgr, env);
    sandesha2_transaction_enlist(t, env, rm_bean);
}

sandesha2_create_seq_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_create_seq_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->create_seq_mgr;
}

sandesha2_next_msg_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_next_msg_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->next_msg_mgr;
}

sandesha2_sender_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_retrans_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->sender_mgr;
}

sandesha2_seq_property_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_seq_property_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);

    return storage_mgr_impl->seq_property_mgr;
}

sandesha2_invoker_mgr_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_storage_map_mgr(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->invoker_mgr;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_set_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    storage_mgr_impl->conf_ctx = conf_ctx;
    return AXIS2_SUCCESS;
}

axis2_conf_ctx_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_get_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env)
{
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    return storage_mgr_impl->conf_ctx;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    
    sandesha2_permanent_storage_mgr_set_ctx(storage_mgr, env, conf_ctx);

    return AXIS2_SUCCESS;
}

axis2_msg_ctx_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_conf_ctx_t *conf_ctx)
{
    axis2_hash_t *storage_map = NULL;
    axis2_property_t *property = NULL;
    axis2_ctx_t *ctx = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    conf_ctx = sandesha2_permanent_storage_mgr_get_ctx(storage_mgr, env);
    ctx = AXIS2_CONF_CTX_GET_BASE(conf_ctx, env);
    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            storage_mgr_impl->SANDESHA2_MSG_MAP_KEY, AXIS2_FALSE);
    storage_map = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!storage_map)
        return NULL;
    return (axis2_msg_ctx_t *) axis2_hash_get(storage_map, key, 
        AXIS2_HASH_KEY_STRING);
}
		
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx)
{
    axis2_hash_t *storage_map = NULL;
    axis2_property_t *property = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_ctx_t *ctx = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
   
    conf_ctx = sandesha2_permanent_storage_mgr_get_ctx(storage_mgr, env);
    ctx = AXIS2_CONF_CTX_GET_BASE(conf_ctx, env);
    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            storage_mgr_impl->SANDESHA2_MSG_MAP_KEY, AXIS2_FALSE);
    if(!property)
    {
        property = axis2_property_create(env);
    }
    storage_map = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!storage_map)
    {
        storage_map = axis2_hash_make(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_APPLICATION);
        AXIS2_PROPERTY_SET_VALUE(property, env, storage_map);
        AXIS2_PROPERTY_SET_FREE_FUNC(property, env, axis2_hash_free_void_arg);
        AXIS2_CTX_SET_PROPERTY(ctx, env, storage_mgr_impl->SANDESHA2_MSG_MAP_KEY, 
                property, AXIS2_FALSE);
    }
    if(!key)
    {
        key = axis2_uuid_gen(env);
    }
    AXIS2_MSG_CTX_SET_KEEP_ALIVE(msg_ctx, env, AXIS2_TRUE);
    axis2_hash_set(storage_map, key, AXIS2_HASH_KEY_STRING, msg_ctx);
    return AXIS2_SUCCESS;
}
			
axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_update_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key,
    axis2_msg_ctx_t *msg_ctx)
{
    axis2_hash_t *storage_map = NULL;
    axis2_property_t *property = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_ctx_t *ctx = NULL;
    void *old_entry = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    conf_ctx = sandesha2_permanent_storage_mgr_get_ctx(storage_mgr, env);
    ctx = AXIS2_CONF_CTX_GET_BASE(conf_ctx, env);
    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            storage_mgr_impl->SANDESHA2_MSG_MAP_KEY, AXIS2_FALSE);
    if(!property)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_STORAGE_MAP_NOT_PRESENT, 
                AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    storage_map = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!storage_map)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_STORAGE_MAP_NOT_PRESENT, 
                AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    old_entry = axis2_hash_get(storage_map, key, AXIS2_HASH_KEY_STRING);
    if(!old_entry)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_ENTRY_IS_NOT_PRESENT_FOR_UPDATING, 
                AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    return sandesha2_permanent_storage_mgr_store_msg_ctx(storage_mgr, env, key, msg_ctx);
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_remove_msg_ctx(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    axis2_hash_t *storage_map = NULL;
    axis2_property_t *property = NULL;
    axis2_conf_ctx_t *conf_ctx = NULL;
    axis2_ctx_t *ctx = NULL;
    void *entry = NULL;
    sandesha2_permanent_storage_mgr_t *storage_mgr_impl = NULL;
    storage_mgr_impl = SANDESHA2_INTF_TO_IMPL(storage_mgr);
    conf_ctx = sandesha2_permanent_storage_mgr_get_ctx(storage_mgr, env);
    ctx = AXIS2_CONF_CTX_GET_BASE(conf_ctx, env);
    property = AXIS2_CTX_GET_PROPERTY(ctx, env, 
            storage_mgr_impl->SANDESHA2_MSG_MAP_KEY, AXIS2_FALSE);
    if(!property)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_STORAGE_MAP_NOT_PRESENT, 
                AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    storage_map = (axis2_hash_t *) AXIS2_PROPERTY_GET_VALUE(property, env);
    if(!storage_map)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_STORAGE_MAP_NOT_PRESENT, 
                AXIS2_FAILURE);
        return AXIS2_FAILURE;
    }
    entry = axis2_hash_get(storage_map, key, AXIS2_HASH_KEY_STRING);
    if(entry)
    {
        axis2_hash_set(storage_map, key, AXIS2_HASH_KEY_STRING, NULL);
    }
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_init_storage(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_module_desc_t *module_desc)
{
    return AXIS2_SUCCESS;
}

axiom_soap_envelope_t *AXIS2_CALL
sandesha2_permanent_storage_mgr_retrieve_soap_envelope(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axis2_char_t *key)
{
    /* TODO No real value */
    return NULL;
}

axis2_status_t AXIS2_CALL
sandesha2_permanent_storage_mgr_store_soap_envelope(
    sandesha2_storage_mgr_t *storage_mgr,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_env,
    axis2_char_t *key)
{
    /* TODO No real value */
    return AXIS2_SUCCESS;
}
