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
 
#include <sandesha2/sandesha2_create_seq.h>
#include <sandesha2/sandesha2_constants.h>
#include <axiom_soap_body.h>
/** 
 * @brief CreateSeq struct impl
 *	Sandesha2 IOM CreateSeq
 */
typedef struct sandesha2_create_seq_impl sandesha2_create_seq_impl_t;  
  
struct sandesha2_create_seq_impl
{
	sandesha2_create_seq_t create_seq;
	sandesha2_acks_to_t *acks_to;
	sandesha2_expires_t *expires;
	sandesha2_seq_offer_t *seq_offer;
	axis2_char_t *rm_ns_val;
	axis2_char_t *addr_ns_val;
};

#define SANDESHA2_INTF_TO_IMPL(create_seq) \
						((sandesha2_create_seq_impl_t *)(create_seq))

/***************************** Function headers *******************************/
axis2_char_t* AXIS2_CALL 
sandesha2_create_seq_get_namespace_value (
                        sandesha2_iom_rm_element_t *create_seq,
						const axis2_env_t *env);
    
void* AXIS2_CALL 
sandesha2_create_seq_from_om_node(sandesha2_iom_rm_element_t *create_seq,
                    	const axis2_env_t *env, axiom_node_t *om_node);
    
axiom_node_t* AXIS2_CALL 
sandesha2_create_seq_to_om_node(sandesha2_iom_rm_element_t *element,
                    	const axis2_env_t *env, void *om_node);
                    	
axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_is_namespace_supported(
                        sandesha2_iom_rm_element_t *create_seq,
                    	const axis2_env_t *env, axis2_char_t *namespace);
                    	
