package cp2023.solution;
import java.util.LinkedList;
import java.util.Map;
import java.util.concurrent.Semaphore;
import cp2023.base.ComponentId;
import cp2023.base.ComponentTransfer;
import cp2023.base.DeviceId;

//class used to execute transfer of type (null, Device) - adding new component to system
public class ExecutorAddition extends Executor{
      public ExecutorAddition(ComponentTransfer transfer,Map<DeviceId, Integer> deviceTotalSlots,
        Map<ComponentId, DeviceId> componentPlacement,   Map<DeviceId, Map<Integer,Semaphore>> deviceSlotsSemaphore,
        Semaphore mutex, Map<ComponentId, Boolean> componentsBeingTransferred,Map<DeviceId,
        Semaphore> deviceSlotsSemaphoreFutures,Map<DeviceId, Map<Integer,Integer>> futureSlotsSemaphoreMap,
        BlockingQueue queueWaitingTransfers, Map<DeviceId,Map<DeviceId,Semaphore>>  graphSemaphoreMap,
        LinkedList<ComponentTransfer> queueInCycleTransfers) {
        super(transfer,deviceTotalSlots,componentPlacement,deviceSlotsSemaphore,
        mutex,componentsBeingTransferred,deviceSlotsSemaphoreFutures,futureSlotsSemaphoreMap,queueWaitingTransfers,
        graphSemaphoreMap,queueInCycleTransfers);
    }
    @Override
    public void execute() {
        ComponentId component = this.transfer.getComponentId();
        DeviceId deviceDest = this.transfer.getDestinationDeviceId();
        try{
            mutex.acquire();
            if(deviceTotalSlots.get(deviceDest) == 0) {
//                adding transfer if deviceSource = null we add deviceId(-1)
                queueWaitingTransfers.AddPair(new DeviceId(-1),deviceDest,transfer);
                mutex.release();
                graphSemaphoreMap.get(new DeviceId(-1)).get(deviceDest).acquire();
                mutex.acquire();
            }
            else{
                deviceTotalSlots.replace(deviceDest, deviceTotalSlots.get(deviceDest)-1);
            }
             deviceSlotsSemaphoreFutures.get(deviceDest).acquire();
             int futureTaken = findFirstFreeAndTake(deviceDest);
             mutex.release();
             transfer.prepare();
             deviceSlotsSemaphore.get(deviceDest).get(futureTaken).acquire();
             mutex.acquire();
             futureSlotsSemaphoreMap.get(deviceDest).replace(futureTaken,0);
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
