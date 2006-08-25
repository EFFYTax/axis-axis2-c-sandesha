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
 
#include <sandesha2/sandesha2_property_mgr.h>
#include <sys/timeb.h>
#include <axis2_param.h>
#include <sandesha2/sandesha2_constants.h>
#include <sandesha2/sandesha2_error.h>
#include <sandesha2_sender.h>
#include <axis2_string.h>
#include <axis2_conf.h>
#include <axis2_property.h>
#include <axiom_soap_body.h>
#include <axis2_options.h>
#include <axis2_msg_ctx.h>
#include <axis2_transport_out_desc.h>
#include <axis2_transport_in_desc.h>
#include <axis2_qname.h>
#include <sandesha2/sandesha2_utils.h>
#include <axis2_param.h>
#include <stdlib.h>

/******************************************************************************/
AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_exp_backoff(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean);
                        
AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_retrans_int(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean);
                        
AXIS2_EXTERN  axis2_status_t AXIS2_CALL                        
sandesha2_property_mgr_load_ack_int(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean);

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_inactive_timeout(const axis2_env_t *env, 
                        axis2_char_t *value,
                        axis2_char_t *measure,                        
                        sandesha2_property_bean_t *property_bean);

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_in_mem_storage_mgr(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean);

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_perm_storage_mgr(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean);
                        
AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_in_order_invocation(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean);

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_msg_types_to_drop(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean);
                        
/******************************************************************************/
AXIS2_EXTERN  sandesha2_property_bean_t* AXIS2_CALL
sandesha2_property_mgr_load_properties_from_def_values(const axis2_env_t *env)
{
    sandesha2_property_bean_t *property_bean = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    
    property_bean = sandesha2_property_bean_create(env);
    SANDESHA2_PROPERTY_BEAN_SET_ACK_INTERVAL(property_bean, env, 
                        SANDESHA2_DEF_VAL_ACK_INTERVAL);
    SANDESHA2_PROPERTY_BEAN_SET_EXP_BACKOFF(property_bean, env,
                        SANDESHA2_DEF_VAL_EXP_BACKOFF);
    SANDESHA2_PROPERTY_BEAN_SET_INACTIVE_TIMEOUT_INTERVAL_WITH_UNITS(
                        property_bean, env, SANDESHA2_DEF_VAL_INACTIVETIMEOUT,
                        SANDESHA2_DEF_VAL_INACTIVETIMEOUT_MEASURE);
    SANDESHA2_PROPERTY_BEAN_SET_IN_ORDER(property_bean, env,
                        SANDESHA2_DEF_VAL_INORDER_INVOCATION);
    SANDESHA2_PROPERTY_BEAN_SET_MSG_TYPES_TO_DROP(property_bean, env, NULL);
    SANDESHA2_PROPERTY_BEAN_SET_RETRANS_INTERVAL(property_bean, env,
                        SANDESHA2_DEF_VAL_RETR_COUNT);
    /* will be useful when we are loading libraries */
    SANDESHA2_PROPERTY_BEAN_SET_IN_MEM_STORAGE_MGR(property_bean, env,
                        SANDESHA2_DEF_VAL_IN_MEMORY_STORAGE_MGR);
    SANDESHA2_PROPERTY_BEAN_SET_PERMENENT_STORAGE_MGR(property_bean, env,
                        SANDESHA2_DEF_VAL_PERM_STORAGE_MGR);
    SANDESHA2_PROPERTY_BEAN_SET_MAX_RETRANS_COUNT(property_bean, env,
                        SANDESHA2_DEF_VAL_MAX_RETR_COUNT);
    sandesha2_property_mgr_load_msg_types_to_drop(env, 
                        SANDESHA2_DEF_VAL_MSG_TYPES_TO_DROP, property_bean);
    return property_bean;
}

