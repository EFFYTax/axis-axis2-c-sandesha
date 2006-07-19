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
#include <sandesha2_sender.h>
#include <sandesha2/sandesha2_constants.h>
#include <sandesha2/sandesha2_utils.h>
#include <sandesha2_transaction.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2/sandesha2_msg_ctx.h>
#include <sandesha2/sandesha2_seq.h>
#include <axis2_addr.h>
#include <axis2_engine.h>
#include <stdio.h>
#include <axis2_http_transport.h>
#include <axis2_http_transport_utils.h>
#include <axiom_soap_const.h>
#include <axiom_soap_fault.h>
#include <axiom_soap_body.h>


/** 
 * @brief Sender struct impl
 *	Sandesha2 Sender Invoker
 */
typedef struct sandesha2_sender_impl sandesha2_sender_impl_t;  
typedef struct sandesha2_sender_args sandesha2_sender_args_t;

struct sandesha2_sender_impl
{
    sandesha2_sender_t sender;
	axis2_conf_ctx_t *conf_ctx;
    axis2_bool_t run_sender;
    axis2_array_list_t *working_seqs;
    axis2_thread_mutex_t *mutex;
};

struct sandesha2_sender_args
{
    sandesha2_sender_impl_t *impl;
    axis2_env_t *env;
};

#define SANDESHA2_INTF_TO_IMPL(sender) \
                        ((sandesha2_sender_impl_t *)(sender))

/***************************** Function headers *******************************/
axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sender_for_seq
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env, axis2_char_t *seq_id);
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sending (sandesha2_sender_t *sender,
                        const axis2_env_t *env);
            
axis2_bool_t AXIS2_CALL 
sandesha2_sender_is_sender_started 
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env);
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run_sender_for_seq 
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        axis2_conf_ctx_t *conf_ctx, 
                        axis2_char_t *seq_id);
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run (sandesha2_sender_t *sender,
                        const axis2_env_t *env);
                        
axis2_status_t AXIS2_CALL
sandesha2_sender_check_for_sync_res(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        axis2_msg_ctx_t *msg_ctx);
                        
axis2_bool_t AXIS2_CALL
sandesha2_sender_is_ack_already_piggybacked(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx);
axis2_bool_t AXIS2_CALL
sandesha2_sender_is_fault_envelope(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        axiom_soap_envelope_t *soap_envelope);
                        

void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_func(axis2_thread_t *thd, void *data);

axis2_status_t AXIS2_CALL 
sandesha2_sender_free(sandesha2_sender_t *sender, 
                        const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_sender_t* AXIS2_CALL
sandesha2_sender_create(const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    sender_impl =  (sandesha2_sender_impl_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_sender_impl_t));
	
    if(NULL == sender_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sender_impl->conf_ctx = NULL;
    sender_impl->run_sender = AXIS2_FALSE;
    sender_impl->working_seqs = NULL;
    sender_impl->mutex = NULL;
    sender_impl->sender.ops = NULL;
    
    sender_impl->sender.ops = AXIS2_MALLOC(env->allocator,
                        sizeof(sandesha2_sender_ops_t));
    if(NULL == sender_impl->sender.ops)
	{
		sandesha2_sender_free(
                        (sandesha2_sender_t*)sender_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    sender_impl->working_seqs = axis2_array_list_create(env, 
                        AXIS2_ARRAY_LIST_DEFAULT_CAPACITY);
    sender_impl->mutex = axis2_thread_mutex_create(env->allocator,
                        AXIS2_THREAD_MUTEX_DEFAULT);
                        
    sender_impl->sender.ops->stop_sender_for_seq = 
                        sandesha2_sender_stop_sender_for_seq;
    sender_impl->sender.ops->stop_sending = 
                        sandesha2_sender_stop_sending;
    sender_impl->sender.ops->is_sender_started = 
                        sandesha2_sender_is_sender_started;
    sender_impl->sender.ops->run_sender_for_seq = 
                        sandesha2_sender_run_sender_for_seq;
    sender_impl->sender.ops->run = sandesha2_sender_run;
    sender_impl->sender.ops->free = sandesha2_sender_free;
                        
	return &(sender_impl->sender);
}


axis2_status_t AXIS2_CALL 
sandesha2_sender_free(sandesha2_sender_t *sender, 
                        const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    /* Do not free this */
    sender_impl->conf_ctx = NULL;
    
    if(NULL != sender_impl->mutex)
    {
        axis2_thread_mutex_destroy(sender_impl->mutex);
        sender_impl->mutex = NULL;
    }
    if(NULL != sender_impl->working_seqs)
    {
        AXIS2_ARRY_LIST_FREE(sender_impl->working_seqs, env);
        sender_impl->working_seqs = NULL;
    }
    if(NULL != sender->ops)
    {
        AXIS2_FREE(env->allocator, sender->ops);
        sender->ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(sender));
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sender_for_seq
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env, axis2_char_t *seq_id)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    int i = 0;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    axis2_thread_mutex_lock(sender_impl->mutex);
    for(i = 0; i < AXIS2_ARRY_LIST_SIZE(sender_impl->working_seqs, env); i++)
    {
        axis2_char_t *tmp_id = NULL;
        tmp_id = AXIS2_ARRAY_LIST_GET(sender_impl->working_seqs, env, i);
        if(0 == AXIS2_STRCMP(seq_id, tmp_id))
        {
            AXIS2_ARRAY_LIST_REMOVE(sender_impl->working_seqs, env, i);
            break;
        }
    }
    if(0 == AXIS2_ARRY_LIST_SIZE(sender_impl->working_seqs, env))
        sender_impl->run_sender = AXIS2_FALSE;
    axis2_thread_mutex_unlock(sender_impl->mutex);
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_stop_sending (sandesha2_sender_t *sender,
                        const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    axis2_thread_mutex_lock(sender_impl->mutex);
    SANDESHA2_INTF_TO_IMPL(sender)->run_sender = AXIS2_FALSE;
    axis2_thread_mutex_unlock(sender_impl->mutex);
    return AXIS2_SUCCESS;
}
            
axis2_bool_t AXIS2_CALL 
sandesha2_sender_is_sender_started 
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env)
{
    axis2_bool_t started = AXIS2_FALSE;
    sandesha2_sender_impl_t *sender_impl = NULL;
     
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    axis2_thread_mutex_lock(sender_impl->mutex);
    started = SANDESHA2_INTF_TO_IMPL(sender)->run_sender;
    axis2_thread_mutex_unlock(sender_impl->mutex);
    return started;    
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run_sender_for_seq 
                        (sandesha2_sender_t *sender, 
                        const axis2_env_t *env, axis2_conf_ctx_t *conf_ctx, 
                        axis2_char_t *seq_id)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf_ctx, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, seq_id, AXIS2_FAILURE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    axis2_thread_mutex_lock(sender_impl->mutex);
    if(AXIS2_FALSE == sandesha2_utils_array_list_contains(env, 
                        sender_impl->working_seqs, seq_id))
        AXIS2_ARRY_LIST_ADD(sender_impl->working_seqs, env, seq_id);
    if(AXIS2_FALSE == sender_impl->run_sender)
    {
        sender_impl->conf_ctx = conf_ctx;
        sender_impl->run_sender = AXIS2_TRUE;
        sandesha2_sender_run(sender, env);
    }
    axis2_thread_mutex_unlock(sender_impl->mutex);
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL 
sandesha2_sender_run (sandesha2_sender_t *sender,
                        const axis2_env_t *env)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    axis2_thread_t *worker_thread = NULL;
    sandesha2_sender_args_t args;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    sender_impl = SANDESHA2_INTF_TO_IMPL(sender);
    
    args.impl = sender_impl;
    args.env = (axis2_env_t*)env;
    worker_thread = AXIS2_THREAD_POOL_GET_THREAD(env->thread_pool,
                        sandesha2_sender_worker_func, (void*)&args);
    if(NULL == worker_thread)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Thread creation "
                        "failed sandesha2_sender_run");
        return AXIS2_FAILURE;
    }
    AXIS2_THREAD_POOL_THREAD_DETACH(env->thread_pool, worker_thread); 
        
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_sender_check_for_sync_res(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, axis2_msg_ctx_t *msg_ctx)
{
    axis2_property_t *property = NULL;
    axis2_msg_ctx_t *res_msg_ctx = NULL;
    axis2_op_ctx_t *req_op_ctx = NULL;
    axiom_soap_envelope_t *res_envelope = NULL;
    axis2_char_t *soap_ns_uri = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);
    
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, AXIS2_TRANSPORT_IN,
                    AXIS2_FALSE);
    if(NULL == property)
        return AXIS2_SUCCESS;
        
    res_msg_ctx = axis2_msg_ctx_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx,
                    env), AXIS2_MSG_CTX_GET_TRANSPORT_IN_DESC(
                    msg_ctx, env), AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx,
                    env));
    AXIS2_MSG_CTX_SET_SVR_SIDE(res_msg_ctx, env, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, AXIS2_TRANSPORT_IN,
                    AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, AXIS2_TRANSPORT_IN,
                    AXIS2_FALSE), AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_SVC_CTX(res_msg_ctx, env, AXIS2_MSG_CTX_GET_SVC_CTX(
                    msg_ctx, env));
    AXIS2_MSG_CTX_SET_SVC_GRP_CTX(res_msg_ctx, env, 
                    AXIS2_MSG_CTX_GET_SVC_GRP_CTX(msg_ctx, env));
    req_op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env);
    if(NULL != req_op_ctx)
    {
        axis2_ctx_t *ctx = NULL;
        ctx = AXIS2_OP_CTX_GET_BASE(req_op_ctx, env);
        if(NULL != AXIS2_CTX_GET_PROPERTY(ctx, env, MTOM_RECIVED_CONTENT_TYPE, 
                    AXIS2_FALSE))
        {
            AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, 
                    MTOM_RECIVED_CONTENT_TYPE, AXIS2_CTX_GET_PROPERTY(ctx, env, 
                    MTOM_RECIVED_CONTENT_TYPE, AXIS2_FALSE), AXIS2_FALSE);
        }
        if(NULL != AXIS2_CTX_GET_PROPERTY(ctx, env, AXIS2_HTTP_CHAR_SET_ENCODING, 
                    AXIS2_FALSE))
        {
            AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, 
                    AXIS2_HTTP_CHAR_SET_ENCODING, AXIS2_CTX_GET_PROPERTY(ctx, env, 
                    AXIS2_HTTP_CHAR_SET_ENCODING, AXIS2_FALSE), AXIS2_FALSE);
        }
    }
    AXIS2_MSG_CTX_SET_DOING_REST(res_msg_ctx, env, AXIS2_MSG_CTX_GET_DOING_REST(
                    msg_ctx, env));
    soap_ns_uri = AXIS2_MSG_CTX_GET_IS_SOAP_11(msg_ctx, env) ?
                    AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI:
                    AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI;

    res_envelope = axis2_http_transport_utils_create_soap_msg(env, msg_ctx,
                    soap_ns_uri);
   
    AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, SANDESHA2_WITHIN_TRANSACTION,
                    AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env,
                    SANDESHA2_WITHIN_TRANSACTION, AXIS2_FALSE), AXIS2_FALSE);
    if(NULL != res_envelope)
    {
        axis2_engine_t *engine = NULL;
        AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(res_msg_ctx, env, res_envelope);
        
        engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, 
                    env));
        if(AXIS2_FALSE == sandesha2_sender_is_fault_envelope(sender, env, 
                    res_envelope))
            AXIS2_ENGINE_RECIEVE_FAULT(engine, env, res_msg_ctx);
        else
            AXIS2_ENGINE_RECIEVE(engine, env, res_msg_ctx);        
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
sandesha2_sender_is_piggybackable_msg_type(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        int msg_type)
{
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(SANDESHA2_MSG_TYPE_ACK == msg_type)
        return AXIS2_FALSE;
    
    return AXIS2_TRUE;
}

