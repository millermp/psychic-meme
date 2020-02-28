FROM centos:8 as build
#COPY confluent.repo /etc/yum.repos.d/
#RUN rpm --import https://packages.confluent.io/rpm/5.4/archive.key \
#    && yum update -y \
#    && yum groupinstall "Development Tools" -y \
#    && yum install curl which librdkafka-devel -y \
#    && yum clean -y all

RUN yum update -y \
    && yum groupinstall "Development Tools" -y \
    && yum install curl which unzip -y \
    && yum clean -y all

WORKDIR /librdkafka-build
RUN curl -OL https://github.com/edenhill/librdkafka/archive/v1.3.0.tar.gz
RUN tar xzf v1.3.0.tar.gz
WORKDIR /librdkafka-build/librdkafka-1.3.0
RUN ./configure --prefix=/usr && make && make install

RUN DESTDIR=/tmp/librdkafka make install 
WORKDIR /tmp/librdkafka
RUN tar cvzf /librdkafka-build/librdkafka-1.3.0/librdkafka-1.3.0.tar.gz *

COPY . /src
WORKDIR /src
RUN make all

FROM centos:8

COPY --from=build /src/TemperatureService /bin/
COPY --from=build /src/Consumer /bin/
COPY --from=build /librdkafka-build/librdkafka-1.3.0/librdkafka-1.3.0.tar.gz /

RUN tar xvzf /librdkafka-1.3.0.tar.gz
ENV LD_LIBRARY_PATH=/usr/lib

CMD ["/bin/TemperatureService"]
