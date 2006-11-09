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
#include <sandesha2_property_bean.h>
#include <sandesha2_constants.h>
#include <stdio.h>
#include <axis2_string.h>
#include <axis2_utils.h>

/** 
 * @brief Property Bean struct impl
 *	Sandesha2 Property Bean
 */
struct sandesha2_property_bean_t
{
    long inactive_timeout_interval;
    long ack_interval;
    long retrans_interval;
    axis2_bool_t is_exp_backoff;
    axis2_char_t *in_mem_storage_mgr;
    axis2_char_t *permanant_storage_mgr;
    axis2_bool_t is_in_order;
    axis2_array_list_t *msg_types_to_drop;
    int max_retrans_count;
};

AXIS2_EXTERN sandesha2_property_bean_t* AXIS2_CALL
sandesha2_property_bean_create(
    const axis2_env_t *env)
{
    sandesha2_property_bean_t *bean = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    
    bean =  (sandesha2_property_bean_t *)AXIS2_MALLOC 
        (env->allocator, sizeof(sandesha2_property_bean_t));
	
    if(!bean)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    bean->inactive_timeout_interval = 0;
    bean->ack_interval = 0;
    bean->retrans_interval = 0;
    bean->is_exp_backoff = 0;
    bean->in_mem_storage_mgr = NULL;
    bean->permanant_storage_mgr= NULL;
    bean->is_in_order = AXIS2_FALSE;
    bean->msg_types_to_drop = NULL;
    bean->max_retrans_count = 0;
    
	return bean;
}


axis2_status_t AXIS2_CALL 
sandesha2_property_bean_free(
    sandesha2_property_bean_t *bean, 
    const axis2_env_t *env)
{
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(bean->msg_types_to_drop)
    {
        int count = AXIS2_ARRAY_LIST_SIZE(bean->msg_types_to_drop,
                        env);
        int i = 0;
        for(i = 0; i < count; i++)
        {
            int *msg_type = AXIS2_ARRAY_LIST_GET(
                        bean->msg_types_to_drop, env, i);
            if(NULL != msg_type)
                AXIS2_FREE(env->allocator, msg_type);
        }
        AXIS2_ARRAY_LIST_FREE(bean->msg_types_to_drop, env);
    }
    if(bean->in_mem_storage_mgr)
    {
        AXIS2_FREE(env->allocator, bean->in_mem_storage_mgr);
        bean->in_mem_storage_mgr = NULL;
    }
    if(bean->permanant_storage_mgr)
    {
        AXIS2_FREE(env->allocator, bean->permanant_storage_mgr);
        bean->permanant_storage_mgr = NULL;
    }
	AXIS2_FREE(env->allocator, bean);
	return AXIS2_SUCCESS;
}

long AXIS2_CALL 
sandesha2_property_bean_get_inactive_timeout_interval(
    sandesha2_property_bean_t *bean, 
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return bean->inactive_timeout_interval;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_inactive_timeout_interval(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, long interval)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    bean->inactive_timeout_interval = interval;
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_inactive_timeout_interval_with_units(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, long interval,
    axis2_char_t *units)
{
    long multiplier = -1;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, units, AXIS2_FAILURE);
    
    if(0 == AXIS2_STRCMP(units, "seconds"))
        multiplier = 1000;
    else if(0 == AXIS2_STRCMP(units, "minutes"))
        multiplier = 60*1000;
    else if(0 == AXIS2_STRCMP(units, "hours"))
        multiplier = 60*60*1000;
    else if(0 == AXIS2_STRCMP(units, "days"))
        multiplier = 24*60*60*1000;
    else
        return AXIS2_FAILURE;
        
    return sandesha2_property_bean_set_inactive_timeout_interval(bean, env, 
                        multiplier*interval);
}
            
long AXIS2_CALL
sandesha2_property_bean_get_ack_interval(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return bean->ack_interval;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_ack_interval(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, long interval)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    bean->ack_interval = interval;
    return AXIS2_SUCCESS;
}
            
long AXIS2_CALL
sandesha2_property_bean_get_retrans_interval( 
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return bean->retrans_interval;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_retrans_interval(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, long interval)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    bean->retrans_interval = interval;
    return AXIS2_SUCCESS;
}
            
axis2_bool_t AXIS2_CALL
sandesha2_property_bean_is_exp_backoff(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env)

{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    return bean->is_exp_backoff;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_exp_backoff(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, axis2_bool_t exp_backoff)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    bean->is_exp_backoff = exp_backoff;
    return AXIS2_SUCCESS;
}
            
axis2_char_t* AXIS2_CALL
sandesha2_property_bean_get_in_mem_storage_mgr(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return bean->in_mem_storage_mgr;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_in_mem_storage_mgr(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, axis2_char_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, manager, AXIS2_FAILURE);
    bean->in_mem_storage_mgr = AXIS2_STRDUP(manager,
                        env);
    return AXIS2_SUCCESS;
}
            
axis2_char_t* AXIS2_CALL
sandesha2_property_bean_get_permanant_storage_mgr(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return bean->permanant_storage_mgr;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_permanant_storage_mgr(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, axis2_char_t *manager)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, manager, AXIS2_FAILURE);
    bean->permanant_storage_mgr = AXIS2_STRDUP(
                        manager, env);
    return AXIS2_SUCCESS;
}
            
axis2_bool_t AXIS2_CALL
sandesha2_property_bean_is_in_order(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    return bean->is_in_order;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_in_order(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, axis2_bool_t in_order)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    bean->is_in_order = in_order;
    return AXIS2_SUCCESS;
}
            
axis2_array_list_t* AXIS2_CALL
sandesha2_property_bean_get_msg_types_to_drop(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return bean->msg_types_to_drop;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_msg_types_to_drop(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, axis2_array_list_t *msg_types)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, msg_types, AXIS2_FAILURE);
    
    bean->msg_types_to_drop = msg_types;
    return AXIS2_SUCCESS;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_add_msg_type_to_drop(
                        sandesha2_property_bean_t *bean,
                        const axis2_env_t *env, int msg_type)
{
    int *_msg_type = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    if(NULL == bean->msg_types_to_drop)
        return AXIS2_FAILURE;
    _msg_type = AXIS2_MALLOC(env->allocator, sizeof(int));
    AXIS2_ARRAY_LIST_ADD(bean->msg_types_to_drop, env, _msg_type);
    return AXIS2_SUCCESS;
}
            
int AXIS2_CALL
sandesha2_property_bean_get_max_retrans_count (
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return bean->max_retrans_count;
}
            
axis2_status_t AXIS2_CALL
sandesha2_property_bean_set_max_retrans_count(
    sandesha2_property_bean_t *bean,
    const axis2_env_t *env, int count)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    bean->max_retrans_count = count;
    return AXIS2_SUCCESS;
}
