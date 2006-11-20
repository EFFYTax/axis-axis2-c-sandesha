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
#include <sandesha2_sender_worker.h>
#include <sandesha2_ack_mgr.h>
#include <sandesha2_constants.h>
#include <sandesha2_utils.h>
#include <sandesha2_transaction.h>
#include <sandesha2_storage_mgr.h>
#include <sandesha2_sender_mgr.h>
#include <sandesha2_seq_property_bean.h>
#include <sandesha2_seq_property_mgr.h>
#include <sandesha2_msg_ctx.h>
#include <sandesha2_seq.h>
#include <axis2_addr.h>
#include <axis2_engine.h>
#include <stdlib.h>
#include <axis2_http_transport.h>
#include <axis2_http_transport_utils.h>
#include <axiom_soap_const.h>
#include <axiom_soap_fault.h>
#include <axiom_soap_body.h>
#include <sandesha2_msg_init.h>
#include <sandesha2_terminate_seq.h>
#include <sandesha2_terminate_mgr.h>
#include <sandesha2_msg_retrans_adjuster.h>
#include <platforms/axis2_platform_auto_sense.h>

/** 
 * @brief Sender struct impl
 *	Sandesha2 Sender Invoker
 */
typedef struct sandesha2_sender_worker_args sandesha2_sender_worker_args_t;

struct sandesha2_sender_worker_t
{
	axis2_conf_ctx_t *conf_ctx;
    axis2_thread_mutex_t *mutex;
    int counter;
    axis2_char_t *msg_id;
    axis2_transport_out_desc_t *transport_out;
};

struct sandesha2_sender_worker_args
{
    sandesha2_sender_worker_t *impl;
    axis2_env_t *env;
};

axis2_status_t AXIS2_CALL 
sandesha2_sender_worker_run (
    sandesha2_sender_worker_t *sender_worker,
    const axis2_env_t *env);
                        
static void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_worker_func(
    axis2_thread_t *thd, 
    void *data);

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_piggybackable_msg_type(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env, 
    int msg_type);

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_ack_already_piggybacked(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx);

static axis2_status_t AXIS2_CALL
sandesha2_sender_worker_check_for_sync_res(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env, 
    axis2_msg_ctx_t *msg_ctx);

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_fault_envelope(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env, 
    axiom_soap_envelope_t *soap_envelope);

axis2_status_t AXIS2_CALL 
sandesha2_sender_worker_free(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env);								


AXIS2_EXTERN sandesha2_sender_worker_t* AXIS2_CALL
sandesha2_sender_worker_create(
    const axis2_env_t *env,
    axis2_conf_ctx_t *conf_ctx,
    axis2_char_t *msg_id)
{
    sandesha2_sender_worker_t *sender_worker = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    sender_worker =  (sandesha2_sender_worker_t *)AXIS2_MALLOC 
                        (env->allocator, 
                        sizeof(sandesha2_sender_worker_t));
	
    if(!sender_worker)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    sender_worker->conf_ctx = conf_ctx;
    sender_worker->mutex = NULL;
    sender_worker->counter = 0;
    sender_worker->msg_id = AXIS2_STRDUP(msg_id, env);
    sender_worker->transport_out = NULL;
    
    sender_worker->mutex = axis2_thread_mutex_create(env->allocator,
                        AXIS2_THREAD_MUTEX_DEFAULT);
                        
	return sender_worker;
}

