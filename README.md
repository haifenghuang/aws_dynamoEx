aws_dynamo
==========

AWS DynamoDB Library for C and C++ with my extension and some bug fixes.

Features
========

* Supports all DynamoDB operations (v1 and v2 protocols)
* A flexible, efficient, and fast API for accessing DynamoDB
  from within C applications.
* Supports obtaining AWS credentials from an IAM Role,
  environment variables or initialization parameters

My Extension
============

* Fix 'calloc' function parameters order problem.
* Fix memory leak when calling DynamoDB API multiple times.
* Add DynamoDB v2 protocols response parsing.
* Much simpler API calling for the DynamoDB v2 protocols.
* DynamoDB v2 response parsing is much simpler to understand(no parse state and no need for providing a template).
* Fix DynamoDB 'query' and 'scan' bugs if response's 'count' attribute is placed before 'items' attribute.


Supported Systems
=================

This library has been developed and tested on GNU/Linux.  That said,
this library attempts to be portable to wherever the dependacies
listed below are available.  Patches to increase portability or
reports of portability successes or failures are appreciated.

Dependencies
============

* libcurl - for http support, http://curl.haxx.se/libcurl/
* libyajl - for json parsing, http://lloyd.github.io/yajl/
* openssl - for general purpose crypto functions, https://www.openssl.org/

Building
========

In addition to development headers for the libraries listed above the build
depends on autoconf, automake, and libtool.

```
$ ./autogen.sh
$ ./configure
$ make
```

If you want to enable verbose debugging messages (this is only appropriate for
development) then pass the '--enable-debug' option to 'configure'.

Then, to run tests:
```
$ make -j check
```

To install the library:
```
$ sudo make install
```

Basic Usage
===========

~~For DynamoDB v2 support the library no longer provides an in-memory representation of the response.  Instead, users of the library need to parse the JSON response on their own.  See ./examples/v2-example.c~~

For v1 the library parses the response and creates an in-memory
representation of the response.

See the examples/ subdirectory for detailed examples.

Get item attributes from DynamoDB.  Assume we have a table named
'users' with a string hash key and attributes 'realName'
and 'lastSeen'.

```c
	struct aws_handle *aws_dynamo;
	struct aws_dynamo_v2_get_item_response *r = NULL;

	const char *request = 
"{\
	\"TableName\":\"Thread\",\
	\"Key\":{\
		\"ForumName\":{\"S\":\"Amazon DynamoDB\"}, \
		\"Subject\":{\"S\":\"How do I update multiple items?\"}, \
	},\
	\"ProjectionExpression\":\"LastPostDateTime, Message"\
}";

	aws_dynamo = aws_init(aws_access_key_id, aws_secret_access_key);

	r = aws_dynamo_v2_get_item(aws_dynamo, request);

	if (r == NULL) {
		return -1;
	}

	if (r->item.attributes == NULL) {
		/* No item found. */
		return;
	}

	struct aws_dynamo_attribute *lastPostDateTime = &(r->item.attributes[0]);
	struct aws_dynamo_attribute *message = &(r->item.attributes[1]);

	printf("message = %s\n", message->value.string);
	printf("lastPostDateTime = %f\n", lastPostDateTime->value.number.value.double_val);

	aws_dynamo_free_v2_get_item_response(r);
```

Notes
=====

In all cases the caller is responsible for creating the request json.  This
requires some understanding of the AWS DynamoDB API.  The v2 API is documented
here:

http://docs.aws.amazon.com/amazondynamodb/latest/developerguide/operationlist.html

~~As discussed above when the v2 API is used the caller is required to parse the JSON response.~~

Documentation for the v1 DynamoDB API can be found here:

http://aws.amazon.com/archives/Amazon-DynamoDB

See "Docs: Amazon DynamoDB (API Version 2011-12-05)"

See section, "Operations in Amazon DynamoDB" starting on page 331 for details
on creating DynamoDB JSON requests.

In many cases the caller also provides a template for the structure where the
response will be written. They then accesses the response attributes directly
using known array indices. For the V2 API you don't need to provide a template.

The response json is parsed in all cases and returned to the caller in a C
structure that must be free'd correctly when the caller is finished with it.

