/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

using namespace modem_can;

Task::Task(std::string const& name)
    : TaskBase(name)
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

// bool Task::configureHook()
// {
//     if (! TaskBase::configureHook())
//         return false;
//     return true;
// }
// bool Task::startHook()
// {
//     if (! TaskBase::startHook())
//         return false;
//     return true;
// }
void Task::updateHook()
{
    TaskBase::updateHook();

    canbus::Message msg;
    while (_canModem.read(msg) == RTT::NewData)
    {

	for(int i=0;i<msg.size;i++)
		printf("%c",msg.data[i]);
    	fflush(stdout);
    }

    canbus::Message resp;
    resp.time = base::Time::now();
    resp.can_id = 0x1E1;
    resp.size = sprintf((char*)resp.data,"-Hallo\n");
    printf("Sending\n");
    _canOut.write(resp);

}
// void Task::errorHook()
// {
//     TaskBase::errorHook();
// }
// void Task::stopHook()
// {
//     TaskBase::stopHook();
// }
// void Task::cleanupHook()
// {
//     TaskBase::cleanupHook();
// }