sandesha2_acks_to_t * AXIS2_CALL
sandesha2_create_seq_get_acks_to(sandesha2_create_seq_t *create_seq,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_create_seq_set_acks_to(sandesha2_create_seq_t *create_seq,
                    	const axis2_env_t *env, sandesha2_acks_to_t *acks_to);

sandesha2_seq_offer_t * AXIS2_CALL
sandesha2_create_seq_get_seq_offer(sandesha2_create_seq_t *create_seq,
                    	const axis2_env_t *env);

axis2_status_t AXIS2_CALL                 
sandesha2_create_seq_set_seq_offer(sandesha2_create_seq_t *create_seq,
                    	const axis2_env_t *env, 
                        sandesha2_seq_offer_t *seq_offer);
                    	
axis2_status_t AXIS2_CALL
sandesha2_create_seq_to_soap_env(sandesha2_iom_rm_part_t *create_seq,
                    	const axis2_env_t *env, 
                        axiom_soap_envelope_t *envelope);
                    	                    	
axis2_status_t AXIS2_CALL 
sandesha2_create_seq_free (sandesha2_iom_rm_element_t *create_seq, 
						const axis2_env_t *env);								

/***************************** End of function headers ************************/

AXIS2_EXTERN sandesha2_create_seq_t* AXIS2_CALL
sandesha2_create_seq_create(const axis2_env_t *env,  axis2_char_t *addr_ns_val,
							axis2_char_t *rm_ns_val)
{
    sandesha2_create_seq_impl_t *create_seq_impl = NULL;
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, rm_ns_val, NULL);
	AXIS2_PARAM_CHECK(env->error, addr_ns_val, NULL);
    
    if(AXIS2_FALSE == sandesha2_create_seq_is_namespace_supported(
                        (sandesha2_iom_rm_element_t*)create_seq_impl, env, 
                        rm_ns_val))
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_UNSUPPORTED_NS, 
                            AXIS2_FAILURE);
        return NULL;
    }    
    create_seq_impl =  (sandesha2_create_seq_impl_t *)AXIS2_MALLOC 
                        (env->allocator, sizeof(sandesha2_create_seq_impl_t));
	
    if(NULL == create_seq_impl)
	{
		AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    create_seq_impl->rm_ns_val = NULL;
    create_seq_impl->addr_ns_val = NULL;
    create_seq_impl->acks_to = NULL;
    create_seq_impl->expires = NULL;
    create_seq_impl->seq_offer = NULL;
    create_seq_impl->create_seq.ops = NULL;
    create_seq_impl->create_seq.part.ops = NULL;
    create_seq_impl->create_seq.part.element.ops = NULL;
    
    create_seq_impl->create_seq.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_create_seq_ops_t));
    if(NULL == create_seq_impl->create_seq.ops)
	{
		sandesha2_create_seq_free((sandesha2_iom_rm_element_t*)
                         create_seq_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    create_seq_impl->create_seq.part.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_part_ops_t));
    if(NULL == create_seq_impl->create_seq.part.ops)
	{
		sandesha2_create_seq_free((sandesha2_iom_rm_element_t*)
                         create_seq_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    create_seq_impl->create_seq.part.element.ops = AXIS2_MALLOC(env->allocator,
        sizeof(sandesha2_iom_rm_element_ops_t));
    if(NULL == create_seq_impl->create_seq.part.element.ops)
	{
		sandesha2_create_seq_free((sandesha2_iom_rm_element_t*)
                         create_seq_impl, env);
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
	}
    
    create_seq_impl->rm_ns_val = (axis2_char_t*)AXIS2_STRDUP(rm_ns_val, env);
    create_seq_impl->addr_ns_val = (axis2_char_t*)AXIS2_STRDUP(addr_ns_val, env);
    
    create_seq_impl->create_seq.part.element.ops->get_namespace_value = 
                        sandesha2_create_seq_get_namespace_value;
    create_seq_impl->create_seq.part.element.ops->from_om_node = 
    					sandesha2_create_seq_from_om_node;
    create_seq_impl->create_seq.part.element.ops->to_om_node = 
    					sandesha2_create_seq_to_om_node;
    create_seq_impl->create_seq.part.element.ops->is_namespace_supported = 
    					sandesha2_create_seq_is_namespace_supported;
    create_seq_impl->create_seq.part.ops->to_soap_env = 
                        sandesha2_create_seq_to_soap_env;
    create_seq_impl->create_seq.ops->set_acks_to = 
                        sandesha2_create_seq_set_acks_to;
    create_seq_impl->create_seq.ops->get_acks_to = 
                        sandesha2_create_seq_get_acks_to;
    create_seq_impl->create_seq.ops->set_seq_offer = 
                        sandesha2_create_seq_set_seq_offer;
    create_seq_impl->create_seq.ops->get_seq_offer = 
                        sandesha2_create_seq_get_seq_offer;
    create_seq_impl->create_seq.part.element.ops->free = 
                        sandesha2_create_seq_free;
                        
	return &(create_seq_impl->create_seq);
}


axis2_status_t AXIS2_CALL 
sandesha2_create_seq_free (sandesha2_iom_rm_element_t *create_seq, 
						const axis2_env_t *env)
{
    sandesha2_create_seq_impl_t *create_seq_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
    
    if(NULL != create_seq_impl->rm_ns_val)
    {
        AXIS2_FREE(env->allocator, create_seq_impl->rm_ns_val);
        create_seq_impl->rm_ns_val = NULL;
    }
    if(NULL != create_seq_impl->addr_ns_val)
    {
        AXIS2_FREE(env->allocator, create_seq_impl->addr_ns_val);
        create_seq_impl->addr_ns_val = NULL;
    }
    create_seq_impl->acks_to = NULL;
    create_seq_impl->expires = NULL;
    create_seq_impl->seq_offer = NULL;
    if(NULL != create_seq->ops)
    {
        AXIS2_FREE(env->allocator, create_seq->ops);
        create_seq->ops = NULL;
    }
    if(NULL != create_seq_impl->create_seq.part.ops)
    {
        AXIS2_FREE(env->allocator, create_seq_impl->create_seq.part.ops);
        create_seq_impl->create_seq.part.ops = NULL;
    }
    if(NULL != create_seq_impl->create_seq.part.element.ops)
    {
        AXIS2_FREE(env->allocator, create_seq_impl->create_seq.part.element.ops);
        create_seq_impl->create_seq.part.element.ops = NULL;
    }
	AXIS2_FREE(env->allocator, SANDESHA2_INTF_TO_IMPL(create_seq));
	return AXIS2_SUCCESS;
}

axis2_char_t* AXIS2_CALL 
sandesha2_create_seq_get_namespace_value (
                        sandesha2_iom_rm_element_t *create_seq,
						const axis2_env_t *env)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
	return create_seq_impl->rm_ns_val;
}


