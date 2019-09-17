/*
 * Copyright (c) 2014 Devicescape Software, Inc.
 * This file is part of aws_dynamo, a C library for AWS DynamoDB.
 *
 * aws_dynamo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * aws_dynamo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with aws_dynamo.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE

#include "aws_dynamo_utils.h"

#include <stdlib.h>

#include <yajl/yajl_tree.h>

#include "http.h"
#include "aws_dynamo.h"
#include "aws_dynamo_v2_list_tables.h"
#include "aws_dynamo_json.h"

struct aws_dynamo_v2_list_tables_response *aws_dynamo_parse_v2_list_tables_response(const char *response, int response_len)
{
    int i = 0;
    yajl_val root;
    char errbuf[1024];

    struct aws_dynamo_v2_list_tables_response *listResponse = NULL;
    listResponse = calloc(1, sizeof(*(listResponse)));
    if (listResponse == NULL) {
        Warnx("aws_dynamo_parse_v2_list_tables_response: alloc failed.");
        return NULL;
    }

    memset(errbuf, 0x00, sizeof(errbuf));
    root = yajl_tree_parse(response, errbuf, sizeof(errbuf));
    if (root == NULL) {
        if (strlen(errbuf)) {
            Warnx("aws_dynamo_v2_list_tables_response: json parse failed, '%s'", errbuf);
        } else {
            Warnx("aws_dynamo_v2_list_tables_response: json parse failed, 'unknown error'");
        }
        return NULL;
    }

    /* LastEvaluatedTableName */
    yajl_val lastEvaluatedVal = yajl_getNode(root, "LastEvaluatedTableName");
    if (lastEvaluatedVal != NULL) {
        listResponse->last_evaluated_table_name = strdup(YAJL_GET_STRING(lastEvaluatedVal));
    }

    /* TableNames */
    yajl_val tableNameVal = yajl_getNode(root, "TableNames");
    int arr_len = YAJL_GET_ARRAY(tableNameVal)->len;
    if (arr_len == 0) {
        return listResponse;
    }

    listResponse->num_tables = arr_len;
    listResponse->table_names = calloc(arr_len, sizeof(char *));

    yajl_val *arr =YAJL_GET_ARRAY(tableNameVal)->values;
    for (i = 0; i < arr_len; i++) {
        yajl_val obj = arr[i];
			  listResponse->table_names[i] = strdup(YAJL_GET_STRING(obj));
    }

    yajl_tree_free(root);

    return listResponse;
}

struct aws_dynamo_v2_list_tables_response *aws_dynamo_v2_list_tables(struct aws_handle *aws, const char *request)
{
	const char *response;
	int response_len;
	struct aws_dynamo_v2_list_tables_response *r;

	if (aws_dynamo_request(aws, AWS_DYNAMO_V2_LIST_TABLES, request) == -1) {
		return NULL;
	}

	response = http_get_data(aws->http, &response_len);

	if (response == NULL) {
		Warnx("aws_dynamo_v2_list_tables: Failed to get response.");
		return NULL;
	}

	if ((r = aws_dynamo_parse_v2_list_tables_response(response, response_len)) == NULL) {
		Warnx("aws_dynamo_v2_list_tables: Failed to parse response: '%s'", response);
		return NULL;
	}

	return r;
}

void aws_dynamo_dump_v2_list_tables_response(struct aws_dynamo_v2_list_tables_response *r)
{
#ifdef DEBUG_PARSER
	int i;
	if (r == NULL) {
		Debug("Empty response.");
		return;
	}

	if (r->last_evaluated_table_name) {
		Debug("Last evaluated table name: %s", r->last_evaluated_table_name);
	}

	Debug("%d Tables:", r->num_tables);
	for (i = 0; i < r->num_tables; i++) {
		Debug("%s", r->table_names[i]);
	}
#endif
}

void aws_dynamo_free_v2_list_tables_response(struct aws_dynamo_v2_list_tables_response *r)
{
	int i;

	if (r == NULL) {
		return;
	}

	free(r->last_evaluated_table_name);

	for (i = 0; i < r->num_tables; i++) {
		free(r->table_names[i]);
	}
	free(r->table_names);

	free(r);
}
