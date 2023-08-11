# base image
FROM yet-another-web-server:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build_coverage

# Build and test coverage 
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage