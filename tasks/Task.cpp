/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

using namespace modem_can;

Task::Task(std::string const& name)
        : TaskBase(name), buffer(500)
{
    currentLightValue = true;
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
// bool Task::startHook()
// {
//     if (! TaskBase::startHook())
//         return false;
//     return true;
// }

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

        char buff[500];
        for (int i=0;i<msg.size;i++)
        {
            buff[i] = msg.data[i];
	    buffer.push_back(msg.data[i]);
        }
        
        buff[msg.size] = 0;
        _modem_out.write(std::string(buff));


	std::vector<uint8_t> result;
	boost::circular_buffer<uint8_t>::iterator end = communication::Communication::removeBitSync(buffer,result);
	buffer.erase(buffer.begin(),end);
	
	if(result.size() == 4){
		//Assuming nurc protokoll
		bool identifier = result[0]&0x01; //fist bit identifier
		if(identifier == 1){ //message from nurc
			double heading = ((result[0] | result[1]<<8) >>1)&0x3FF;
			heading = ((heading/(double)0x3FF)*(2.0*M_PI)-M_PI);
			double depth = (result[1] | result[2]<<8)>>2&0x0F;
			depth= (depth/(double)0x0F)*10.0;
		    	base::AUVMotionCommand motion_command;
		    	motion_command.heading = heading;
		    	motion_command.z = -depth;
		    	motion_command.x_speed = (result[2]/(double)0xFF)*3.0-1.5;
		    	motion_command.y_speed = (result[2]/(double)0xFF)*3.0-1.5;
		    	_motion_command.write(motion_command);
		}else{
			std::cerr << "Got invalid package" << std::endl;
		}
	}else{
		printf("Cold not parse packet with undknown length of: %lu\n",result.size());
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
 

     base::samples::RigidBodyState position_samples;
     while(_position_samples.read(position_samples,false) != RTT::NoData && (base::Time::now() -lastSendTime).toSeconds() > _sendInterval.get()){
	lastSendTime = base::Time::now();
     	uint8_t message[4];
	message[0] = 1; //first bit is one
	uint16_t heading = ((position_samples.getYaw()+ M_PI)/(2.0*M_PI))*(double)0x3FF;
	uint8_t depth =  ((-position_samples.position[2])/10.0)*(double)0x0F;
	uint8_t posX = ((-position_samples.position[1])/100.0)*(double)0x0F; 	
	uint8_t posY = ((-position_samples.position[2])/100.0)*(double)0x0F;
	heading &= 0x3FF;
	depth &=0x0F;
	posX &=0x0F;
	posY &=0x0F;
        canbus::Message resp;
	uint32_t* p = (uint32_t*)resp.data;
	p[0] = heading & (heading<<1) & (depth<<11) & (posX<<16) & (posY<<24);
        resp.time = base::Time::now();
        resp.can_id = 0x1E1;
        resp.size = 4;
        _canOut.write(resp);
     }
}

#if 0
int Task::count1s(int number)
{
    int count = 0;
    // TODO: more than 255. in this scenario not relevant
    for (int i = 255 - 1; i >= 0; --i)
    {
        if (number & (1 << i))
        {
            count++;
        }
    }
    return count;
}
#endif
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

