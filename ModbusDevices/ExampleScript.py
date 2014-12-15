import devemu

#set_poll_timer(device, tm)

#class Register:
#
#    def __init__(self):
#        self.address = 0
#        self.function = 0
#        self.__value = 0
#
#    def set_value(self, val):
#        pass
#
#    def get_value(self):
#        pass
#
#class Device:
#
#    def __init__(self):
#        self.id = 0



class DeviceExample(Device):

    poll_tm = 500

    def __init__(self, config):
        super(DeviceEx, self).__init__(self)
        self.regs = devemu.registers(self)

    def hanlde_req(self, req):
        if req[1] == 4:
            reg = struct.unpack('H', req[2:3])
            count = struct.unpack('H', req[4:5])

    def poll(self):
        devemu.log("POLL")

device_class = DeviceExample