axis2_status_t AXIS2_CALL
sandesha2_sender_worker_free_void_arg(
    void *sender_worker,
    const axis2_env_t *env)
{
    sandesha2_sender_worker_t *sender_worker_l = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    sender_worker_l = (sandesha2_sender_worker_t *) sender_worker;
    return sandesha2_sender_worker_free(sender_worker_l, env);
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_worker_free(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env)
{
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    /* Do not free this */
    sender_worker->conf_ctx = NULL;
    
    if(sender_worker->mutex)
    {
        axis2_thread_mutex_destroy(sender_worker->mutex);
        sender_worker->mutex = NULL;
    }
    if(sender_worker->msg_id)
    {
        AXIS2_FREE(env->allocator, sender_worker->msg_id);
        sender_worker->msg_id = NULL;
    }
	AXIS2_FREE(env->allocator, sender_worker);
	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL 
sandesha2_sender_worker_run (
    sandesha2_sender_worker_t *sender_worker,
    const axis2_env_t *env)
{
    axis2_thread_t *worker_thread = NULL;
    sandesha2_sender_worker_args_t *args = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    args = AXIS2_MALLOC(env->allocator, sizeof(sandesha2_sender_worker_args_t)); 
    args->impl = sender_worker;
    args->env = (axis2_env_t*)env;

    worker_thread = AXIS2_THREAD_POOL_GET_THREAD(env->thread_pool,
                        sandesha2_sender_worker_worker_func, (void*)args);
    if(!worker_thread)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2]Thread creation "
                        "failed sandesha2_sender_worker_run");
        return AXIS2_FAILURE;
    }
    AXIS2_THREAD_POOL_THREAD_DETACH(env->thread_pool, worker_thread); 
    printf("came1\n");        
    return AXIS2_SUCCESS;
}

/**
 * Thread worker function.
 */
