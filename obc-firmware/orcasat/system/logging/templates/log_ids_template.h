/* 
 * log_ids.h
 *
 *  Created on: Jun 19, 2019
 *  Re-generated: {{date}}
 * 
 *      Author: Richard
 *      
 *      IDs for logging system
 *      
 *      NOTE: this is an automatically generated file from the OBC system sheet, please do not update it. 
*/

// Turn off formatting so that the template doesn't get adjusted by the formatter.
// clang-format off

#ifndef LOG_ID_H_
#define LOG_ID_H_

{% for item in items %}
#define {{ item[3] }} {{ item[0] }} /* {{item[1]}} */
{% endfor %}

// clang-format on

#endif /* LOG_ID_H_ */
