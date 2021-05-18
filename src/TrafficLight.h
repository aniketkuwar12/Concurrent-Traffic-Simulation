#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

enum TrafficLightPhase
{
    red,
    green
};
// forward declarations to avoid include cycle
class Vehicle;
 
template <class T>
class MessageQueue
{
public:

    T receive();
    void send(T&& msg);
    int getSize();
private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _queue;
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();
    void setCurrentPhase(TrafficLightPhase newPhase);

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:    
    // typical behaviour methods
    void cycleThroughPhases();


    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _msgQ;
};

#endif