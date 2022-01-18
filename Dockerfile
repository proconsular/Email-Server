FROM ubuntu

RUN apt-get -y update && apt-get install -y
RUN apt-get -y install gcc
RUN apt-get -y install clang
RUN apt-get -y install cmake
RUN apt-get -y install build-essential checkinstall zlib1g-dev libtemplate-perl
RUN apt-get -y install wget
RUN wget -c "https://www.openssl.org/source/openssl-1.1.1k.tar.gz"
RUN tar -xzvf "openssl-1.1.1k.tar.gz"
WORKDIR "/openssl-1.1.1k/"
RUN ["./config", "--prefix=/usr/local/ssl", "--openssldir=/usr/local/ssl shared zl"]
RUN make
RUN make test
RUN make install
RUN export PATH="/usr/local/ssl/bin:${PATH}"
RUN apt-get -y install git
RUN apt-get -y install mysql-client libmysqlcppconn-dev libmysql++-dev
RUN wget -c "https://tangentsoft.com/mysqlpp/releases/mysql++-3.3.0.tar.gz"
RUN tar -xzvf "mysql++-3.3.0.tar.gz"
WORKDIR "mysql++-3.3.0"
RUN ["./configure"]
RUN make
RUN make install

WORKDIR /app

COPY web ./web
COPY src ./src
COPY tests ./tests
COPY lib ./lib
COPY CMakeLists.txt .
COPY main.cpp .
COPY http_client.cpp .
COPY config.json .
COPY keys ./keys

RUN cmake .
RUN cmake --build . --target P11_Mail_Server_run

EXPOSE 80 443

CMD ["./P11_Mail_Server_run"]
