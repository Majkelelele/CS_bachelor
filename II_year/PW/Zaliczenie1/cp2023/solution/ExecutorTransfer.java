package cp2023.solution;
import java.util.LinkedList;
import java.util.Map;
import java.util.concurrent.Semaphore;

import cp2023.base.ComponentId;
import cp2023.base.ComponentTransfer;
import cp2023.base.DeviceId;

//class used to transfer component from device to other device. Type (device,device).

public class ExecutorTransfer extends Executor{

    public ExecutorTransfer(ComponentTransfer transfer,Map<DeviceId, Integer> deviceTotalSlots,
        Map<ComponentId, DeviceId> componentPlacement, Map<DeviceId, Map<Integer,Semaphore>> deviceSlotsSemaphore,
        Semaphore mutex, Map<ComponentId,Boolean> componentsBeingTransferred,Map<DeviceId,
        Semaphore> deviceSlotsSemaphoreFutures,Map<DeviceId, Map<Integer,Integer>> futureSlotsSemaphoreMap,
        BlockingQueue queueWaitingTransfers, Map<DeviceId,Map<DeviceId,Semaphore>>  graphSemaphoreMap,
        LinkedList<ComponentTransfer> queueInCycleTransfers) {
        super(transfer,deviceTotalSlots,componentPlacement,deviceSlotsSemaphore,
            mutex,componentsBeingTransferred,deviceSlotsSemaphoreFutures,futureSlotsSemaphoreMap,
            queueWaitingTransfers,graphSemaphoreMap,queueInCycleTransfers);
    }
    @Override
    public void execute() {
        ComponentId component = this.transfer.getComponentId();
        DeviceId deviceSource = this.transfer.getSourceDeviceId();
        DeviceId deviceDest = this.transfer.getDestinationDeviceId();
        try{
            mutex.acquire();
//            checking if there is free place on deviceDest
            if(deviceTotalSlots.get(deviceDest) == 0) {
//                checking for path (deviceDest,DeviceSource), with transfer (deviceSource,DeviceDest), we have cycle
                boolean result = pathExists(transfer.getDestinationDeviceId(),transfer.getSourceDeviceId());
                if(!result) {
                    queueWaitingTransfers.AddPair(deviceSource,deviceDest,transfer);
                    mutex.release();
                    graphSemaphoreMap.get(deviceSource).get(deviceDest).acquire();
                    mutex.acquire();
                }
                else{
                    queueInCycleTransfers.add(transfer);
                }
            }
            else{
//              else we take place, only if we enter not from queue
                deviceTotalSlots.replace(deviceDest, deviceTotalSlots.get(deviceDest)-1);
            }
//            allowing reservation for our place
            int taken = findFirstTakenAndFree(deviceSource);
            deviceSlotsSemaphoreFutures.get(deviceSource).release();
            mutex.release();
//           reserving place
            deviceSlotsSemaphoreFutures.get(deviceDest).acquire();
            mutex.acquire();
            int futureTaken = findFirstFreeAndTake(deviceDest);
//            if in cycle we dont free place for others on our DeviceSource
            if(!findInCycle(transfer)){
                Semaphore semTransferToBeFreed = firstWaitingInQueue(deviceSource);
//                if no transfers in queue we free place for future transfers
                if(semTransferToBeFreed == null) {
                    deviceTotalSlots.replace(deviceSource, deviceTotalSlots.get(deviceSource)+1);
                }
                else {
                    semTransferToBeFreed.release();
                }
            }
            else{
                queueInCycleTransfers.remove(transfer);
            }
            mutex.release();
            transfer.prepare();
//            freeing the real place on DeviceSource
            deviceSlotsSemaphore.get(deviceSource).get(taken).release();
//            taking the real place on deviceDest
            deviceSlotsSemaphore.get(deviceDest).get(futureTaken).acquire();
            mutex.acquire();
//            taking place, we reserved earlier
            futureSlotsSemaphoreMap.get(deviceDest).replace(futureTaken,0);
            componentPlacement.remove(component);
            mutex.release();
            transfer.perform();
            mutex.acquire();
            componentsBeingTransferred.replace(component,false);
            componentPlacement.put(component, deviceDest);
            mutex.release();
        }
        catch (InterruptedException e) {
            throw new RuntimeException("panic: unexpected thread interruption");
        } 
    }
}