AXIS2_EXTERN  sandesha2_property_bean_t* AXIS2_CALL
sandesha2_property_mgr_load_properties_from_module_desc(const axis2_env_t *env,
                        axis2_module_desc_t *module_desc)
{
    sandesha2_property_bean_t *property_bean = NULL;
    axis2_param_t *param = NULL;
    axis2_char_t *exp_backoff_str = NULL;
    axis2_char_t *retrans_int_str = NULL;
    axis2_char_t *ack_int_str = NULL;
    axis2_char_t *inactive_timeout_str = NULL;
    axis2_char_t *in_mem_mgr_str = NULL;
    axis2_char_t *perm_mgr_str = NULL;
    axis2_char_t *in_order_invoker_str = NULL;
    axis2_char_t *msg_types_str = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, module_desc, NULL);
    
    property_bean = sandesha2_property_bean_create(env);
    
    param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_EXP_BACKOFF);
    if(NULL != param)
    {
        exp_backoff_str = AXIS2_PARAM_GET_VALUE(param, env);
        sandesha2_property_mgr_load_exp_backoff(env, exp_backoff_str, 
                        property_bean);
    }
    param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_RETRANSMISSION_INTERVAL);
    if(NULL != param)
    {
        retrans_int_str = AXIS2_PARAM_GET_VALUE(param, env);
        sandesha2_property_mgr_load_retrans_int(env, retrans_int_str, 
                        property_bean);
    }
    param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_ACK_INTERVAL);
    if(NULL != param)
    {
        ack_int_str = AXIS2_PARAM_GET_VALUE(param, env);
        sandesha2_property_mgr_load_ack_int(env, ack_int_str, 
                        property_bean);
    }
    param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_INACTIVETIMEOUT);
    if(NULL != param)
    {
        axis2_char_t *inactive_to_measure_str = NULL;
        
        inactive_timeout_str = AXIS2_PARAM_GET_VALUE(param, env);
        param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_INACTIVETIMEOUT_MEASURE);
        if(NULL != param)
            inactive_to_measure_str = AXIS2_PARAM_GET_VALUE(param, env);
        if(NULL == inactive_to_measure_str)
            inactive_to_measure_str = SANDESHA2_DEF_VAL_INACTIVETIMEOUT_MEASURE;
        
        sandesha2_property_mgr_load_inactive_timeout(env, inactive_timeout_str, 
                        inactive_to_measure_str, property_bean);
    }
    param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_IN_MEMORY_STORAGE_MGR);
    if(NULL != param)
    {
        in_mem_mgr_str = AXIS2_PARAM_GET_VALUE(param, env);
        sandesha2_property_mgr_load_in_mem_storage_mgr(env, in_mem_mgr_str, 
                        property_bean);        
    }
    param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_PERM_STORAGE_MGR);
    if(NULL != param)
    {
        perm_mgr_str = AXIS2_PARAM_GET_VALUE(param, env);
        sandesha2_property_mgr_load_perm_storage_mgr(env, perm_mgr_str, 
                        property_bean);        
    }
    param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_INORDER_INVOCATION);
    if(NULL != param)
    {
        in_order_invoker_str = AXIS2_PARAM_GET_VALUE(param, env);
        sandesha2_property_mgr_load_in_order_invocation(env, 
                        in_order_invoker_str, property_bean);        
    }
    param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, 
                        SANDESHA2_PROPERTIES_MSG_TYPES_TO_DROP);
    if(NULL != param)
    {
        msg_types_str = AXIS2_PARAM_GET_VALUE(param, env);
        sandesha2_property_mgr_load_msg_types_to_drop(env, msg_types_str, 
                        property_bean);        
    }
    return property_bean;
}

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_exp_backoff(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean)
{
    axis2_char_t *str = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, value, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, property_bean, AXIS2_FAILURE);
    
    str = sandesha2_utils_trim_string(env, value);
    if(0 == AXIS2_STRCMP(str, SANDESHA2_VALUE_TRUE))
        SANDESHA2_PROPERTY_BEAN_SET_EXP_BACKOFF(property_bean, env, AXIS2_TRUE);
    else
        SANDESHA2_PROPERTY_BEAN_SET_EXP_BACKOFF(property_bean, env, AXIS2_FALSE);
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_retrans_int(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean)
{
    axis2_char_t *str = NULL;
    int retrans_int = -1;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, value, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, property_bean, AXIS2_FAILURE);
    
    str = sandesha2_utils_trim_string(env, value);
    if(NULL != str)
        retrans_int = atoi(str);
    if(0 < retrans_int)
        SANDESHA2_PROPERTY_BEAN_SET_RETRANS_INTERVAL(property_bean, env, 
                    retrans_int);
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN  axis2_status_t AXIS2_CALL                        
sandesha2_property_mgr_load_ack_int(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean)
{
    axis2_char_t *str = NULL;
    int ack_int = -1;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, value, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, property_bean, AXIS2_FAILURE);
    
    str = sandesha2_utils_trim_string(env, value);
    if(NULL != str)
        ack_int = atoi(str);
    if(0 < ack_int)
        SANDESHA2_PROPERTY_BEAN_SET_ACK_INTERVAL(property_bean, env, ack_int);
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_inactive_timeout(const axis2_env_t *env, 
                        axis2_char_t *value,
                        axis2_char_t *measure,                        
                        sandesha2_property_bean_t *property_bean)
{
    axis2_char_t *str = NULL;
    axis2_char_t *str2 = NULL;
    int timeout = -1;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, value, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, measure, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, property_bean, AXIS2_FAILURE);
    
    str = sandesha2_utils_trim_string(env, value);
    str2 = sandesha2_utils_trim_string(env, measure);
    
    if(NULL != str)
        timeout = atoi(str);
    if(0 < timeout)
    {
        if(NULL != str2)
            SANDESHA2_PROPERTY_BEAN_SET_INACTIVE_TIMEOUT_INTERVAL_WITH_UNITS(
                        property_bean, env, timeout, str2);
        else
            SANDESHA2_PROPERTY_BEAN_SET_INACTIVE_TIMEOUT_INTERVAL(property_bean,
                        env, timeout);
    }
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_in_mem_storage_mgr(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean)
{
 
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, value, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, property_bean, AXIS2_FAILURE);
    
    SANDESHA2_PROPERTY_BEAN_SET_IN_MEM_STORAGE_MGR(property_bean, env, value);
    
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_perm_storage_mgr(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, value, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, property_bean, AXIS2_FAILURE);
    
    SANDESHA2_PROPERTY_BEAN_SET_PERMENENT_STORAGE_MGR(property_bean, env, value);
    
    return AXIS2_SUCCESS;
}
                        
AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_in_order_invocation(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean)
{
    axis2_char_t *str = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, value, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, property_bean, AXIS2_FAILURE);
    
    str = sandesha2_utils_trim_string(env, value);
    if(0 == AXIS2_STRCMP(str, SANDESHA2_VALUE_TRUE))
        SANDESHA2_PROPERTY_BEAN_SET_IN_ORDER(property_bean, env, AXIS2_TRUE);
    else
        SANDESHA2_PROPERTY_BEAN_SET_IN_ORDER(property_bean, env, AXIS2_FALSE);
    
    return AXIS2_SUCCESS;
}

AXIS2_EXTERN  axis2_status_t AXIS2_CALL
sandesha2_property_mgr_load_msg_types_to_drop(const axis2_env_t *env, 
                        axis2_char_t *value, 
                        sandesha2_property_bean_t *property_bean)
{
    axis2_char_t *str = NULL;
    
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, value, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, property_bean, AXIS2_FAILURE);
    
    str = sandesha2_utils_trim_string(env, value);
    if(NULL != str && 0 != AXIS2_STRCMP(str, SANDESHA2_VALUE_NONE))
    {
        axis2_char_t *str2 = NULL;
        axis2_array_list_t *list = NULL;
        
        str2 = axis2_strcat(env, "[", str, "]", NULL);
        list = sandesha2_utils_get_array_list_from_string(env, str2);
        if(NULL != list)
        {
            int i = 0;
            for(i = 0; i < AXIS2_ARRAY_LIST_SIZE(list, env); i++)
            {
                axis2_char_t *val = NULL;
                val = AXIS2_ARRAY_LIST_GET(list, env, i);
                SANDESHA2_PROPERTY_BEAN_ADD_MSG_TYPE_TO_DROP(property_bean, env,
                        atoi(val));
            }
        }
    }
    
    return AXIS2_SUCCESS;
}
