import devemu

#print devemu.registers(self)

class IOBoard:

    def __init__(self, cfg):
        print "YEEEAAAAHH!"
        if isinstance(cfg, str):
            import json
            cfg = json.loads(cfg)
        elif not isinstance(cfg, dict):
            raise "Config has unknown format"
        self.addr = cfg['addr']
        self.num = 8
    
    def __str__(self):
        return "IOBoard: %d"%self.addr
            
    def hanlde_req(self, req):
        if self.addr != req[0]:
            return
        if req[1] == 4:
            reg = struct.unpack('H', req[2:3])
            count = struct.unpack('H', req[4:5])

    def poll(self):
        devemu.log("POLL")

device_class = IOBoard
devcls = "IOBoard"