void* AXIS2_CALL 
sandesha2_create_seq_from_om_node(sandesha2_iom_rm_element_t *create_seq,
                    	const axis2_env_t *env, axiom_node_t *om_node)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
    axiom_element_t *om_element = NULL;
    axiom_element_t *seq_part = NULL;
    axiom_element_t *offer_part = NULL;
    axiom_element_t *expires_part = NULL;
    axiom_node_t *seq_node = NULL;
    axiom_node_t *offer_node = NULL;
    axiom_node_t *expires_node = NULL;
    axis2_qname_t *seq_qname = NULL;
    axis2_qname_t *offer_qname = NULL;
    axis2_qname_t *expires_qname = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
    om_element = AXIOM_NODE_GET_DATA_ELEMENT(om_node, env);
    if(NULL == om_element)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    seq_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_CREATE_SEQUENCE,
                        create_seq_impl->rm_ns_val, NULL);
    if(NULL == seq_qname)
    {
        return NULL;
    }
    seq_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(om_element, env,
                        seq_qname, om_node, &seq_node);
    if(NULL == seq_part)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_NULL_OM_ELEMENT,
                        AXIS2_FAILURE);
        return NULL;
    }
    
    create_seq_impl->acks_to = sandesha2_acks_to_create(env, NULL,
                        create_seq_impl->rm_ns_val, 
                        create_seq_impl->addr_ns_val);
    if(NULL == create_seq_impl->acks_to)
    {
        return NULL;
    }
    if(NULL == SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(
                        ((sandesha2_iom_rm_element_t*)
                        create_seq_impl->acks_to), env, seq_node))
    {
        return NULL;
    }
    offer_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_SEQUENCE_OFFER, 
                        create_seq_impl->rm_ns_val, NULL);
    if(NULL == offer_qname)
    {
        return NULL;
    }
    offer_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(seq_part, env, 
                        offer_qname, seq_node, &offer_node);
    if(NULL != offer_part)
    {
        create_seq_impl->seq_offer = sandesha2_seq_offer_create(env, 
                        create_seq_impl->rm_ns_val);  
        if(NULL == create_seq_impl->seq_offer)
        {
            return NULL;
        }
        if(NULL == SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(
                        ((sandesha2_iom_rm_element_t*)
                        create_seq_impl->seq_offer), env, seq_node))
        {
            return NULL;
        } 
    }
    expires_qname = axis2_qname_create(env, SANDESHA2_WSRM_COMMON_EXPIRES, 
                        create_seq_impl->rm_ns_val, NULL);
    if(NULL == expires_qname)
    {
        return NULL;
    }
    expires_part = AXIOM_ELEMENT_GET_FIRST_CHILD_WITH_QNAME(seq_part, env, 
                        expires_qname, seq_node, &expires_node);
    if(NULL != expires_part)
    {
        create_seq_impl->expires = sandesha2_expires_create(env, 
                        create_seq_impl->rm_ns_val);
        if(NULL == create_seq_impl->expires)
        {
            return NULL;
        }
        if(NULL == SANDESHA2_IOM_RM_ELEMENT_FROM_OM_NODE(
                        ((sandesha2_iom_rm_element_t*)create_seq_impl->expires),
                        env, seq_node))
        {
            return NULL;
        }
    }
    return create_seq;
}

