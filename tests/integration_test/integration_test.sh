# This file requires lsof, which is pretty standard on most linux distros, to run.
# This file will start the server, run the integration tests, and then kill the server.

# file paths
SERVER_PATH="./bin/server"
CONFIG_PATH="../conf/test_config"
ANSWER_PATH="../tests/integration_test/answer"
# start server
$SERVER_PATH $CONFIG_PATH &

sleep 1 # wait for server to start

# server info
PID=$!
IP=127.0.0.1
PORT=8080

# integration test for server

# test 1: Simple GET request
curl -s -S -H "Host: local" -H "User-Agent: Mozilla/5.0"-i $IP:$PORT/echo -X GET -o test1.txt
diff test1.txt "$ANSWER_PATH"1.txt
if [ $? -eq 0 ]; then
    echo "Test 1 passed"
else
    echo "Test 1 failed"
    kill $(lsof -t -i:$PORT)
    exit 1
fi

# test 2: Empty GET request
printf "GET /echo HTTP/1.1\r\n\r\n\r\n" | nc $IP $PORT > test2.txt
diff test2.txt "$ANSWER_PATH"2.txt
if [ $? -eq 0 ]; then
    echo "Test 2 passed"
else
    echo "Test 2 failed"
    kill -9 $PID
    exit 1
fi

# test 3: Incorrect GET request
printf "GET HTTP/1.1\r\nHost: local\r\nUser-Agent: Mozilla/5.0\r\n\r\n" | nc $IP $PORT > test3.txt
diff test3.txt "$ANSWER_PATH"3.txt
if [ $? -eq 0 ]; then
    echo "Test 3 passed"
else
    echo "Test 3 failed"
    kill $(lsof -t -i:$PORT)
    exit 1
fi

# test 4: Simple POST request
curl -s -S -H "Content-Length: 11" -i $IP:$PORT/echo -X POST -d "hello=world" -o test4.txt

diff test4.txt "$ANSWER_PATH"4.txt
if [ $? -eq 0 ]; then
    echo "Test 4 passed"
else
    echo "Test 4 failed"
    kill $(lsof -t -i:$PORT)
    exit 1
fi

# test 5: Incorrect POST request
printf "POST HTTP/1.1\r\nHost: local\r\nUser-Agent: Mozilla/5.0\r\n\r\n" | nc $IP $PORT > test6.txt
diff test6.txt "$ANSWER_PATH"5.txt # same as test 3, 400 error
if [ $? -eq 0 ]; then
    echo "Test 5 passed"
else
    echo "Test 5 failed"
    kill $(lsof -t -i:$PORT)
    exit 1
fi

# test 6: Simple PUT request
printf "PUT /api/testing/1 HTTP/1.1\r\nHost: local\r\nContent-type: text/html\r\nContent-length: 16\r\n\r\n<p>New File</p>\r\n\r\n" | nc $IP $PORT > test6.txt
diff test6.txt "$ANSWER_PATH"6.txt
if [ $? -eq 0 ]; then
    echo "Test 6 passed"
else
    echo "Test 6 failed"
    kill $(lsof -t -i:$PORT)
    exit 1
fi

# test 7: Simple DELETE request
printf "DELETE /api/testing/1 HTTP/1.1\r\nHost: local\r\n\r\n" | nc $IP $PORT > test7.txt
printf "PUT /api/testing/1 HTTP/1.1\r\nHost: local\r\nContent-type: text/html\r\nContent-length: 16\r\n\r\n<p>New File</p>\r\n\r\n" | nc $IP $PORT > test7.txt
diff test7.txt "$ANSWER_PATH"7.txt
if [ $? -eq 0 ]; then
    echo "Test 7 passed"
else
    echo "Test 7 failed"
    kill $(lsof -t -i:$PORT)
    exit 1
fi

# test 8: Blocking test for multithreading
# make requests
curl -s $IP:$PORT/sleep > /dev/null & # sleep in background
pid1=$! # get sleep pid
curl -s $IP:$PORT/echo > /dev/null &
pid2=$! # get echo pid

# get end time
wait $pid2
end2=$(date +%s%N) &
wait $pid1 
end1=$(date +%s%N)

if [ $((end1-end2)) > 0 ]; then
    echo "Test 8 passed"
else
    echo "Test 8 failed"
    kill $(lsof -t -i:$PORT)
    exit 1
fi

rm test*.txt
echo "All tests passed"
kill $(lsof -t -i:$PORT)
exit 0