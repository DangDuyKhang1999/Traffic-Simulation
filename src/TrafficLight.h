#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

// Define an enum for traffic light phases
enum TrafficLightPhase
{
    red,
    green
};

// Define a class "MessageQueue" for sending and receiving traffic light phases
template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

// Define a class "TrafficLight" as a child class of TrafficObject
class TrafficLight : public TrafficObject
{
public:
    // Constructor and destructor
    TrafficLight();

    // Getter for current traffic light phase
    TrafficLightPhase getCurrentPhase();

    // Methods
    void waitForGreen();
    void simulate() override;

private:
    // Private methods
    void cycleThroughPhases();

    // Private members
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _messageQueue;
};

#endif
