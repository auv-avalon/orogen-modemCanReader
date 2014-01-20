require 'orocos'
include Orocos

ENV['PKG_CONFIG_PATH'] = "#{File.expand_path("..", File.dirname(__FILE__))}/build:#{ENV['PKG_CONFIG_PATH']}"

Orocos::CORBA.name_service = "192.168.128.50"
Orocos.initialize

Orocos::run 'modem_test' do
    can = Orocos::TaskContext.get 'can'
    driver = Orocos::TaskContext.get 'modem_test'
    Orocos.log_all_ports
    can.device = 'can0'
    can.configure
    can.start
   


    #can.out.connect_to driver.canModem
    driver.configure
    driver.start

    loop do
        driver.modem_in.write "TEST"
        sleep 0.1
    end
end

