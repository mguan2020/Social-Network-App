### Build/test container ###
# Define builder stage
FROM yet-another-web-server:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:jammy as deploy

# TODO(!): Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
RUN mkdir ./data
RUN mkdir ./data_static
COPY --from=builder /usr/src/project/data/test1 ./data_static
COPY --from=builder /usr/src/project/data/image ./data_static
COPY --from=builder /usr/src/project/data/html ./data_static
COPY --from=builder /usr/src/project/data/zip ./data_static
COPY --from=builder /usr/src/project/data/crud ./data/crud
COPY --from=builder /usr/src/project/temp ./temp

RUN mkdir ./another
COPY --from=builder /usr/src/project/data/test2 ./another

# Expose port 80
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# Update with real server name, which is server
ENTRYPOINT ["./server"]

# Pass deploy_config(config file) to the server as argument
COPY --from=builder /usr/src/project/conf/deploy_config .
CMD ["./deploy_config"]