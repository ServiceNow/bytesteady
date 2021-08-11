FROM images.borgy.elementai.net/eai-ocso:latest

RUN apt update && \
    apt install -y libboost-dev libboost-program-options-dev libboost-system-dev libboost-thread-dev libboost-math-dev libboost-test-dev libboost-python-dev zlib1g-dev cmake python3 python3-pip

RUN cd /usr/lib/x86_64-linux-gnu/ && ln -s libboost_python-py37.so libboost_python3.so

RUN git clone https://github.com/VowpalWabbit/vowpal_wabbit.git
RUN cd vowpal_wabbit && cmake . && make && make install
