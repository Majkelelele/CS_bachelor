package cp2023.solution;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Semaphore;

import cp2023.base.ComponentId;
import cp2023.base.ComponentTransfer;
import cp2023.base.DeviceId;
import cp2023.solution.BlockingQueue.*;

public abstract class Executor {
    protected ComponentTransfer transfer;
    protected  Map<DeviceId, Integer> deviceTotalSlots;
    protected  Map<ComponentId, DeviceId> componentPlacement;
    protected  Map<DeviceId, Map<Integer,Semaphore>> deviceSlotsSemaphore;
    protected Map<ComponentId, Semaphore> componentsSemaphore;
    protected Semaphore mutex;
    protected Map<ComponentId, Boolean> componentsBeingTransferred;
    protected Map<DeviceId, Semaphore> deviceSlotsSemaphoreFutures;
    protected Map<DeviceId, Map<Integer,Integer>> futureSlotsSemaphoreMap;
    protected BlockingQueue queueWaitingTransfers;
    protected LinkedList<ComponentTransfer> queueInCycleTransfers;
    protected   Map<DeviceId,Map<DeviceId,Semaphore>>  graphSemaphoreMap;
    public Executor(ComponentTransfer transfer,Map<DeviceId, Integer> deviceTotalSlots,
    Map<ComponentId, DeviceId> componentPlacement,  Map<DeviceId, Map<Integer,Semaphore>> deviceSlotsSemaphore,
    Semaphore mutex, Map<ComponentId, Boolean> componentsBeingTransferred,
    Map<DeviceId, Semaphore> deviceSlotsSemaphoreFutures,Map<DeviceId, Map<Integer,Integer>>futureSlotsEmptySemaphoreMap,
    BlockingQueue queue, Map<DeviceId,Map<DeviceId,Semaphore>>  graphSemaphoreMap,LinkedList<ComponentTransfer> queueInCycleTransfers) {
        this.transfer = transfer;
        this.deviceTotalSlots = deviceTotalSlots;
        this.componentPlacement = componentPlacement;
        this.deviceSlotsSemaphore = deviceSlotsSemaphore;
        this.mutex = mutex;
        this.componentsBeingTransferred = componentsBeingTransferred;
        this.deviceSlotsSemaphoreFutures = deviceSlotsSemaphoreFutures;
        this.futureSlotsSemaphoreMap = futureSlotsEmptySemaphoreMap;
        this.queueWaitingTransfers = queue;
        this.graphSemaphoreMap = graphSemaphoreMap;
        this.queueInCycleTransfers = queueInCycleTransfers;
    }
//    finding first free place on device and reserving, which is not reserved. taken = 0, reserved = 1, free = 2
    public int findFirstFreeAndTake(DeviceId device) {
        Integer count = 0;
        Map<Integer,Integer> map = futureSlotsSemaphoreMap.get(device);
        while(map.get(count) != 2) count++;
        map.replace(count,1);
        return count;
    }
//    as above but finding first Taken and freeing place.
    public int findFirstTakenAndFree(DeviceId device) {
        Integer count = 0;
        Map<Integer,Integer> map = futureSlotsSemaphoreMap.get(device);
        while(map.get(count) != 0) count++;
        map.replace(count,2);
        return count;
    }
//    finding first transfer, which waits for place on deviceDest.
//    Returning the semaphor on which the transfer is withheld
    public Semaphore firstWaitingInQueue(DeviceId deviceDest){
        PairDevices foundWaiting = queueWaitingTransfers.findFirstWaiting(deviceDest);
        if(foundWaiting == null) {
            return null;
        }
        else{
            queueWaitingTransfers.remove(foundWaiting);
            return graphSemaphoreMap.get(foundWaiting.getDeviceSource()).get(foundWaiting.getDeviceDest());
        }
    }
    public boolean findInCycle(ComponentTransfer transfer){
        return queueInCycleTransfers.contains(transfer);
    }
//checking if in queue of waiting transfers, cycle exists. It is checked everytime new transfers has to wait.
//method finds if there exists path deviceSource-deviceDest

    public boolean pathExists(DeviceId deviceSource, DeviceId deviceDest){
        List<DeviceId> alreadyVisited = new ArrayList<>();
        List<DeviceId> currentPath = new ArrayList<>();
        BooleanWrapper check = new BooleanWrapper(false);
        alreadyVisited.add(deviceSource);
        currentPath.add(deviceSource);
        for(PairDevices pair: queueWaitingTransfers.getElements()){
            if(pair.getDeviceSource().equals(deviceSource)) {
                findPath(pair.getDeviceDest(),deviceDest,check,alreadyVisited,currentPath);
            }
        }
//        if path exists we:
//        1) remove transfer from waiting queue
//        2)add transfers in path to queue of elements in detected cycles.
//        3)releasing semaphores so the transfer can enter the prepare zone
        if(check.getValue()) {
            for(int i = 0; i < currentPath.size()-1; i++) {
                DeviceId deviceS = currentPath.get(i);
                DeviceId deviceD = currentPath.get(i+1);
                PairDevices found = queueWaitingTransfers.findFirstWaitingPair(deviceS,deviceD);
                queueWaitingTransfers.remove(found);
                queueInCycleTransfers.add(found.getTransfer());
                graphSemaphoreMap.get(deviceS).get(deviceD).release();
            }
        }
        return check.getValue();
    }
//    recursive algorithm to find path using DFS
    public void findPath(DeviceId deviceSource, DeviceId deviceDest, BooleanWrapper check,
        List<DeviceId> alreadyVisited,List<DeviceId> currentPath) {
        if(!check.getValue()) {
            if(deviceSource.equals(deviceDest)) {
                currentPath.add(deviceSource);
                check.changeValue(true);
            }
            else if(!alreadyVisited.contains(deviceSource)){
                alreadyVisited.add(deviceSource);
                currentPath.add(deviceSource);
                for(PairDevices edge: queueWaitingTransfers.getElements()) {
                    if(edge.getDeviceSource().equals(deviceSource)){
                        findPath(edge.getDeviceDest(),deviceDest,check,alreadyVisited,currentPath);
                    }
                }
                if(!check.getValue()) {
                    currentPath.remove(deviceSource);
                }

            }
        }


    }


    public abstract void execute();
}
