RM=rm -f

CXXFLAGS=-g -Wall -Werror -std=c++11
LDLIBS=-lrdkafka++

SRCS=Temperature.cpp main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: TemperatureService Consumer

TemperatureService: $(OBJS)
	$(CXX) $(LDFLAGS) -o TemperatureService $(OBJS) $(LDLIBS)

Consumer: consumer.o
	$(CXX) $(LDFLAGS) -o Consumer $^ $(LDLIBS)

clean:
	$(RM) $(OBJS) consumer.o

distclean: clean
	$(RM) *~ TemperatureService TemperatureService.dSYM Consumer Consumer.dSYM
	docker-compose down
	docker image rm -f temperature-service
	docker image rm -f consumer

image: clean
	docker build . -t temperature-service
	docker build -f Dockerfile.consumer . -t consumer

run: image
	docker-compose up --scale kafka=3 --scale temperature-service=3
