require 'orocos'
include Orocos

ENV['PKG_CONFIG_PATH'] = "#{File.expand_path("..", File.dirname(__FILE__))}/build:#{ENV['PKG_CONFIG_PATH']}"

Orocos::CORBA.name_service = "192.168.128.50"
Orocos.initialize

Orocos::Process.spawn 'modem_test' do |p|
    can = Orocos::TaskContext.get 'can'
    driver = Orocos::TaskContext.get 'modem_test'
    Orocos.log_all_ports

    can.modem.connect_to driver.canModem
    driver.configure
    driver.start

    loop do
    sleep 0.1
    end
end

