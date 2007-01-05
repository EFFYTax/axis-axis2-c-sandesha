/*
 * Copyright 2004,2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sandesha2_msg_store_bean.h>
#include <axis2_string.h>
#include <axis2_utils.h>

/**
 * Defines a set of properties which extracted from a Message Context
 * when serializing it.
 */
struct sandesha2_msg_store_bean_t 
{

	
	axis2_char_t * stored_key;
	
    axis2_char_t * msg_id;
	
    axis2_char_t * soap_env_str;
	
	int soap_version;

	axis2_char_t * transport_out;
	
    axis2_char_t * op;
	
    axis2_char_t * svc;
	
	axis2_char_t * svc_grp;
	
	axis2_char_t * op_mep;
	
	axis2_char_t * to_url;
	
    axis2_char_t * reply_to;
	
	axis2_char_t * transport_to;
	
	axis2_char_t * execution_chain_str;

	int flow;
	
	axis2_char_t * msg_recv_str;
	
	axis2_bool_t svr_side;
	
	axis2_char_t * in_msg_store_key;
		
	axis2_char_t * persistent_property_str;
	
	axis2_char_t * action;
};
	
AXIS2_EXTERN sandesha2_msg_store_bean_t* AXIS2_CALL
sandesha2_msg_store_bean_create(const axis2_env_t *env)
{
	sandesha2_msg_store_bean_t *msg_store_bean = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	msg_store_bean = (sandesha2_msg_store_bean_t*) AXIS2_MALLOC(env->allocator,
	    sizeof(sandesha2_msg_store_bean_t));

	if(!msg_store_bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}
    msg_store_bean->stored_key = NULL;
    msg_store_bean->msg_id = NULL;
    msg_store_bean->soap_env_str = NULL;
	msg_store_bean->soap_version = -1;
	msg_store_bean->transport_out = NULL;
    msg_store_bean->op = NULL;
    msg_store_bean->svc = NULL;
	msg_store_bean->svc_grp = NULL;
	msg_store_bean->op_mep = NULL;
	msg_store_bean->to_url = NULL;
	msg_store_bean->reply_to = NULL;
	msg_store_bean->transport_to = NULL;
	msg_store_bean->execution_chain_str = NULL;
	msg_store_bean->flow = -1;
	msg_store_bean->msg_recv_str = NULL;
	msg_store_bean->svr_side = -1;
	msg_store_bean->in_msg_store_key = NULL;
	msg_store_bean->persistent_property_str = NULL;
	msg_store_bean->action = NULL;

    return msg_store_bean;
}

void AXIS2_CALL
sandesha2_msg_store_bean_free (
    sandesha2_msg_store_bean_t *msg_store_bean,
	const axis2_env_t *env)
{
    if(msg_store_bean)
    {
        AXIS2_FREE(env->allocator, msg_store_bean);
        msg_store_bean = NULL;
    }
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_msg_id(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->msg_id;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_msg_id(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * msg_id) 
{
    msg_store_bean->msg_id = AXIS2_STRDUP(msg_id, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_stored_key(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->stored_key;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_stored_key(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * key) 
{
    msg_store_bean->stored_key = AXIS2_STRDUP(key, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_soap_envelope_str(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->soap_env_str;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_soap_envelope_str(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * soap_env_str) 
{
    msg_store_bean->soap_env_str = AXIS2_STRDUP(soap_env_str, env);
}

int AXIS2_CALL 
sandesha2_msg_store_bean_get_soap_version( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->soap_version;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_soap_version(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    int soap_version) 
{		
    msg_store_bean->soap_version = soap_version;
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_transport_out(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->transport_out;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_transport_out(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * transport_sender) 
{
    msg_store_bean->transport_out = AXIS2_STRDUP(transport_sender, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_op( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->op;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_op(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * op) 
{
    msg_store_bean->op = AXIS2_STRDUP(op, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_svc( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->svc;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_svc(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * svc) 
{
    msg_store_bean->svc = AXIS2_STRDUP(svc, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_svc_grp( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->svc_grp;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_svc_grp(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * svc_grp) 
{
    msg_store_bean->svc_grp = AXIS2_STRDUP(svc_grp, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_op_mep( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->op_mep;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_op_mep(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * opAdd) 
{
    msg_store_bean->op_mep = AXIS2_STRDUP(opAdd, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_to_url( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->to_url;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_to_url(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * to_url) 
{
    msg_store_bean->to_url = AXIS2_STRDUP(to_url, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_reply_to( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->reply_to;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_reply_to(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * reply_to) 
{
    msg_store_bean->reply_to = AXIS2_STRDUP(reply_to, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_transport_to( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->transport_to;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_transport_to(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * transport_to) 
{
    msg_store_bean->transport_to = AXIS2_STRDUP(transport_to, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_execution_chain_str( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->execution_chain_str;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_execution_chain_str(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * execution_chain_str) 
{
    msg_store_bean->execution_chain_str = AXIS2_STRDUP(execution_chain_str,        env);
}

int AXIS2_CALL 
sandesha2_msg_store_bean_get_flow( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->flow;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_flow(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    int flow) 
{
    msg_store_bean->flow = flow;
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_msg_recv_str( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->msg_recv_str;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_msg_recv_str(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * msg_recv_str) 
{
    msg_store_bean->msg_recv_str = AXIS2_STRDUP(msg_recv_str, env);
}

axis2_bool_t AXIS2_CALL 
sandesha2_msg_store_bean_is_svr_side( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->svr_side;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_svr_side(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_bool_t svr_side) 
{
    msg_store_bean->svr_side = svr_side;
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_in_msg_store_key(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->in_msg_store_key;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_in_msg_store_key(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * request_msg_key) 
{
    msg_store_bean->in_msg_store_key = AXIS2_STRDUP(request_msg_key, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_persistent_property_str( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->persistent_property_str;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_persistent_property_str(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * persistent_property_str) 
{
    msg_store_bean->persistent_property_str = AXIS2_STRDUP(
        persistent_property_str, env);
}

axis2_char_t *AXIS2_CALL 
sandesha2_msg_store_bean_get_action( 
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env)
{
    return msg_store_bean->action;
}

void AXIS2_CALL 
sandesha2_msg_store_bean_set_action(
    sandesha2_msg_store_bean_t *msg_store_bean,
    const axis2_env_t *env,
    axis2_char_t * action) 
{
    msg_store_bean->action = AXIS2_STRDUP(action, env);
}
