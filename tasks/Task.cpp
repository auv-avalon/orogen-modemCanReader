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
    modemData = fopen("modemMessages.txt","w");
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

    int biterrortest_size = 128;
    fprintf(modemData,"#%i\n",base::Time::now().toSeconds());

    while (_canModem.read(msg) == RTT::NewData)
    {
        fprintf(modemData, "Modem hat daten\n");
        char buff[500];
        for (int i=0;i<msg.size;i++)
        {
            fprintf(modemData,"%i\n",msg.data[i]);
            buff[i] = msg.data[i];
        }
        buff[msg.size] = 0;
        _modem_out.write(std::string(buff));

        fflush(modemData);

        /*
        for (int i=0;i<msg.size;i++) {
            buffer[pos++] = msg.data[i];
            char buff[5];
            buff[0] = msg.data[i];
            buff[1] = 0;
            _modem_out.write(std::string(buff));

            if (msg.data[i] == '\n') {
                if (pos==biterrortest_size-1) { // bit error test
                    uint8_t received[biterrortest_size];
                    for (int i=0; i<biterrortest_size; i++) {
                        received[i]=buffer[i];
                    }
                    // compare
                    int errorbitscount = 0;
                    for (int i=0; i<biterrortest_size; i++) {
                        // not(a and b)
                        int cmp = ~(((int)received[i]) & i);
                        errorbitscount += count1s(cmp);
                    }

                    double errorrate = ((errorbitscount / biterrortest_size) * 8.0) * 100.0;
                    std::cout << "Biterrortest: ERRORRATE IS " << errorrate << " PERCENT." << std::endl;
                }

                printf("Modem: Pos is: %i\n",pos);
                pos = 0;
            } else {
                printf("No newline detected char is: 0x%02x\n",msg.data[i]);
            }
                if(pos > 450){
                fprintf(stderr,"Cirtical catched overflow\n");
                pos = 0;
        }
        */

    }

    std::string string;
    while (_modem_in.read(string) == RTT::NewData) {
        char buff[5000];
        canbus::Message resp;
        resp.time = base::Time::now();
        resp.can_id = 0x1E1;
        resp.size = sprintf((char*)resp.data,"%s",string.c_str());
        printf("Sending\n");
        _canOut.write(resp);
    }


    _position_command.write(auv);
}

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

