package cp2023.solution;
import java.util.LinkedList;
import java.util.Map;
import java.util.concurrent.Semaphore;

import cp2023.base.ComponentId;
import cp2023.base.ComponentTransfer;
import cp2023.base.DeviceId;
//class used to remove component from system. Type (device,null).
public class ExecutorRemoval extends Executor{

    public ExecutorRemoval(ComponentTransfer transfer,Map<DeviceId, Integer> deviceTotalSlots,
        Map<ComponentId, DeviceId> componentPlacement,Map<DeviceId, Map<Integer,Semaphore>> deviceSlotsSemaphore,
        Semaphore mutex, Map<ComponentId,Boolean> componentsBeingTransferred,
        Map<DeviceId, Semaphore> deviceSlotsSemaphoreFutures,Map<DeviceId, Map<Integer,Integer>> futureSlotsSemaphoreMap,
        BlockingQueue queueWaitingTransfers, Map<DeviceId,Map<DeviceId,Semaphore>>  graphSemaphoreMap,
        LinkedList<ComponentTransfer> queueInCycleTransfers) {
        super(transfer,deviceTotalSlots,componentPlacement,deviceSlotsSemaphore,
        mutex,componentsBeingTransferred,deviceSlotsSemaphoreFutures,futureSlotsSemaphoreMap,queueWaitingTransfers,
        graphSemaphoreMap,queueInCycleTransfers);
    }
    @Override
    public void execute() {
        ComponentId component = this.transfer.getComponentId();
        DeviceId deviceSource = this.transfer.getSourceDeviceId();
        try{
            mutex.acquire();
            int i = findFirstTakenAndFree(deviceSource);
            deviceSlotsSemaphoreFutures.get(deviceSource).release();
            Semaphore semTransferToBeFreed = firstWaitingInQueue(deviceSource);
//          if no transfers in queue we free place for future transfers
            if(semTransferToBeFreed == null) {
                deviceTotalSlots.replace(deviceSource, deviceTotalSlots.get(deviceSource)+1);
            }
            else {
                semTransferToBeFreed.release();
            }
            mutex.release();
            transfer.prepare();
            mutex.acquire();
            componentPlacement.remove(component);
//            freeing real place
            deviceSlotsSemaphore.get(deviceSource).get(i).release();
            mutex.release();
            transfer.perform();
            componentsBeingTransferred.replace(component,false);
        }
        catch (InterruptedException e) {
            throw new RuntimeException("panic: unexpected thread interruption");
        }
        
    }
}