static void * AXIS2_THREAD_FUNC
sandesha2_sender_worker_worker_func(
    axis2_thread_t *thd, 
    void *data)
{
    sandesha2_sender_worker_t *sender_worker = NULL;
    sandesha2_sender_worker_args_t *args;
    axis2_env_t *env = NULL;
    sandesha2_storage_mgr_t *storage_mgr = NULL;
    sandesha2_transaction_t *transaction = NULL;
    sandesha2_sender_bean_t *sender_worker_bean = NULL;
    sandesha2_sender_bean_t *bean1 = NULL;
    sandesha2_sender_mgr_t *sender_mgr = NULL;
    axis2_char_t *key = NULL;
    axis2_msg_ctx_t *msg_ctx = NULL;
    axis2_property_t *property = NULL;
    axis2_bool_t continue_sending = AXIS2_TRUE;
    axis2_char_t *qualified_for_sending = NULL;
    sandesha2_msg_ctx_t *rm_msg_ctx = NULL;
    sandesha2_property_bean_t *prop_bean = NULL;
    axis2_array_list_t *msgs_not_to_send = NULL;
    int msg_type = -1;
    axis2_transport_out_desc_t *transport_out = NULL;
    axis2_transport_sender_t *transport_sender = NULL;
    axis2_bool_t successfully_sent = AXIS2_FALSE;
    const axis2_char_t *msg_id = NULL;

    args = (sandesha2_sender_worker_args_t*)data;
    env = axis2_init_thread_env(args->env);
    sender_worker = args->impl;
    msg_id = sender_worker->msg_id;
    transport_out = sender_worker->transport_out;
    
    storage_mgr = sandesha2_utils_get_storage_mgr(env, 
                        sender_worker->conf_ctx, 
                        AXIS2_CONF_CTX_GET_CONF(sender_worker->conf_ctx, env));
                        
    transaction = sandesha2_storage_mgr_get_transaction(storage_mgr,
                    env);
    sender_mgr = sandesha2_storage_mgr_get_retrans_mgr(storage_mgr, env);
    sender_worker_bean = sandesha2_sender_mgr_retrieve(sender_mgr, env, msg_id);
    if(!sender_worker_bean)
    {
        return NULL;
    }

    key = sandesha2_sender_bean_get_msg_ctx_ref_key(sender_worker_bean, env);
    msg_ctx = sandesha2_storage_mgr_retrieve_msg_ctx(storage_mgr, env, key, 
                    sender_worker->conf_ctx);
    if(!msg_ctx)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "[sandesha2] msg_ctx is "
                    "not present in the store");
        return NULL;
    }
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                    SANDESHA2_VALUE_TRUE, env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, SANDESHA2_WITHIN_TRANSACTION,
                    property, AXIS2_FALSE);
    continue_sending = sandesha2_msg_retrans_adjuster_adjust_retrans(env,
                    sender_worker_bean, sender_worker->conf_ctx, storage_mgr);
    if(!continue_sending)
    {
        return NULL;
    }
    
    property = AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, 
                    SANDESHA2_QUALIFIED_FOR_SENDING, AXIS2_FALSE);
    if(property)
        qualified_for_sending = AXIS2_PROPERTY_GET_VALUE(property, env);
        
    if(qualified_for_sending && 0 != AXIS2_STRCMP(
                    qualified_for_sending, SANDESHA2_VALUE_TRUE))
        return NULL;
    rm_msg_ctx = sandesha2_msg_init_init_msg(env, msg_ctx);
    
    prop_bean = sandesha2_utils_get_property_bean_from_op(env, 
                    AXIS2_MSG_CTX_GET_OP(msg_ctx, env));
    if(prop_bean)
        msgs_not_to_send = sandesha2_property_bean_get_msg_types_to_drop(
                    prop_bean, env);
    if(msgs_not_to_send)
    {
        int j = 0;
        axis2_bool_t continue_sending = AXIS2_FALSE;

        for(j = 0; j < AXIS2_ARRAY_LIST_SIZE(msgs_not_to_send, env); j++)
        {
            axis2_char_t *value = NULL;
            int int_val = -1;
            int msg_type = -1;
            
            value = AXIS2_ARRAY_LIST_GET(msgs_not_to_send, env, j);
            int_val = atoi(value);
            msg_type = sandesha2_msg_ctx_get_msg_type(rm_msg_ctx, env);
            if(msg_type == int_val)
                continue_sending = AXIS2_TRUE;
        }
        if(continue_sending)
            return NULL;
    }
    /* 
     *   This method is not implemented yet
     *  update_msg(sender_worker, env, msg_xtx);
     */
    msg_type = sandesha2_msg_ctx_get_msg_type(rm_msg_ctx, env);
    if(msg_type == SANDESHA2_MSG_TYPE_APPLICATION)
    {
        sandesha2_seq_t *seq = NULL;
        axis2_char_t *seq_id = NULL;
        sandesha2_identifier_t *identifier = NULL;
        
        seq = (sandesha2_seq_t*)
                    sandesha2_msg_ctx_get_msg_part(rm_msg_ctx, 
                    env, SANDESHA2_MSG_PART_SEQ);
        identifier = sandesha2_seq_get_identifier(seq, env);
        seq_id = sandesha2_identifier_get_identifier(identifier, env);
    }
    if(sandesha2_sender_worker_is_piggybackable_msg_type(sender_worker, env,
                    msg_type) && AXIS2_FALSE  == 
                    sandesha2_sender_worker_is_ack_already_piggybacked(sender_worker, env,
                    rm_msg_ctx))
    {
        sandesha2_ack_mgr_piggyback_acks_if_present(env, rm_msg_ctx, 
                    storage_mgr);
    }
    
    
    transport_out = AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx, env);
    transport_sender = AXIS2_TRANSPORT_OUT_DESC_GET_SENDER(transport_out, env);
    if(transport_sender)
    {
        SANDESHA2_TRANSACTION_COMMIT(transaction, env);
        property = axis2_property_create(env);
        AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
        AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                    SANDESHA2_VALUE_FALSE, env));
        AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                    SANDESHA2_WITHIN_TRANSACTION, property, AXIS2_FALSE);
        /* Consider building soap envelope */
        AXIS2_TRANSPORT_SENDER_INVOKE(transport_sender, env, msg_ctx);
        successfully_sent = AXIS2_TRUE;
        sender_worker->counter++;
        /*printf("**********************counter******************:%d\n", sender_worker->counter);
        if(2 == sender_worker->counter)
        sleep(300000);*/
                    
    }
    transaction = sandesha2_storage_mgr_get_transaction(storage_mgr,
                    env);
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                    SANDESHA2_VALUE_TRUE, env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                    SANDESHA2_WITHIN_TRANSACTION, property, AXIS2_FALSE);
    msg_id = sandesha2_sender_bean_get_msg_id(sender_worker_bean, env);
    bean1 = sandesha2_sender_mgr_retrieve(sender_mgr, env, msg_id);
    if(bean1)
    {
        axis2_bool_t resend = AXIS2_FALSE;
        
        resend = sandesha2_sender_bean_is_resend(sender_worker_bean, env);
        if(resend)
        {
            sandesha2_sender_bean_set_sent_count(bean1, env, 
                    sandesha2_sender_bean_get_sent_count(sender_worker_bean, env));
            sandesha2_sender_bean_set_time_to_send(bean1, env, 
                    sandesha2_sender_bean_get_time_to_send(sender_worker_bean, env));
            sandesha2_sender_mgr_update(sender_mgr, env, bean1);
        }
        else
        {
            axis2_char_t *msg_stored_key = NULL;
            
            msg_id = sandesha2_sender_bean_get_msg_id(bean1, env); 
            sandesha2_sender_mgr_remove(sender_mgr, env, msg_id);
            /* Removing the message from the storage */
            msg_stored_key = sandesha2_sender_bean_get_msg_ctx_ref_key(
                bean1, env);
            sandesha2_storage_mgr_remove_msg_ctx(storage_mgr, env, 
                msg_stored_key);
        }
    }
    if(successfully_sent)
    {
        if(AXIS2_FALSE == AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env))
            sandesha2_sender_worker_check_for_sync_res(sender_worker, env, msg_ctx);
    }
    if(SANDESHA2_MSG_TYPE_TERMINATE_SEQ == sandesha2_msg_ctx_get_msg_type(
                    rm_msg_ctx, env))
    {
        sandesha2_terminate_seq_t *terminate_seq = NULL;
        axis2_char_t *seq_id = NULL;
        axis2_conf_ctx_t *conf_ctx = NULL;
        axis2_char_t *int_seq_id = NULL;
        
        terminate_seq = (sandesha2_terminate_seq_t*)
                    sandesha2_msg_ctx_get_msg_part(rm_msg_ctx, env, 
                    SANDESHA2_MSG_PART_TERMINATE_SEQ);
        seq_id = sandesha2_identifier_get_identifier(
                    sandesha2_terminate_seq_get_identifier(terminate_seq, 
                    env), env);
        conf_ctx = AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, env);
        int_seq_id = sandesha2_utils_get_seq_property(env, seq_id, 
                    SANDESHA2_SEQ_PROP_INTERNAL_SEQ_ID, storage_mgr);
        sandesha2_terminate_mgr_terminate_sending_side(env, conf_ctx,
                    int_seq_id, AXIS2_MSG_CTX_GET_SERVER_SIDE(msg_ctx, env), 
                    storage_mgr);
    }
    property = axis2_property_create(env);
    AXIS2_PROPERTY_SET_SCOPE(property, env, AXIS2_SCOPE_REQUEST);
    AXIS2_PROPERTY_SET_VALUE(property, env, AXIS2_STRDUP(
                    SANDESHA2_VALUE_FALSE, env));
    AXIS2_MSG_CTX_SET_PROPERTY(msg_ctx, env, 
                    SANDESHA2_WITHIN_TRANSACTION, property, AXIS2_FALSE);
    /* TODO make transaction handling effective */
    if(transaction)
    {
        SANDESHA2_TRANSACTION_COMMIT(transaction, env);
    }
    #ifdef AXIS2_SVR_MULTI_THREADED
        AXIS2_THREAD_POOL_EXIT_THREAD(env->thread_pool, thd);
    #endif
    return NULL;
}

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_piggybackable_msg_type(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env, 
    int msg_type)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(SANDESHA2_MSG_TYPE_ACK == msg_type)
        return AXIS2_FALSE;
    
    return AXIS2_TRUE;
}

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_ack_already_piggybacked(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env, 
    sandesha2_msg_ctx_t *rm_msg_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, rm_msg_ctx, AXIS2_FAILURE);
    
    if(sandesha2_msg_ctx_get_msg_part(rm_msg_ctx, env, 
                        SANDESHA2_MSG_PART_SEQ_ACKNOWLEDGEMENT))
        return AXIS2_TRUE;
    
    return AXIS2_FALSE;
}

