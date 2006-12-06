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

#include <sandesha2_next_msg_bean.h>
#include <sandesha2_rm_bean.h>
#include <string.h>
#include <axis2_string.h>
#include <axis2_utils.h>

/* next_msg_bean struct */
struct sandesha2_next_msg_bean_t
{
    sandesha2_rm_bean_t *rm_bean;
	axis2_char_t *seq_id;
    axis2_char_t *ref_msg_key;
    axis2_bool_t polling_mode;
	long msg_no;
};

AXIS2_EXTERN sandesha2_next_msg_bean_t* AXIS2_CALL
sandesha2_next_msg_bean_create(const axis2_env_t *env)
{
	sandesha2_next_msg_bean_t *bean = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean = (sandesha2_next_msg_bean_t *)AXIS2_MALLOC(env->allocator,
							sizeof(sandesha2_next_msg_bean_t));

	if(!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init the properties. */
	bean->seq_id = NULL;
    bean->ref_msg_key = NULL;
	bean->msg_no = -1;
    bean->polling_mode = AXIS2_FALSE;
    bean->rm_bean = NULL;

	return bean;
}

AXIS2_EXTERN sandesha2_next_msg_bean_t* AXIS2_CALL
sandesha2_next_msg_bean_create_with_data(
    const axis2_env_t *env,
	axis2_char_t *seq_id,
	long msg_no)
{
	sandesha2_next_msg_bean_t *bean = NULL;
	AXIS2_ENV_CHECK(env, NULL);

	bean = (sandesha2_next_msg_bean_t *)AXIS2_MALLOC(env->allocator,
	    sizeof(sandesha2_next_msg_bean_t));

	if(!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
		return NULL;
	}

	/* init the properties. */
	bean->seq_id = (axis2_char_t*)AXIS2_STRDUP(seq_id, env);
    bean->ref_msg_key = NULL;
	bean->msg_no = msg_no;
    bean->polling_mode = AXIS2_FALSE;
    bean->rm_bean = NULL;

	return bean;
}

axis2_status_t AXIS2_CALL
sandesha2_next_msg_bean_free (
    sandesha2_next_msg_bean_t *next_msg_bean,
	const axis2_env_t *env)
{
	if(next_msg_bean->rm_bean)
	{
		sandesha2_rm_bean_free(next_msg_bean->rm_bean, env);
		next_msg_bean->rm_bean= NULL;
	}
	if(next_msg_bean->seq_id)
	{
		AXIS2_FREE(env->allocator, next_msg_bean->seq_id);
		next_msg_bean->seq_id= NULL;
	}
	if(next_msg_bean->ref_msg_key)
	{
		AXIS2_FREE(env->allocator, next_msg_bean->ref_msg_key);
		next_msg_bean->ref_msg_key= NULL;
	}
    return AXIS2_SUCCESS;
}

sandesha2_rm_bean_t * AXIS2_CALL
sandesha2_next_msg_bean_get_base( 
    sandesha2_next_msg_bean_t* next_msg,
    const axis2_env_t *env)
{
	return next_msg->rm_bean;

}	

void AXIS2_CALL
sandesha2_next_msg_bean_set_base (
    sandesha2_next_msg_bean_t *next_msg,
    const axis2_env_t *env, 
    sandesha2_rm_bean_t* rm_bean)

{
	next_msg->rm_bean = rm_bean;
}

axis2_char_t* AXIS2_CALL
sandesha2_next_msg_bean_get_seq_id(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axis2_env_t *env)
{
	return next_msg_bean->seq_id;
}


void AXIS2_CALL
sandesha2_next_msg_bean_set_seq_id(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axis2_env_t *env, 
    axis2_char_t *seq_id)
{
	if(next_msg_bean->seq_id)
	{
		AXIS2_FREE(env->allocator, next_msg_bean->seq_id);
		next_msg_bean->seq_id = NULL;
	}

	next_msg_bean->seq_id = (axis2_char_t*) AXIS2_STRDUP(seq_id, env); 
}


long AXIS2_CALL
sandesha2_next_msg_bean_get_next_msg_no_to_process(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axis2_env_t *env)
{
	return next_msg_bean->msg_no;
}

void AXIS2_CALL
sandesha2_next_msg_bean_set_next_msg_no_to_process(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axis2_env_t *env, 
    long next_msg_no)
{
	next_msg_bean->msg_no = next_msg_no;
}

axis2_bool_t AXIS2_CALL
sandesha2_next_msg_bean_is_polling_mode(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axis2_env_t *env) 
{
    return next_msg_bean->polling_mode;
}

void AXIS2_CALL
sandesha2_next_msg_bean_set_polling_mode(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axis2_env_t *env,
    axis2_bool_t polling_mode) 
{
    next_msg_bean->polling_mode = polling_mode;
}

axis2_char_t *AXIS2_CALL
sandesha2_next_msg_bean_get_ref_msg_key(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axis2_env_t *env) 
{
    return next_msg_bean->ref_msg_key;
}

void AXIS2_CALL
sandesha2_next_msg_bean_set_ref_msg_key(
    sandesha2_next_msg_bean_t *next_msg_bean,
    const axis2_env_t *env,
    axis2_char_t *ref_msg_key) 
{
    next_msg_bean->ref_msg_key = ref_msg_key;
}


