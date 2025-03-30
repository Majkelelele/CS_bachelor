/*
 * University of Warsaw
 * Concurrent Programming Course 2023/2024
 * Java Assignment
 *
 * Author: Konrad Iwanicki (iwanicki@mimuw.edu.pl)
 */
package cp2023.solution;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import cp2023.base.ComponentId;
import cp2023.base.DeviceId;
import cp2023.base.StorageSystem;


public final class StorageSystemFactory {
    public static StorageSystem newSystem(
            Map<DeviceId, Integer> deviceTotalSlots,
            Map<ComponentId, DeviceId> componentPlacement) throws IllegalArgumentException {
            if(deviceTotalSlots == null) throw new IllegalArgumentException("deviceTotalSlots == null");
            if(componentPlacement == null) throw new IllegalArgumentException("componentPlacement == null");
            if(deviceTotalSlots.size() == 0) throw new IllegalArgumentException("deviceTotalSlots is empty");
            for(DeviceId key : deviceTotalSlots.keySet()) {
                if(deviceTotalSlots.get(key) == 0) throw new IllegalArgumentException(key.toString() + " has capacity of 0");
                int count = Collections.frequency(componentPlacement.values(), key);
                if(deviceTotalSlots.get(key) < count) throw new IllegalArgumentException("Too many components on device: " + key.toString());
            }

        Map<DeviceId, Integer> deviceCopy = new HashMap<>();
        for(DeviceId device: deviceTotalSlots.keySet()) {
                deviceCopy.put(device, deviceTotalSlots.get(device));
        }
        Map<ComponentId, DeviceId> componentCopy = new HashMap<>();
        for(ComponentId device: componentPlacement.keySet()) {
            componentCopy.put(device, componentPlacement.get(device));
        }
        return new StorageSystemImplementation(deviceCopy, componentCopy);
    }
}
