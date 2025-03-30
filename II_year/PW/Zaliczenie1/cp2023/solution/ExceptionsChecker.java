package cp2023.solution;
import java.util.Map;
import java.util.concurrent.Semaphore;

import cp2023.base.ComponentId;
import cp2023.base.ComponentTransfer;
import cp2023.base.DeviceId;
import cp2023.exceptions.ComponentAlreadyExists;
import cp2023.exceptions.ComponentDoesNotExist;
import cp2023.exceptions.ComponentDoesNotNeedTransfer;
import cp2023.exceptions.ComponentIsBeingOperatedOn;
import cp2023.exceptions.DeviceDoesNotExist;
import cp2023.exceptions.IllegalTransferType;
import cp2023.exceptions.TransferException;

public class ExceptionsChecker {
    public ExceptionsChecker() {

    }
    public static void checkComponentDoesNotExist(ComponentTransfer transfer, Semaphore mutex,
    Map<ComponentId, DeviceId> componentPlacement) throws TransferException {
        ComponentId component = transfer.getComponentId();
        try{
            if(!componentPlacement.containsKey(component)) throw new ComponentDoesNotExist(component, transfer.getSourceDeviceId());
            DeviceId device = transfer.getSourceDeviceId();
            if(!componentPlacement.get(component).equals(device)) throw new ComponentDoesNotExist(component, device);
        }
        catch(ComponentDoesNotExist e) {
            mutex.release();
            throw e;
        }
    }
    public static void checkComponentAlreadyExists(ComponentTransfer transfer, Semaphore mutex,
    Map<ComponentId, DeviceId> componentPlacement) throws TransferException{
         ComponentId component = transfer.getComponentId();
        try {        
            if(componentPlacement.get(component) != null) throw new ComponentAlreadyExists(component,componentPlacement.get(component));    
        }
        catch(ComponentAlreadyExists e) {
            mutex.release();
            throw e;
        }
    }
    
    public static void checkDeviceDoesNotExitst(ComponentTransfer transfer, Semaphore mutex,
    Map<ComponentId, DeviceId> componentPlacement,Map<DeviceId, Integer> deviceTotalSlots) throws TransferException{       
        try {
            if(transfer.getDestinationDeviceId() != null && 
            deviceTotalSlots.get(transfer.getDestinationDeviceId()) == null) throw new DeviceDoesNotExist(transfer.getDestinationDeviceId());
            if(transfer.getSourceDeviceId() != null  &&
            deviceTotalSlots.get(transfer.getSourceDeviceId()) == null) throw new DeviceDoesNotExist(transfer.getSourceDeviceId());  
        }
        catch(DeviceDoesNotExist e) {
            mutex.release();
            throw e;
        }
    }

    public static void checkComponentDoesNotNeedTransfer(ComponentTransfer transfer, Semaphore mutex,
    Map<ComponentId, DeviceId> componentPlacement) throws TransferException{
        ComponentId component = transfer.getComponentId();
        DeviceId device = transfer.getDestinationDeviceId();
        try{
            if(device.equals(componentPlacement.get(component))) throw new ComponentDoesNotNeedTransfer(component, device);
        }
        catch(ComponentDoesNotNeedTransfer e) {
            mutex.release();
            throw e;
        }
    }
    public static void checkComponentIsBeingTransferred(ComponentTransfer transfer, Semaphore mutex,
    Map<ComponentId, DeviceId> componentPlacement,
    Map<ComponentId, Boolean> componentsBeingTransferred) throws TransferException{
        try{
            if(componentsBeingTransferred.get(transfer.getComponentId()) != null &&
            componentsBeingTransferred.get(transfer.getComponentId()) == true) throw new ComponentIsBeingOperatedOn(transfer.getComponentId());   
        }
        catch(ComponentIsBeingOperatedOn e) {
            mutex.release();
            throw e;
        }
    }
    public static void checkIllegalTransfer(ComponentTransfer transfer, Semaphore mutex,
    Map<DeviceId, Integer> deviceTotalSlots) throws TransferException{
        try{
            // if(transfer.equals(null)) throw new IllegalTransferType(tra);
            // if(transfer.getDestinationDeviceId() != null &&
            //   deviceTotalSlots.get(transfer.getDestinationDeviceId()) == 0) throw new IllegalTransferType(transfer.getComponentId());
            if(transfer.getDestinationDeviceId() == null
           && transfer.getSourceDeviceId() == null) throw new IllegalTransferType(transfer.getComponentId());
            
        }
        catch(IllegalTransferType e) {
            mutex.release();
            throw e;
        }
    }
}
