/*
 * Copyright (c) 2012-2014 Devicescape Software, Inc.
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

#include <string.h>
#include "aws_dynamo.h"

#include <assert.h>

static void test_aws_dynamo_parse_get_item_response(const char *json,
	struct aws_dynamo_attribute *attributes, int num_attributes) {

	struct aws_dynamo_get_item_response *r;

	r = aws_dynamo_parse_get_item_response(json, strlen(json), attributes, num_attributes);

	aws_dynamo_dump_get_item_response(r);
	aws_dynamo_free_get_item_response(r);
}

static void test_parse_get_item_response_example(void) {
	struct aws_dynamo_attribute attributes[] = {
		{
			.type = AWS_DYNAMO_STRING_SET,
			.name = "friends",
			.name_len = strlen("friends"),
		},
		{
			.type = AWS_DYNAMO_STRING,
			.name = "status",
			.name_len = strlen("status"),
		},
		{
			.type = AWS_DYNAMO_NUMBER,
			.name = "size",
			.name_len = strlen("size"),
			.value.number.type = AWS_DYNAMO_NUMBER_DOUBLE,
		},
	};

	test_aws_dynamo_parse_get_item_response("{\"Item\": {\"friends\":{\"SS\":[\"Lynda, Aaron\"]}, \"status\":{\"S\":\"online\"},\"size\":{\"N\":\"2.5\"} }, \"ConsumedCapacityUnits\": 1 }", attributes, sizeof(attributes) / sizeof(attributes[0]));
}

static void test_parse_get_item_response(void) {
	test_parse_get_item_response_example();
}

int main(int argc, char *argv[]) {

	test_parse_get_item_response();

	return 0;
}
