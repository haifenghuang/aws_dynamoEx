lib_LTLIBRARIES = libaws_dynamo.la
libaws_dynamo_la_SOURCES = aws.c aws_dynamo.c \
	aws_dynamo_batch_get_item.c \
	aws_dynamo_batch_write_item.c \
	aws_dynamo_create_table.c aws_dynamo_get_item.c aws_dynamo_put_item.c \
	aws_dynamo_query.c aws_dynamo_scan.c \
	aws_dynamo_update_item.c aws_iam.c http.c \
	aws_dynamo_delete_item.c \
	aws_dynamo_delete_table.c \
	aws_dynamo_describe_table.c \
	aws_dynamo_json.c \
	aws_dynamo_json.h \
	aws_dynamo_list_tables.c \
	aws_dynamo_update_table.c \
	aws_dynamo_utils.h \
	aws_kinesis.c \
	aws_kinesis_put_record.c \
	aws_iam.h \
	aws_sigv4.c \
	aws_sigv4.h \
	jsmn.c \
	sim.c \
	jsmn.h \
	http.h \
	aws_dynamo_v2_delete_item.c \
	aws_dynamo_v2_get_item.c \
	aws_dynamo_v2_put_item.c \
	aws_dynamo_v2_query.c \
	aws_dynamo_v2_scan.c \
	aws_dynamo_v2_update_item.c \
	aws_dynamo_v2_create_table.c \
	aws_dynamo_v2_list_tables.c \
	aws_dynamo_v2_delete_table.c


libaws_dynamo_la_CPPFLAGS = -I$(top_srcdir)/src
libaws_dynamo_la_LDFLAGS = -version-info 0:0:0

pkginclude_HEADERS=\
	aws_dynamo_batch_get_item.h \
	aws_dynamo_batch_write_item.h \
	aws_dynamo_create_table.h \
	aws_dynamo_delete_item.h \
	aws_dynamo_delete_table.h \
	aws_dynamo_describe_table.h \
	aws_dynamo_get_item.h \
	aws_dynamo_list_tables.h \
	aws_dynamo.h \
	aws_dynamo_put_item.h \
	aws_dynamo_query.h \
	aws_dynamo_scan.h \
	aws_dynamo_update_item.h \
	aws_dynamo_update_table.h \
	aws_kinesis.h \
	aws_kinesis_put_record.h \
	aws.h \
	aws_dynamo_v2_delete_item.h \
	aws_dynamo_v2_get_item.h \
	aws_dynamo_v2_put_item.h \
	aws_dynamo_v2_query.h \
	aws_dynamo_v2_scan.h \
	aws_dynamo_v2_update_item.h \
	aws_dynamo_v2_create_table.h \
	aws_dynamo_v2_list_tables.h \
	aws_dynamo_v2_delete_table.h


# We need to run ldconfig on some systems after installing.
# Run it here if possible, it is ok if this fails.
install-exec-hook:
	-/sbin/ldconfig || :;