axis2_bool_t AXIS2_CALL
sandesha2_sender_is_ack_already_piggybacked(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        sandesha2_msg_ctx_t *rm_msg_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    if(NULL != SANDESHA2_MSG_CTX_GET_MSG_PART(rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT))
        return AXIS2_TRUE;
    
    return AXIS2_FALSE;
}

axis2_bool_t AXIS2_CALL
sandesha2_sender_is_fault_envelope(
                        sandesha2_sender_t *sender, 
                        const axis2_env_t *env, 
                        axiom_soap_envelope_t *soap_envelope)
{
    axiom_soap_fault_t *fault = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    
    fault = AXIOM_SOAP_BODY_GET_FAULT(AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope,
                        env), env);
    if(NULL != fault)
        return AXIS2_TRUE;
        
    return AXIS2_FALSE;
}

/**
 * Thread worker function.
 */
void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_func(axis2_thread_t *thd, void *data)
{
    sandesha2_sender_impl_t *sender_impl = NULL;
    sandesha2_sender_t *sender = NULL;
    sandesha2_sender_args_t *args;
    axis2_env_t *env = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    
    args = (sandesha2_sender_args_t*)data;
    env = args->env;
    sender_impl = args->impl;
    sender = (sandesha2_sender_t*)sender_impl;
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, 
                        sender_impl->conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(sender_impl->conf_ctx, env));
                        
    while(AXIS2_TRUE == sender_impl->run_sender)
    {
        sandesha2_transaction_t *transaction = NULL;
        /* Use when transaction handling is done 
        axis2_bool_t rollbacked = AXIS2_FALSE;*/
        sandesha2_next_msg_mgr_t *next_msg_mgr = NULL;
        sandesha2_sender_mgr_t *storage_map_mgr = NULL;
        sandesha2_seq_property_mgr_t *seq_prop_mgr = NULL;
        sandesha2_seq_property_bean_t *all_seq_bean = NULL;
        axis2_array_list_t *all_seq_list = NULL;
        int i = 0;
        
        
        next_msg_mgr = SANDESHA2_STORAGE_MGR_GET_NEXT_MSG_MGR(
                        storage_mgr, env);
        storage_map_mgr = SANDESHA2_STORAGE_MGR_GET_STORAGE_MAP_MGR
                        (storage_mgr, env);
        seq_prop_mgr = SANDESHA2_STORAGE_MGR_GET_SEQ_PROPERTY_MGR(
                        storage_mgr, env);
        transaction = SANDESHA2_STORAGE_MGR_GET_TRANSACTION(storage_mgr,
                        env);
        all_seq_bean = SANDESHA2_SEQ_PROPERTY_MGR_RETRIEVE(seq_prop_mgr,
                        env, SANDESHA2_SEQ_PROP_ALL_SEQS, 
                        SANDESHA2_SEQ_PROP_INCOMING_SEQ_LIST);
        if(NULL == all_seq_bean)
            continue;
        all_seq_list = sandesha2_utils_get_array_list_from_string(env,
                        SANDESHA2_SEQ_PROPERTY_BEAN_GET_VALUE(all_seq_bean, env));
        if(NULL == all_seq_list)
            continue;
            
        for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(all_seq_list, env); i++)
        {
            axis2_char_t *seq_id = NULL;
            long next_msg_no = -1;
            sandesha2_next_msg_bean_t *next_msg_bean = NULL;
            axis2_array_list_t *st_map_list = NULL;
            sandesha2_sender_bean_t *find_bean = NULL;
            axis2_bool_t invoked = AXIS2_FALSE;
            int j = 0;
            axis2_bool_t continue_seq = AXIS2_TRUE;
            
            seq_id = AXIS2_ARRAY_LIST_GET(all_seq_list, env, i);
            SANDESHA2_TRANSACTION_COMMIT(transaction, env);
            transaction = SANDESHA2_STORAGE_MGR_GET_TRANSACTION(
                        storage_mgr, env);
            next_msg_bean = SANDESHA2_NEXT_MSG_MGR_RETRIEVE(
                        next_msg_mgr, env, seq_id);
            if(NULL == next_msg_bean)
            {
                axis2_char_t *str_list = NULL;
                AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Next message not set" 
                        " correctly. Removing invalid entry.");
                AXIS2_ARRAY_LIST_REMOVE(all_seq_list, env, i);
                /* We need to make sure we are not skipping element after 
                 * removing current element
                 */                 
                i--;
                str_list = sandesha2_utils_array_list_to_string(env, all_seq_list, 
                        SANDESHA2_ARRAY_LIST_STRING);
                SANDESHA2_SEQ_PROPERTY_BEAN_SET_VALUE(all_seq_bean, env,
                        str_list);
                SANDESHA2_SEQ_PROPERTY_MGR_UPDATE(seq_prop_mgr, env, 
                        all_seq_bean);
                continue;
            }
            next_msg_no = SANDESHA2_NEXT_MSG_BEAN_GET_NEXT_MSG_NO_TO_PROCESS(
                        next_msg_bean, env);
            if(next_msg_no < 0)
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Invalid message number"
                        " as the Next Message Number.");
                return data;
            }
            /*find_bean = sandesha2_sender_bean_create_with_data(env, NULL,
                        next_msg_no, seq_id, AXIS2_FALSE);*/
            st_map_list = SANDESHA2_INVOKER_MGR_FIND(storage_map_mgr,
                        env, find_bean);
            for(j = 0; j < AXIS2_ARRAY_LIST_SIZE(st_map_list, env); j++)
            {
                sandesha2_sender_bean_t *st_map_bean = NULL;
                axis2_char_t *key = NULL;
                axis2_msg_ctx_t *msg_to_invoke = NULL;
                sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
                axis2_property_t *property = NULL;
                axis2_bool_t post_failure_invocation = AXIS2_FALSE;
                axis2_char_t *post_failure_str = NULL;
                axis2_msg_ctx_t *msg_ctx = NULL;
                axis2_engine_t *engine = NULL;
                
                st_map_bean = AXIS2_ARRAY_LIST_GET(st_map_list, env, j);
                key = SANDESHA2_INVOKER_BEAN_GET_MSG_CONTEXT_REF_KEY(st_map_bean,
                        env);
                msg_to_invoke = SANDESHA2_STORAGE_MGR_RETRIEVE_MSG_CTX(
                        storage_mgr, env, key, sender_impl->conf_ctx);
                rm_msg_ctx = sandesha2_msg_initilizer_init_msg(env, 
                        msg_to_invoke);
                /* have to commit the transaction before invoking. This may get 
                 * changed when WS-AT is available.
                 */
                SANDESHA2_TRANSACTION_COMMIT(transaction, env);
                property = axis2_property_create(env);
                AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
                AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                        SANDESHA2_VALUE_TRUE, env));
                AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                        SANDESHA2_WITHIN_TRANSACTION, property, AXIS2_FALSE);
                        
                property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env,
                        SANDESHA2_POST_FAILURE_MESSAGE, AXIS2_FALSE);
                if(NULL != property)
                    post_failure_str = AXIS2_PROPERTY_GET_VALUE(property, env);
                if(NULL != post_failure_str && 0 == AXIS2_STRCMP(
                        post_failure_str, SANDESHA2_VALUE_TRUE))
                    post_failure_invocation = AXIS2_TRUE;
                engine = axis2_engine_create(env, sender_impl->conf_ctx);
                if(AXIS2_TRUE == post_failure_invocation)
                {
                    sandesha2_sender_make_msg_ready_for_reinjection(
                        sender, env, msg_to_invoke);
                    AXIS2_ENGINE_RECIEVE(engine, env, msg_to_invoke);
                }
                else
                    AXIS2_ENGINE_RESUME_RECIEVE(engine, env, msg_to_invoke);
                invoked = AXIS2_TRUE;
                transaction = SANDESHA2_STORAGE_MGR_GET_TRANSACTION(
                        storage_mgr, env);
                SANDESHA2_STORAGE_MGR_DELETE(storage_mgr, env, key);
                msg_ctx = SANDESHA2_STORAGE_MGR_RETRIEVE_MSG_CTX(
                        storage_mgr, env, key, sender_impl->conf_ctx);
                if(NULL != msg_ctx)
                    SANDESHA2_STORAGE_MGR_REMOVE_MSG_CTX(storage_mgr,
                        env, key);
                if(SANDESHA2_MSG_TYPE_APPLICATION == 
                        SANDESHA2_MSG_CTX_GET_MSG_TYPE(rm_msg_ctx, env))
                {
                    sandesha2_seq_t *seq = NULL;
                    seq = (sandesha2_seq_t*)_MSG_CTX_GET_MSG_PART(
                            rm_msg_ctx, env, SANDESHA2_MSG_PART_SEQ);
                    if(NULL != SANDESHA2_SEQ_GET_LAST_MSG(seq, env))
                    {
                        sandesha2_terminate_mgr_clean_recv_side_after_invocation(
                            env, sender_impl->conf_ctx, seq_id, 
                            storage_mgr);
                        /* we are done with current seq */
                        continue_seq = AXIS2_FALSE;
                        break;
                    }
                }
            }
            if(AXIS2_FALSE == continue_seq)
                break;
            if(AXIS2_TRUE == invoked)
            {
                next_msg_no++;
                SANDESHA2_NEXT_MSG_BEAN_SET_NEXT_MSG_NO_TO_PROCESS(next_msg_bean,
                        env, next_msg_no);
                SANDESHA2_NEXT_MSG_BEAN_MGR_UPDATE(next_msg_mgr, env, 
                        next_msg_bean);
            }
        }
        SANDESHA2_TRANSACTION_COMMIT(transaction, env);
        
        /* TODO make transaction handling effective */
    }
    return NULL;
}
