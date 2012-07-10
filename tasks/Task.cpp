/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

using namespace modem_can;

Task::Task(std::string const& name)
        : TaskBase(name), buffer(500)
{
}



// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

// bool Task::configureHook()
// {
//     if (! TaskBase::configureHook())
//         return false;
//     return true;
// }
bool Task::startHook()
{
    if (! TaskBase::startHook())
        return false;
    currentLightValue = true;
    gotValidPos=false;
    return true;
}

void Task::updateHook()
{
    TaskBase::updateHook();

    {
      bool v;
      while(_light_value.read(v) == RTT::NewData){
	  currentLightValue = v;
      }
    }

    canbus::Message msg;
    while (_canModem.read(msg) == RTT::NewData)
    {
        //printf("Got something with size: %i\n",msg.size);
        char buff[500];
        for (int i=0;i<msg.size;i++)
        {
            buff[i] = msg.data[i];
	    buffer.push_back(msg.data[i]);
        }
        
        buff[msg.size] = 0;
        _modem_out.write(std::string(buff));


	base::AUVMotionCommand motion_command_new;
        if(communication::Communication::getMotionCommandfromMessage(buffer,motion_command_new)){
            motion_command = motion_command_new;
            gotValidPos = true;
        }
        if(gotValidPos){
             _motion_command.write(motion_command);
        }

    }

     std::string string;
     while (_modem_in.read(string) == RTT::NewData) {
         canbus::Message resp;
         resp.time = base::Time::now();
         resp.can_id = 0x1E1;
         resp.size = sprintf((char*)resp.data,"%s",string.c_str());
         printf("Sending\n");
         _canOut.write(resp);
     }
 

     while(_position_samples.read(position_samples,false) != RTT::NoData && (base::Time::now() -lastSendTime).toSeconds() > _sendInterval.get()){
	lastSendTime = base::Time::now();
        std::vector<canbus::Message> resp = communication::Communication::createPacketFromAUV(position_samples,currentLightValue);
        for(int i=0;i<resp.size();i++)
            _canOut.write(resp[i]);
     }
}

// void Task::errorHook()
// {
//     TaskBase::errorHook();
// }
// void Task::stopHook()
// {
//     TaskBase::stopHook();
// }
void Task::cleanupHook()
{
    TaskBase::cleanupHook();
    
//    fclose(modemData);
}

