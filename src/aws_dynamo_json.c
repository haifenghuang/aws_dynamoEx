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

#include <string.h>
#include <stdlib.h>

#include "http.h"
#include "aws_sigv4.h"
#include "aws_dynamo_json.h"
#include "aws_dynamo_utils.h"
#include "jsmn.h"

#ifndef MAXPATH_LEVEL
    #define MAXPATH_LEVEL 256
#endif

static const char *parser_state_strings[] = {
	"none",
	"root map",
	"responses key",
	"responses map",
	"table name key",
	"table name map",
	"consumed capacity key",
	"unprocessed items key",
};

const char *parser_state_string(int state)
{
	if (state < 0 || state > sizeof(parser_state_strings) / sizeof(parser_state_strings[0])) {
		return "invalid state";
	} else {
		return parser_state_strings[state];
	}
}

void dump_token(jsmntok_t * t, const char *response)
{
	char *type;
	char *str;
	switch (t->type) {
	case JSMN_PRIMITIVE:
		type = "primitive";
		break;
	case JSMN_OBJECT:
		type = "object";
		break;
	case JSMN_ARRAY:
		type = "array";
		break;
	case JSMN_STRING:
		type = "string";
		break;
	default:
		type = "unknown";
		break;
	}
	str = strndup(response + t->start, t->end - t->start);
	Debug("%s start=%d end=%d size=%d -%s-", type, t->start, t->end, t->size, str);
	free(str);
}

yajl_val yajl_getNode(yajl_val node, const char *path) {
    int i = 0;
    const char *paths[MAXPATH_LEVEL]; /* maximum path levels */
    yajl_val result = NULL;

    for (i = 0; i < MAXPATH_LEVEL; i++) paths[i] = NULL; /* init */

    i = 0;
    char *path_tmp = strdup(path);
    char *pch = strtok(path_tmp, "/");
    while (pch != NULL) {
        if (i == MAXPATH_LEVEL) {
            Err("JSON tree path too long: '%s'", path);
            goto bail;
        }

        paths[i++] = pch;
        pch = strtok(NULL, "/");
    }

    result = yajl_tree_get(node, paths, yajl_t_any);

bail:
    free(path_tmp);
    return result;
}

int getArrayItems(yajl_val node, struct aws_dynamo_item *items, const char *path) {
    int i = 0, j = 0;

    yajl_val v = yajl_getNode(node, path);
    if (v == NULL) {
        Warnx("geArrayItems: path '%s' not found.", path);
        return 1;
    }

    int arr_len = YAJL_GET_ARRAY(v)->len;
    if (arr_len == 0) {
        return 0;
    }

    yajl_val *arr =YAJL_GET_ARRAY(v)->values;
    for (i = 0; i < arr_len; i++) {
        struct aws_dynamo_item *item = &(items[i]);

        yajl_val obj = arr[i];

        int obj_len = YAJL_GET_OBJECT(obj)->len;

        item->num_attributes = obj_len;
        item->attributes = calloc(obj_len, sizeof(*(item->attributes)));
        if (item->attributes == NULL) {
            Errx("getObjectItem: allocation failed");
            return 1;
        }

        for (j = 0; j < obj_len; j++) {
            struct aws_dynamo_attribute *attribute = &(item->attributes[j]);

            const char *key = YAJL_GET_OBJECT(obj)->keys[j];
            attribute->name = strdup(key);
            attribute->name_len = strlen(key);

            yajl_val attr = YAJL_GET_OBJECT(obj)->values[j];

            const char *attr_type = YAJL_GET_OBJECT(attr)->keys[0];
            attribute->type = str2AttrType(attr_type);

            yajl_val attr_val = YAJL_GET_OBJECT(attr)->values[0];
            const char *val = YAJL_GET_STRING(attr_val);

            aws_dynamo_parse_attribute_value(attribute, val, strlen(val));
        } /* inner for */
    } /* outer for */

    return 0;
}

int getObjectItem(yajl_val node, struct aws_dynamo_item *item, char *path) {
    yajl_val obj = yajl_getNode(node, path);
    if (obj == NULL) {
        Warnx("getObjectItem: path '%s' not found.", path);
        return 1;
    }

    int obj_len = YAJL_GET_OBJECT(obj)->len;

    item->num_attributes = obj_len;
    item->attributes = calloc(obj_len, sizeof(*(item->attributes)));
    if (item->attributes == NULL) {
        Errx("getObjectItem: allocation failed");
        return 1;
    }

    int i = 0;
    for (i = 0; i < obj_len; i++) {
        struct aws_dynamo_attribute *attribute = &item->attributes[i];
	
        const char *key = YAJL_GET_OBJECT(obj)->keys[i];
        attribute->name = strdup(key);
        attribute->name_len = strlen(key);

        yajl_val attr = YAJL_GET_OBJECT(obj)->values[i];
        const char *attr_type = YAJL_GET_OBJECT(attr)->keys[0];
        attribute->type = str2AttrType(attr_type);
	
        yajl_val attr_val = YAJL_GET_OBJECT(attr)->values[0];
        const char *val = YAJL_GET_STRING(attr_val);

        aws_dynamo_parse_attribute_value(attribute, val, strlen(val));
    } /* inner for */

    return 0;
}

enum aws_dynamo_attribute_type str2AttrType(const char *str) {
    if (strcmp(str, AWS_DYNAMO_JSON_TYPE_STRING) == 0) {
        return AWS_DYNAMO_STRING;
    } else if (strcmp(str, AWS_DYNAMO_JSON_TYPE_STRING_SET) == 0) {
        return AWS_DYNAMO_STRING_SET;
    } else if (strcmp(str, AWS_DYNAMO_JSON_TYPE_NUMBER) == 0) {
        return AWS_DYNAMO_NUMBER;
    } else if (strcmp(str, AWS_DYNAMO_JSON_TYPE_NUMBER_SET) == 0) {
        return AWS_DYNAMO_NUMBER_SET;
    }

    return AWS_DYNAMO_STRING;
}

char *attrType2Str(enum aws_dynamo_attribute_type type) {
    switch (type) {
        case AWS_DYNAMO_STRING:
            return AWS_DYNAMO_JSON_TYPE_STRING;
        case AWS_DYNAMO_STRING_SET:
            return AWS_DYNAMO_JSON_TYPE_STRING_SET;
        case AWS_DYNAMO_NUMBER:
            return AWS_DYNAMO_JSON_TYPE_NUMBER;
        case AWS_DYNAMO_NUMBER_SET:
            return AWS_DYNAMO_JSON_TYPE_NUMBER_SET;
    }

    return AWS_DYNAMO_JSON_TYPE_STRING;
}
