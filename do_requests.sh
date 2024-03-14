#!/bin/sh

echo "Fill posts"
echo "----------"
CURL_VERBOSITY=$1

set -x
curl http://0.0.0.0:18080/api/posts $CURL_VERBOSITY -X POST --json '{"id":0,"title": "title1","content": "my personal content"}'; echo
curl http://0.0.0.0:18080/api/posts $CURL_VERBOSITY -X POST --json '{"id":0,"title": "title2","content": "my personal content"}'; echo
curl http://0.0.0.0:18080/api/posts $CURL_VERBOSITY -X POST --json '{"id":0,"title": "title3","content": "my personal content"}'; echo

echo "Show Posts"
echo "----------"
set -x
curl http://0.0.0.0:18080/api/posts $CURL_VERBOSITY; echo
set +x

echo "Change Post"
echo "----------"
echo "before"
echo "----------"
set -x
curl http://0.0.0.0:18080/api/posts/1 $CURL_VERBOSITY; echo
set +x
echo "change..."
echo "----------"

set -x
curl http://0.0.0.0:18080/api/posts -X PUT $CURL_VERBOSITY --json '{"id":1,"title": "a changed title","content": "also the content was changed"}'; echo
set +x
echo "after"
echo "----------"
set -x
curl http://0.0.0.0:18080/api/posts/1 $CURL_VERBOSITY; echo
set +x
echo "Delete Post"
echo "----------"
echo "before"
echo "----------"
set -x
curl http://0.0.0.0:18080/api/posts $CURL_VERBOSITY; echo
set +x
echo "delete"
echo "----------"
set -x
curl http://0.0.0.0:18080/api/posts/1 -X DELETE $CURL_VERBOSITY; echo
set +x
echo "after"
echo "----------"
set -x
curl http://0.0.0.0:18080/api/posts $CURL_VERBOSITY; echo
set +x

echo "login"
echo "----------"
set -x
curl http://0.0.0.0:18080/api/login $CURL_VERBOSITY; echo
set +x

echo "----------"
set -x
curl -u frank:frank http://0.0.0.0:18080/api/login $CURL_VERBOSITY; echo
set +x

echo "----------"
set -x
curl -u frank:frank http://0.0.0.0:18080/api/do_authenticated $CURL_VERBOSITY; echo
set +x

