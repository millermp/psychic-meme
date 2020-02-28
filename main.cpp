#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include <librdkafka/rdkafkacpp.h>

#include "Temperature.h"

#define RATE 800 // milliseconds

const std::string topic = "temperature-service";

void processInput();
void update();
void render(Temperature* temperature);
const std::string getXmlString(Temperature* temperature);

class DeliveryReportCallback : public RdKafka::DeliveryReportCb
{
public:
    void dr_cb(RdKafka::Message &message) {
        if (message.err()) {
            std::cerr << "% Message delivery failed: " << message.errstr() << std::endl;
        } else {
            std::cerr << "% Message delivered to topic " << message.topic_name() <<
                " [" << message.partition() << "] at offset " <<
                message.offset() << std::endl;
        }
    }
};

int main()
{
    Temperature* bodyTemp = new Temperature(98.6);

/****************************/
    // Create the kafka configuration object
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;

    // Bootstrap broker will get the full list of brokers from the cluster
    if (conf->set("bootstrap.servers", "kafka:9092", errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << errstr << std::endl;
        exit(1);
    }

    // Set the delivery report callback. Called once per message
    DeliveryReportCallback deliveryReportCallback;
    if (conf->set("dr_cb", &deliveryReportCallback, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << errstr << std::endl;
        exit(1);
    }

    // Create producer
    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    delete conf;

    // Write to the producer
    std::string xml = getXmlString(bodyTemp);

/****************************/
    while (true) {
        processInput();
/****************************/
        retry:
            RdKafka::ErrorCode err = producer->produce(topic, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY, const_cast<char *>(xml.c_str()), xml.size(), NULL, 0, 0, NULL, NULL);

            if (err != RdKafka::ERR_NO_ERROR) {
                std::cerr << "% Failed to produce to topic " << topic << ": " << 
                    RdKafka::err2str(err) << std::endl;
                
                if (err == RdKafka::ERR__QUEUE_FULL) {
                    producer->poll(1000);
                    goto retry;
                }
            } else {
                std::cerr << "% Enqueued message (" << xml.size() << " bytes) for topic " << topic << std::endl;
            }
        
        producer->poll(0);
/****************************/
        update();
        render(bodyTemp);

        // TODO: Make this more resilient when drift becomes an issue
        std::this_thread::sleep_for(std::chrono::milliseconds(RATE));
    }
    delete bodyTemp;
    delete producer;
    return 0;
}

void render(Temperature* temperature) 
{
    // std::cout << "<?xml version=\"1.0\" standalone=\"yes\" ?><temperature>" << temperature->getTemperature() << "</temperature>";
    // std::cout << std::endl;
}

const std::string getXmlString(Temperature* temperature)
{
    std::string xml = "<?xml version=\"1.0\" standalone=\"yes\" ?><temperature>";
    xml.append(std::to_string(temperature->getTemperature()));
    xml.append("</temperature>");
    return xml;
}

void processInput() {}
void update() {}