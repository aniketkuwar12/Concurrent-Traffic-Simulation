#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" 
*  This is generic Message Queue which is thread safe and can be used for any object */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] {
        return !_queue.empty();
    });

    T msg = std::move(_queue.back());
    _queue.clear();

    return msg;
}


template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> uLock(_mutex);

    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight()
{

}
void TrafficLight::waitForGreen()
{
    while(true)
    {
        //std::cout <<"SIZE OF Q : "<< _msgQ.getSize() << std::endl;
        TrafficLightPhase trafficPhase = _msgQ.receive();
        if(trafficPhase == TrafficLightPhase::green)
        {
            break;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLightPhase newVal)
{
    _currentPhase = newVal;
}
void TrafficLight::simulate()
{

    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    // init stop watch
    lastUpdate = std::chrono::system_clock::now();

    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_real_distribution<> distr(4000, 6000);

    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        double cycleDuration = distr(eng);

        if (timeSinceLastUpdate >= cycleDuration)
        {
            if(getCurrentPhase() == TrafficLightPhase::green)
            {
                setCurrentPhase(TrafficLightPhase::red);
            }
            else
            {
                setCurrentPhase(TrafficLightPhase::green);
            }
            
            TrafficLightPhase msg = _currentPhase;

            _msgQ.send(std::move(msg));  //Add updated signal to queue using move semantics to transfer ownership.

            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }

    }
}

