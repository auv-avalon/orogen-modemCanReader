/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

using namespace modem_can;

Task::Task(std::string const& name)
    : TaskBase(name)
{
	auv.x = 0;
	auv.y = 0;
	auv.z = -1.0;
	auv.heading = 0;
	pos=0;
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
	for(int i=0;i<msg.size;i++){
		buffer[pos++] = msg.data[i];
		if(msg.data[i] == '\n'){
			if(pos==6){
				double scale = _scale.get();
				int8_t posX = msg.data[0];
				int8_t posY = msg.data[1];
				int8_t posZ = msg.data[2];
				uint8_t heading = msg.data[3];
				uint8_t checksum = msg.data[4];
				if( ((uint8_t)((uint8_t)posX + (uint8_t)posY + (uint8_t)posZ + (uint8_t)heading)) == checksum){
					auv.x = (posX/128)*scale;
					auv.y = (posY/128)*scale;
					auv.z = (posZ/128)*scale;
					auv.heading = ((double)heading)/255.0*M_PI*2.0;
				}else{
					printf("Checksum is invalid in modem Driver\n");
				}
			}
			pos = 0;
		}
		if(pos > 150) pos = 0;
	}
    }

    canbus::Message resp;
    resp.time = base::Time::now();
    resp.can_id = 0x1E1;
    resp.size = sprintf((char*)resp.data,"-Hallo\n");
    printf("Sending\n");
    _canOut.write(resp);
    
    _position_command.write(auv);


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

