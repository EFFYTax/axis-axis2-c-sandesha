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
#ifndef CALC_H
#define CALC_H


#include <axis2_svc_skeleton.h>
#include <axis2_log_default.h>
#include <axis2_error_default.h>
#include <axiom_text.h>
#include <axiom_node.h>
#include <axiom_element.h>

axiom_node_t *
rm_sample_svc_echo(
    const axis2_env_t *env, 
    axiom_node_t *node);

void
rm_sample_svc_ping (
    const axis2_env_t *env, 
    axiom_node_t *node);

axiom_node_t *
rm_sample_svc_mtom(
    const axis2_env_t *env, 
    axiom_node_t *node);

#endif /* CALC_H*/
