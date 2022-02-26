FROM gcc:10.3

ARG DEBIAN_FRONTEND=noninteractive

RUN apt clean && apt-get update -y

RUN apt-get install -y --no-install-recommends\
                    libboost-all-dev \
                    cmake && \
    apt-get autoclean && \
    apt-get autoremove && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update -y && apt-get install -y libicu-dev libbz2-dev


RUN cd /tmp/ && \
    wget https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.tar.gz  && \
    tar xzvf boost_1_78_0.tar.gz

RUN cd /tmp/boost_1_78_0 &&./bootstrap.sh --prefix=/usr/
RUN cd /tmp/boost_1_78_0 && ./b2
RUN cd /tmp/boost_1_78_0 && ./b2 install


COPY . /usr/src/gcc_test
RUN cd /usr/src/gcc_test && ./build.sh