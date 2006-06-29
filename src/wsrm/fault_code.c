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
 
#include <sandesha2/sandesha2_fault_code.h>
#include <sandesha2/sandesha2_constants.h>
/** 
 * @brief FaultCode struct impl
 *	Sandesha2 IOM FaultCode
 */
typedef struct sandesha2_fault_code_impl sandesha2_fault_code_impl_t;  
  
struct sandesha2_fault_code_impl
{
	sandesha2_fault_code_t fault_code;
	axis2_char_t *str_fault_code;
	axis2_char_t *ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(fault_code) \
						((sandesha2_fault_code_impl_t *)(fault_code))

/***************************** Function headers *******************************/
axis2_char_t* AXIS2_CALL 
sandesha2_fault_code_get_namespace_value (
                        sandesha2_iom_rm_element_t *fault_code,
						const axis2_env_t *env);
    
void* AXIS2_CALL 
sandesha2_fault_code_from_om_node(sandesha2_iom_rm_element_t *fault_code,
                    	const axis2_env_t *env, axiom_node_t *om_node);
    
axiom_node_t* AXIS2_CALL 
sandesha2_fault_code_to_om_node(sandesha2_iom_rm_element_t *fault_code,
                    	const axis2_env_t *env, void *om_node);
                    	
axis2_bool_t AXIS2_CALL 
sandesha2_fault_code_is_namespace_supported(
                        sandesha2_iom_rm_element_t *fault_code,
                    	const axis2_env_t *env, axis2_char_t *namespace);
                    	
axis2_char_t * AXIS2_CALL
sandesha2_fault_code_get_fault_code(sandesha2_fault_code_t *fault_code,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_fault_code_set_fault_code(sandesha2_fault_code_t *fault_code,
                    	const axis2_env_t *env, axis2_char_t *str_fault_code);

axis2_status_t AXIS2_CALL 
sandesha2_fault_code_free (sandesha2_iom_rm_element_t *fault_code, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_fault_code_t* AXIS2_CALL
sandesha2_fault_code_create(const axis2_env_t *env,  axis2_char_t *ns_val)
{
    sandesha2_fault_code_impl_t *fault_code_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, ns_val, NULL);

    if(AXIS2_FALSE == sandesha2_fault_code_is_namespace_supported(
                        (sandesha2_iom_rm_element_t*)fault_code_impl, env, 
                        ns_val))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNSUPPORTED_NS, 
                            AXIS2_FAILURE);
        return NULL;
    }        
    fault_code_impl =  (sandesha2_fault_code_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_fault_code_impl_t));
	
    if(NULL == fault_code_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    fault_code_impl->ns_val = NULL;
    fault_code_impl->str_fault_code = NULL;
    fault_code_impl->fault_code.ops = NULL;
    fault_code_impl->fault_code.element.ops = NULL;
    
    fault_code_impl->fault_code.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_fault_code_ops_t));
    if(NULL == fault_code_impl->fault_code.ops)
	{
		sandesha2_fault_code_free((sandesha2_iom_rm_element_t*)
                         fault_code_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    fault_code_impl->fault_code.element.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_element_ops_t));
    if(NULL == fault_code_impl->fault_code.element.ops)
	{
		sandesha2_fault_code_free((sandesha2_iom_rm_element_t*)
                         fault_code_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    fault_code_impl->ns_val = (axis2_char_t *)AXIS2_STRDUP(ns_val, env);
    
    fault_code_impl->fault_code.element.ops->get_namespace_value = 
                        sandesha2_fault_code_get_namespace_value;
    fault_code_impl->fault_code.element.ops->from_om_node = 
    					sandesha2_fault_code_from_om_node;
    fault_code_impl->fault_code.element.ops->to_om_node = 
    					sandesha2_fault_code_to_om_node;
    fault_code_impl->fault_code.element.ops->is_namespace_supported = 
    					sandesha2_fault_code_is_namespace_supported;
    fault_code_impl->fault_code.ops->get_fault_code = 
                        sandesha2_fault_code_get_fault_code;
    fault_code_impl->fault_code.ops->set_fault_code = 
                        sandesha2_fault_code_set_fault_code;
    fault_code_impl->fault_code.element.ops->free = sandesha2_fault_code_free;
                        
	return &(fault_code_impl->fault_code);
}


axis2_status_t AXIS2_CALL 
sandesha2_fault_code_free (sandesha2_iom_rm_element_t *fault_code, 
						const axis2_env_t *env)
{
    sandesha2_fault_code_impl_t *fault_code_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    fault_code_impl = SANDESHA2_INTF_TO_IMPL(fault_code);
    
    if(NULL != fault_code_impl->ns_val)
    {
        AXIS2_FREE(env->allocator, fault_code_impl->ns_val);
        fault_code_impl->ns_val = NULL;
    }
    if(NULL != fault_code_impl->str_fault_code)
    {
    	AXIS2_FREE(env->allocator, fault_code_impl->str_fault_code);
    	fault_code_impl->str_fault_code = NULL;
    }
    if(NULL != fault_code->ops)
    {
        AXIS2_FREE(env->allocator, fault_code->ops);
        fault_code->ops = NULL;
    }
    if(NULL != fault_code_impl->fault_code.element.ops)
    {
        AXIS2_FREE(env->allocator, fault_code_impl->fault_code.element.ops);
        fault_code_impl->fault_code.element.ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(fault_code));
	return AXIS2_SUCCESS;
}

axis2_char_t* AXIS2_CALL 
sandesha2_fault_code_get_namespace_value (
                        sandesha2_iom_rm_element_t *fault_code,
						const axis2_env_t *env)
{
	sandesha2_fault_code_impl_t *fault_code_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	fault_code_impl = SANDESHA2_INTF_TO_IMPL(fault_code);
	return fault_code_impl->ns_val;
}


void* AXIS2_CALL 
sandesha2_fault_code_from_om_node(sandesha2_iom_rm_element_t *fault_code,
                    	const axis2_env_t *env, axiom_node_t *om_node)
{
	sandesha2_fault_code_impl_t *fault_code_impl = NULL;
    axiom_element_t *om_element = NULL;
    axiom_element_t *fault_part = NULL;
    axiom_node_t *fault_node = NULL;
    axis2_qname_t *fault_qname = NULL;
    axis2_char_t *fault_text = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    fault_code_impl = SANDESHA2_INTF_TO_IMPL(fault_code);
    om_element = AXIOM_NODE_GET_DATA_ELEMENT(om_node, env);
    if(NULL == om_element)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    fault_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_FAULT_CODE, 
                        fault_code_impl->ns_val, NULL);
    if(NULL == fault_qname)
    {
        return NULL;
    }
    fault_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(om_element, env,
                        fault_qname, om_node, &fault_node);
    if(NULL == fault_part)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    } 
    fault_text = AXIOM_ELEMENT_GET_TEXT(fault_part, env, fault_node);
    if(NULL == fault_text)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_EMPTY_OM_ELEMENT, 
                        AXIS2_FAILURE)
        return NULL;
    }
    fault_code_impl->str_fault_code = AXIS2_STRDUP(fault_text, env);
    if(NULL == fault_code_impl->str_fault_code)
    {
        return NULL;
    }
    return fault_code;
}


axiom_node_t* AXIS2_CALL 
sandesha2_fault_code_to_om_node(sandesha2_iom_rm_element_t *fault_code,
                    	const axis2_env_t *env, void *om_node)
{
	sandesha2_fault_code_impl_t *fault_code_impl = NULL;
    axiom_namespace_t *rm_ns = NULL;
    axiom_element_t *fc_element = NULL;
    axiom_node_t *fc_node = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    fault_code_impl = SANDESHA2_INTF_TO_IMPL(fault_code);
    if(NULL == fault_code_impl->str_fault_code || 0 == AXIS2_STRLEN(
                        fault_code_impl->str_fault_code))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_TO_OM_NULL_ELEMENT, 
                        AXIS2_FAILURE);
        return NULL;
    }
    rm_ns = axiom_namespace_create(env, fault_code_impl->ns_val,
                        SANDESHA2_WSRM_COMMON_NS_PREFIX_RM);
    if(NULL == rm_ns)
    {
        return NULL;
    }
    fc_element = axiom_element_create(env, NULL, 
                        SANDESHA2_WSRM_COMMON_FAULT_CODE, rm_ns, 
                        &fc_node);
    if(NULL == fc_element)
    {
        return NULL;
    }
    AXIOM_ELEMENT_SET_TEXT(fc_element, env, fault_code_impl->str_fault_code, 
                        fc_node);
    AXIOM_NODE_ADD_CHILD((axiom_node_t*)om_node, env, fc_node);
    return (axiom_node_t*)om_node;
}

axis2_bool_t AXIS2_CALL 
sandesha2_fault_code_is_namespace_supported(
                        sandesha2_iom_rm_element_t *fault_code,
                    	const axis2_env_t *env, axis2_char_t *namespace)
{
	sandesha2_fault_code_impl_t *fault_code_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    fault_code_impl = SANDESHA2_INTF_TO_IMPL(fault_code);
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_02_NS_URI))
    {
        return AXIS2_TRUE;
    }
    if(0 == AXIS2_STRCMP(namespace, SANDESHA2_SPEC_2005_10_NS_URI))
    {
        return AXIS2_TRUE;
    }
    return AXIS2_FALSE;
}

axis2_char_t * AXIS2_CALL
sandesha2_fault_code_get_fault_code(sandesha2_fault_code_t *fault_code,
                    	const axis2_env_t *env)
{
	sandesha2_fault_code_impl_t *fault_code_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	fault_code_impl = SANDESHA2_INTF_TO_IMPL(fault_code);
	
	return fault_code_impl->str_fault_code;
}                    	


axis2_status_t AXIS2_CALL                 
sandesha2_fault_code_set_fault_code(sandesha2_fault_code_t *fault_code,
                    	const axis2_env_t *env, axis2_char_t *str_fault_code)
{
	sandesha2_fault_code_impl_t *fault_code_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	fault_code_impl = SANDESHA2_INTF_TO_IMPL(fault_code);
    if(NULL != fault_code_impl->str_fault_code)
    {
        AXIS2_FREE(env->allocator, fault_code_impl->str_fault_code);
    }
	fault_code_impl->str_fault_code = AXIS2_STRDUP(str_fault_code, env);
	return AXIS2_SUCCESS;
}