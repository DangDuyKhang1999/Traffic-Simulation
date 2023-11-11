#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_queue.empty(); });

    T message = std::move(_queue.back());
    _queue.pop_back();

    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> uLock(_mutex);

    _queue.push_back(std::move(msg));
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (_messageQueue.receive() == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

void TrafficLight::cycleThroughPhases()
{
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> uDistr(4000, 6000); // Random cycle duration between 4 and 6 seconds

    std::chrono::time_point<std::chrono::system_clock> lastUpdate = std::chrono::system_clock::now();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        int cycleDuration = uDistr(eng); // Get a random cycle duration

        if (timeSinceLastUpdate >= cycleDuration) {
            lastUpdate = std::chrono::system_clock::now();
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;

            // Send the updated phase to the message queue
            _messageQueue.send(std::move(_currentPhase));
        }
    }
}