axiom_node_t* AXIS2_CALL 
sandesha2_create_seq_to_om_node(sandesha2_iom_rm_element_t *create_seq,
                    	const axis2_env_t *env, void *om_node)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
    axiom_namespace_t *rm_ns = NULL;
    axiom_element_t *cs_element = NULL;
    axiom_node_t *cs_node = NULL;
    
    AXIS2_ENV_CHECK(env, NULL);
    AXIS2_PARAM_CHECK(env->error, om_node, NULL);
    
    create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
    if(NULL == create_seq_impl->acks_to)
    {
        AXIS2_ERROR_SET(env->error, SANDESHA2_ERROR_TO_OM_NULL_ELEMENT, 
                        AXIS2_FAILURE);
        return NULL;
    }
    rm_ns = axiom_namespace_create(env, create_seq_impl->rm_ns_val,
                        SANDESHA2_WSRM_COMMON_NS_PREFIX_RM);
    if(NULL == rm_ns)
    {
        return NULL;
    }
    cs_element = axiom_element_create(env, NULL, 
                        SANDESHA2_WSRM_COMMON_CREATE_SEQUENCE, rm_ns, 
                        &cs_node);
    if(NULL == cs_element)
    {
        return NULL;
    }
    SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        create_seq_impl->acks_to), env, cs_node);
    if(NULL != create_seq_impl->expires)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        create_seq_impl->expires), env, cs_node);
    }
    if(NULL != create_seq_impl->seq_offer)
    {
        SANDESHA2_IOM_RM_ELEMENT_TO_OM_NODE(((sandesha2_iom_rm_element_t*)
                        create_seq_impl->seq_offer), env, cs_node);
    }
    AXIOM_NODE_ADD_CHILD((axiom_node_t*)om_node, env, cs_node);
    return (axiom_node_t*)om_node;
}

axis2_bool_t AXIS2_CALL 
sandesha2_create_seq_is_namespace_supported(
                        sandesha2_iom_rm_element_t *create_seq,
                    	const axis2_env_t *env, axis2_char_t *namespace)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
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

sandesha2_acks_to_t * AXIS2_CALL
sandesha2_create_seq_get_acks_to(sandesha2_create_seq_t *create_seq,
                    	const axis2_env_t *env)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
	
	return create_seq_impl->acks_to;
}                    	

axis2_status_t AXIS2_CALL                 
sandesha2_create_seq_set_acks_to(sandesha2_create_seq_t *create_seq,
                    	const axis2_env_t *env, sandesha2_acks_to_t *acks_to)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
 	if(NULL != create_seq_impl->acks_to)
	{
		SANDESHA2_IOM_RM_ELEMENT_FREE((sandesha2_iom_rm_element_t*)
                        create_seq_impl->acks_to, env);
		create_seq_impl->acks_to = NULL;
	}
	create_seq_impl->acks_to = acks_to;
 	return AXIS2_SUCCESS;
}

sandesha2_seq_offer_t * AXIS2_CALL
sandesha2_create_seq_get_seq_offer(sandesha2_create_seq_t *create_seq,
                    	const axis2_env_t *env)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
	AXIS2_ENV_CHECK(env, NULL);
	
	create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
	
	return create_seq_impl->seq_offer;
}                    	

axis2_status_t AXIS2_CALL                 
sandesha2_create_seq_set_seq_offer(sandesha2_create_seq_t *create_seq,
                    	const axis2_env_t *env, 
                        sandesha2_seq_offer_t *seq_offer)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
	
	create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
 	if(NULL != create_seq_impl->seq_offer)
	{
		SANDESHA2_IOM_RM_ELEMENT_FREE((sandesha2_iom_rm_element_t*)
                        create_seq_impl->seq_offer, env);
		create_seq_impl->seq_offer = NULL;
	}
	create_seq_impl->seq_offer = seq_offer;
 	return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
sandesha2_create_seq_to_soap_env(sandesha2_iom_rm_part_t *create_seq,
                    	const axis2_env_t *env, axiom_soap_envelope_t *envelope)
{
	sandesha2_create_seq_impl_t *create_seq_impl = NULL;
    axiom_node_t *body_node = NULL;
    axis2_qname_t *create_seq_qname = NULL;
    
	AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, envelope, AXIS2_FAILURE);
	
	create_seq_impl = SANDESHA2_INTF_TO_IMPL(create_seq);
    /**
     * Remove if old exists
     */
    create_seq_qname = axis2_qname_create(env, 
                        SANDESHA2_WSRM_COMMON_CREATE_SEQUENCE, 
                        create_seq_impl->rm_ns_val, NULL);
    if(NULL == create_seq_qname)
    {
        return AXIS2_FAILURE;
    }
    sandesha2_utils_remove_soap_body_part(env, envelope, create_seq_qname);
    body_node = AXIOM_SOAP_BODY_GET_BASE_NODE(AXIOM_SOAP_ENVELOPE_GET_BODY(
                        envelope, env), env);
    sandesha2_create_seq_to_om_node((sandesha2_iom_rm_element_t*)create_seq, 
                        env, body_node);
	return AXIS2_SUCCESS;
}