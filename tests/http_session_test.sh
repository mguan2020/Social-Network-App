# two simple tests on GET and POST http requests
# run the script in tests dir
# the test result will be stored in build/test_result
cd ..
dir=build
if [[ ! -e $dir ]]; then
    mkdir build
    cd build
    cmake .. 
    make 
elif [[ ! -d $dir ]]; then
    echo "$dir already exists but is not a directory" 1>&2
    exit
fi
cd build
PORT=8080
echo "port $PORT;" >config
bin/server config &
echo "==========HTTP SESSION TEST==========" >test_result
for FILE in ../tests/http_requests/*; 
    do 
    echo -ne '\r\n==========Test==========\r\n' >>test_result; 
    echo -ne "$(cat $FILE)" | nc localhost $PORT >>test_result
done

kill $(lsof -t -i:$PORT)