static axis2_status_t AXIS2_CALL
sandesha2_sender_worker_check_for_sync_res(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env, 
    axis2_msg_ctx_t *msg_ctx)
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
    if(!property)
        return AXIS2_SUCCESS;
        
    res_msg_ctx = axis2_msg_ctx_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx,
                    env), AXIS2_MSG_CTX_GET_TRANSPORT_IN_DESC(
                    msg_ctx, env), AXIS2_MSG_CTX_GET_TRANSPORT_OUT_DESC(msg_ctx,
                    env));
    AXIS2_MSG_CTX_SET_SERVER_SIDE(res_msg_ctx, env, AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, AXIS2_TRANSPORT_IN,
                    AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env, AXIS2_TRANSPORT_IN,
                    AXIS2_FALSE), AXIS2_FALSE);
    AXIS2_MSG_CTX_SET_SVC_CTX(res_msg_ctx, env, AXIS2_MSG_CTX_GET_SVC_CTX(
                    msg_ctx, env));
    AXIS2_MSG_CTX_SET_SVC_GRP_CTX(res_msg_ctx, env, 
                    AXIS2_MSG_CTX_GET_SVC_GRP_CTX(msg_ctx, env));
    req_op_ctx = AXIS2_MSG_CTX_GET_OP_CTX(msg_ctx, env);
    if(req_op_ctx)
    {
        axis2_ctx_t *ctx = NULL;
        
        ctx = AXIS2_OP_CTX_GET_BASE(req_op_ctx, env);
        if(AXIS2_CTX_GET_PROPERTY(ctx, env, MTOM_RECIVED_CONTENT_TYPE, 
                    AXIS2_FALSE))
        {
            AXIS2_MSG_CTX_SET_PROPERTY(res_msg_ctx, env, 
                    MTOM_RECIVED_CONTENT_TYPE, AXIS2_CTX_GET_PROPERTY(ctx, env, 
                    MTOM_RECIVED_CONTENT_TYPE, AXIS2_FALSE), AXIS2_FALSE);
        }
        if(AXIS2_CTX_GET_PROPERTY(ctx, env, AXIS2_HTTP_CHAR_SET_ENCODING, 
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
    if(res_envelope)
    {
        axis2_engine_t *engine = NULL;
        AXIS2_MSG_CTX_SET_SOAP_ENVELOPE(res_msg_ctx, env, res_envelope);
        
        engine = axis2_engine_create(env, AXIS2_MSG_CTX_GET_CONF_CTX(msg_ctx, 
                    env));
        if(AXIS2_TRUE == sandesha2_sender_worker_is_fault_envelope(sender_worker, env, 
                    res_envelope))
            AXIS2_ENGINE_RECEIVE_FAULT(engine, env, res_msg_ctx);
        else
            AXIS2_ENGINE_RECEIVE(engine, env, res_msg_ctx);        
    }
    return AXIS2_SUCCESS;
}

static axis2_bool_t AXIS2_CALL
sandesha2_sender_worker_is_fault_envelope(
    sandesha2_sender_worker_t *sender_worker, 
    const axis2_env_t *env, 
    axiom_soap_envelope_t *soap_envelope)
{
    axiom_soap_fault_t *fault = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, soap_envelope, AXIS2_FAILURE);
    
    fault = AXIOM_SOAP_BODY_GET_FAULT(AXIOM_SOAP_ENVELOPE_GET_BODY(soap_envelope,
                        env), env);
    if(fault)
        return AXIS2_TRUE;
        
    return AXIS2_FALSE;
}

void sandesha2_sender_worker_set_transport_out(
    sandesha2_sender_worker_t *sender_worker,
    const axis2_env_t *env,
    axis2_transport_out_desc_t *transport_out)
{
    sender_worker->transport_out = transport_out;
}



