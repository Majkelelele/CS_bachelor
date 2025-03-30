package cp2023.solution;

import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.concurrent.Semaphore;

import cp2023.base.ComponentId;
import cp2023.base.ComponentTransfer;
import cp2023.base.DeviceId;
import cp2023.base.StorageSystem;
import cp2023.exceptions.TransferException;


public class StorageSystemImplementation implements StorageSystem {
    private  Map<DeviceId, Integer> deviceTotalSlots;
    private  Map<ComponentId, DeviceId> componentPlacement;
    private Semaphore mutex;
    //used to aquire slots on device, between prepare and perform
    private Map<DeviceId, Map<Integer,Semaphore>> deviceSlotsSemaphore;
    //used to determine which place on given device can be reserved before prepare
    private Map<DeviceId, Map<Integer,Integer>> futureSlotsEmptySemaphoreMap;
    //used to check if components is being transferred
    private Map<ComponentId, Boolean> componentsBeingTransferred;
    //used to determine if there is ANY place on device before prepare
    private Map<DeviceId, Semaphore> SemaphoreFutures;
    //used to store transfers waiting for place on device
    private BlockingQueue queueWaitingTransfers;
    //used to store transfers already found to be in cycle, which are removed before prepare
    private LinkedList<ComponentTransfer> queueInCycleTransfers;
    //used to withhold transfers that are stored in queueWaitingTransfers
    private Map<DeviceId,Map<DeviceId,Semaphore>>  graphSemaphoreMap;
    public StorageSystemImplementation(Map<DeviceId, Integer> deviceTotalSlots,
            Map<ComponentId, DeviceId> componentPlacement) {
                this.deviceTotalSlots = deviceTotalSlots;
                this.componentPlacement = componentPlacement;
                this.mutex = new Semaphore(1,true);
                calculateEmptySlots();
                createComponentsTransferred();   
                createMapSlotsFutures();
                this.queueWaitingTransfers = new BlockingQueue();
                this.queueInCycleTransfers = new LinkedList<>();
                createGraphSemaphores();
        }
//initializing semaphores
    public void createGraphSemaphores(){
        graphSemaphoreMap = new HashMap<>(deviceTotalSlots.size()+1);
        for(DeviceId deviceSource: deviceTotalSlots.keySet()){
            Map<DeviceId,Semaphore> tempMap = new HashMap<>(deviceTotalSlots.size());
            for(DeviceId deviceDest: deviceTotalSlots.keySet()){
                tempMap.put(deviceDest,new Semaphore(0,true));
            }
            graphSemaphoreMap.put(deviceSource,tempMap);
        }
        Map<DeviceId,Semaphore> tempMap = new HashMap<>(deviceTotalSlots.size());
        for(DeviceId deviceDest: deviceTotalSlots.keySet()){
            tempMap.put(deviceDest,new Semaphore(0,true));
        }
        graphSemaphoreMap.put(new DeviceId(-1),tempMap);
    }
    //initializing futures with number of free slots
    public void createMapSlotsFutures(){
        SemaphoreFutures = new HashMap<>(deviceTotalSlots.size());
        for(DeviceId device: deviceTotalSlots.keySet()) {
            SemaphoreFutures.put(device, new Semaphore(deviceTotalSlots.get(device),true));
        }

    }
//    calculating empty slots, if taken = 0, if empty = 2
    public void calculateEmptySlots() {
        deviceSlotsSemaphore = new HashMap<>(deviceTotalSlots.size());
        futureSlotsEmptySemaphoreMap = new HashMap<>(deviceTotalSlots.size());
        for(DeviceId key : deviceTotalSlots.keySet()) {
            int count = Collections.frequency(componentPlacement.values(), key);
            Map<Integer,Semaphore> tempMapDevice = new HashMap<>(deviceTotalSlots.get(key));
            Map<Integer,Integer> tempMapFutures = new HashMap<>(deviceTotalSlots.get(key));
            for(int i = 0; i < deviceTotalSlots.get(key); i++) {
                if(i < count){
                    tempMapDevice.put(i,new Semaphore(0,true));
                    tempMapFutures.put(i,0);
                }
                else {
                    tempMapDevice.put(i,new Semaphore(1,true));
                    tempMapFutures.put(i,2);
                }
            }
            this.deviceSlotsSemaphore.put(key,tempMapDevice);
            this.futureSlotsEmptySemaphoreMap.put(key,tempMapFutures);
            deviceTotalSlots.replace(key, deviceTotalSlots.get(key)-count);
        }
    }
//    None components are transferred
    public void createComponentsTransferred() {
        componentsBeingTransferred = new HashMap<>(componentPlacement.size());
        for(ComponentId key : componentPlacement.keySet()) {
            componentsBeingTransferred.put(key, false);
        }
    }
//checking exceptions and executing transfer divided into 3 types: (Device,null), (null,Device), (Device,Device)
    public void execute(ComponentTransfer transfer) throws TransferException{
        
        if(transfer.getDestinationDeviceId() == null) {
            ComponentId component = transfer.getComponentId();
            try {
                mutex.acquire();
                ExceptionsChecker.checkIllegalTransfer(transfer, mutex,deviceTotalSlots);
                ExceptionsChecker.checkDeviceDoesNotExitst(transfer, mutex, componentPlacement, deviceTotalSlots);
                ExceptionsChecker.checkComponentDoesNotExist(transfer, mutex, componentPlacement);
                ExceptionsChecker.checkComponentIsBeingTransferred(transfer, mutex, componentPlacement, componentsBeingTransferred);
                componentsBeingTransferred.replace(component, true);
                
            }
            catch (InterruptedException e) {
                throw new RuntimeException("panic: unexpected thread interruption");
            }
            finally {mutex.release();}            
            
            new ExecutorRemoval(transfer,deviceTotalSlots,componentPlacement,deviceSlotsSemaphore,
                mutex,componentsBeingTransferred, SemaphoreFutures,futureSlotsEmptySemaphoreMap,
                queueWaitingTransfers,graphSemaphoreMap,queueInCycleTransfers).execute();
        } 
        else if(transfer.getSourceDeviceId() == null) {
            ComponentId component = transfer.getComponentId();
            try {
                mutex.acquire();
                ExceptionsChecker.checkIllegalTransfer(transfer, mutex,deviceTotalSlots);
                ExceptionsChecker.checkDeviceDoesNotExitst(transfer, mutex, componentPlacement, deviceTotalSlots);
                ExceptionsChecker.checkComponentAlreadyExists(transfer, mutex, componentPlacement);
                ExceptionsChecker.checkComponentIsBeingTransferred(transfer, mutex, componentPlacement, componentsBeingTransferred);
                
                componentsBeingTransferred.put(component,true);            
            }
            catch (InterruptedException e) {
                throw new RuntimeException("panic: unexpected thread interruption");
            }
            finally {mutex.release();}            
            new ExecutorAddition(transfer,deviceTotalSlots,componentPlacement,deviceSlotsSemaphore,
                mutex,componentsBeingTransferred, SemaphoreFutures,futureSlotsEmptySemaphoreMap,
                queueWaitingTransfers,graphSemaphoreMap,queueInCycleTransfers).execute();
        } 
        else {
            ComponentId component = transfer.getComponentId();
            try{
                mutex.acquire();
                ExceptionsChecker.checkIllegalTransfer(transfer, mutex,deviceTotalSlots);
                ExceptionsChecker.checkDeviceDoesNotExitst(transfer, mutex, componentPlacement, deviceTotalSlots);
                ExceptionsChecker.checkComponentDoesNotExist(transfer, mutex, componentPlacement);
                ExceptionsChecker.checkComponentDoesNotNeedTransfer(transfer, mutex, componentPlacement);
                ExceptionsChecker.checkComponentIsBeingTransferred(transfer, mutex, componentPlacement, componentsBeingTransferred);
               
                componentsBeingTransferred.replace(component,true);
            }
            catch (InterruptedException e) {
                throw new RuntimeException("panic: unexpected thread interruption");
            }
            finally {mutex.release();} 
            new ExecutorTransfer(transfer,deviceTotalSlots,componentPlacement,deviceSlotsSemaphore,
                mutex,componentsBeingTransferred, SemaphoreFutures,futureSlotsEmptySemaphoreMap,
                queueWaitingTransfers,graphSemaphoreMap,queueInCycleTransfers).execute();
        } 
        
        

    }
   
}
