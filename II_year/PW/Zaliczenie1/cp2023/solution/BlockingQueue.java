package cp2023.solution;

import cp2023.base.ComponentTransfer;
import cp2023.base.DeviceId;

import java.util.LinkedList;

//queue used to store transfers waiting for place on device
public class BlockingQueue {
    private LinkedList<PairDevices> list;

    public class PairDevices{
        private DeviceId deviceSource;
        private DeviceId deviceDest;
        private ComponentTransfer transfer;
        public PairDevices(DeviceId deviceSource, DeviceId deviceDest, ComponentTransfer t){
            this.deviceSource = deviceSource;
            this.deviceDest = deviceDest;
            this.transfer = t;
        }
        public DeviceId getDeviceSource(){return deviceSource;}
        public DeviceId getDeviceDest(){return deviceDest;}
        public ComponentTransfer getTransfer(){return transfer;}
    }

    public BlockingQueue() {
        this.list = new LinkedList<>();
    }
    public void AddPair(DeviceId device1, DeviceId device2,ComponentTransfer t) {
        list.addLast(new PairDevices(device1,device2,t));
    }
    public LinkedList<PairDevices> getElements() {return list;}

//    finding first transfer waiting for place on deviceDest
    public PairDevices findFirstWaiting(DeviceId deviceDest) {
        PairDevices foundWaiting = null;
        for(PairDevices pair: list) {
            if(pair.deviceDest.equals(deviceDest)){
                foundWaiting = pair;
                break;
            }
        }
        return foundWaiting;
    }
//    finding first transfer of type(Device,Device) waiting in queue
    public PairDevices findFirstWaitingPair(DeviceId deviceSource, DeviceId deviceDest){
        PairDevices found = null;
        for(PairDevices pair: list) {
            if(pair.getDeviceSource().equals(deviceSource) && pair.getDeviceDest().equals(deviceDest)) {
                found = pair;
                break;
            }
        }
        return found;
    }

    public void remove(PairDevices pair) {
        list.remove(pair);
    }

    public int size() {return this.list.size();}



}
