#include <iostream>
#include <memory>
#include <mutex>

using namespace std;
using namespace std::chrono_literals;

struct PeriodicTimer
{
    template <typename F>
    PeriodicTimer(F f) {}

    template <typename T>
    void Start(T t) {}
};

class SensorAcquisition
{
public:
    // interface for clients
    const SomeComplexDataStructure& Data() const { /* ... */ }
    // interface for SensorNetwork
    template <typename Handler>
    void Scan(Handler h) { /* ... */ }
};

class SensorNetwork
{
public:
    SensorNetwork() : 
        timer( [this](){ OnTimerExpired(); } )
    {
        timer.Start(10s);
    }
    shared_ptr<SensorAcquisition> GetLastMeasure() const
    {
        lock_guard<mutex> lock(mtx);
        return current;
    }
private:
    void OnTimerExpired()
    {
        filling = make_shared<SensorAcquisition>();
        filling->Scan([this](){ OnScanCompleted(); });
    }
    void OnScanCompleted()
    {
        lock_guard<mutex> lock(mtx);        
        current = filling;
    }

    PeriodicTimer timer;
    shared_ptr<SensorAcquisition> filling;
    shared_ptr<SensorAcquisition> current;
    mutable mutex mtx; // protect "current"
};

class Client
{
public:
    Client(const SensorNetwork& sn) : sensors(sn) {}

    // possibly executed in another thread
    void DoSomeWork()
    {
        auto measure = sensors.GetLastMeasure();
        cout << measure->Data() << endl;
    }
private:
    const SensorNetwork& sensors;
};

int main()
{
    SensorNetwork sn;
    Client client(sn);

    return 0;
}