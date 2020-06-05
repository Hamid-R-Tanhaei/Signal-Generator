/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package main_pack;

import hid4java.HidDevice;
import hid4java.HidException;
import hid4java.HidManager;
import hid4java.HidServices;
import hid4java.HidServicesListener;
import hid4java.HidServicesSpecification;
import hid4java.ScanMode;
import hid4java.event.HidServicesEvent;
import static main_pack.MyJFrame.WriteLog;

/**
 *
 * @author Hamid
 */
public class MyHidDeviceHandler implements HidServicesListener{
//
    private boolean IsConnected = false;
    private final Integer VENDOR_ID; // = 0x16c0; //0x534c;
    private final Integer PRODUCT_ID; // = 0x05df; //0x01;
    private final int PACKET_LENGTH; // = 64;
    private static final String SERIAL_NUMBER = null;
    private HidServicesSpecification hidServicesSpecification = new HidServicesSpecification();
    private HidServices hidServices = HidManager.getHidServices(hidServicesSpecification);
    private HidDevice hidDevice;
//
    public MyHidDeviceHandler(Integer VID,Integer PID, int Data_Size){
        this.VENDOR_ID = VID;
        this.PRODUCT_ID = PID;
        this.PACKET_LENGTH = Data_Size;
    }
//
    public void Config_HID() throws HidException {
                // Configure to use custom specification
    //HidServicesSpecification hidServicesSpecification = new HidServicesSpecification();
    hidServicesSpecification.setAutoShutdown(true);
    hidServicesSpecification.setScanInterval(500);
    hidServicesSpecification.setPauseInterval(5000);
    hidServicesSpecification.setScanMode(ScanMode.SCAN_AT_FIXED_INTERVAL_WITH_PAUSE_AFTER_WRITE);

    // Get HID services using custom specification
    //HidServices hidServices = HidManager.getHidServices(hidServicesSpecification);
    hidServices.addHidServicesListener(this);
//
    // Start the services
    //MyJFrame.jTextArea_ActLog.append("Starting HID services.\n");
        WriteLog("Starting HID services...", true);
//System.out.println("Starting HID services.");
    hidServices.start();

    //    System.out.println("Enumerating attached devices...");
    //MyJFrame.jTextArea_ActLog.append("Enumerating attached devices...\n");
        WriteLog("Enumerating attached devices...", true);
    // Provide a list of attached devices
    for (HidDevice hidDevices : hidServices.getAttachedHidDevices()) {
      //System.out.println(hidDevice);
      //MyJFrame.jTextArea_ActLog.append(hidDevice + "\n");
      WriteLog(hidDevices.toString(), true);
    }
    //Connect2Device(hidDevice,hidServices);
// Integer VENDOR_ID = 0x16c0; //0x534c;
// Integer PRODUCT_ID = 0x05df; //0x01;
// int PACKET_LENGTH = 64;
// String SERIAL_NUMBER = null;

    // Open the device device by Vendor ID and Product ID with wildcard serial number
    
//while(true){
    //}
    //sleepUninterruptibly(5, TimeUnit.MINUTES);
    //hidServices.stop();
    //hidServices.shutdown();

    
    }
//
    public void Connect2Device() throws HidException {
        hidDevice = hidServices.getHidDevice(VENDOR_ID, PRODUCT_ID, SERIAL_NUMBER);
        if (hidDevice != null) {
            
            //WriteLog(String.format( "Connected to the device: VID = 0x%h, PID = 0x%h", VENDOR_ID, PRODUCT_ID), true);
            WriteLog(String.format( "Connected to: " + hidDevice.Name()), true);
            
            IsConnected = true;
            //sendMessage(hidDevice);
        }
        else{
            WriteLog("Device NOT Connected...", true);
            IsConnected = false;
        }
        
    }
//
    public void Device_Close() throws HidException{
          hidServices.stop();
          hidServices.shutdown();
    }
//
    public boolean getConnectionStatus(){
        return IsConnected;
    }
    public void InteractWithDevice(byte[] TX_message, byte[] RX_message) {
    
        if (!hidDevice.isOpen()) {
            hidDevice.open();
        }
        
        //byte[] message = new byte[PACKET_LENGTH];
        //byte data[] = new byte[PACKET_LENGTH];
        //message[0] = 0x3f; // USB: Payload 63 bytes
        //message[1] = 0x00; // Device: '#'
        //message[2] = 0x00; // Device: '#'
        //message[3] = 0x00; // INITIALISE
        int val;
        //
            //     wait(1000);
            val = hidDevice.write(TX_message, PACKET_LENGTH, (byte) 0x00);
        if (val >= 0) {
            //WriteLog("Message Sent...", true);
            } 
            if(val < 0){
            WriteLog(hidDevice.getLastErrorMessage(), true);
            } 
            // This method will now block for 500ms or until data is read
            for (int i = 0; i < RX_message.length; i++) {
                RX_message[i] = 0;
            }
            val = hidDevice.read(RX_message, 1000);
            if (val >= 0){
              // WriteLog("Message Received...", true);
            }
            if (val == -1){  
            WriteLog(hidDevice.getLastErrorMessage(), true);
            }
    }
//
    @Override
    public void hidDeviceAttached(HidServicesEvent event) {
    //System.out.println("Device attached: " + event);
    WriteLog("Device Attached... ", true);        //throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
        hidDevice = hidServices.getHidDevice(VENDOR_ID, PRODUCT_ID, SERIAL_NUMBER);
        if (hidDevice != null) {
    
            WriteLog(String.format( "Connected to: " + hidDevice.Name()), true);
            //WriteLog(String.format( "Connected to the device: VID = 0x%h, PID = 0x%h", VENDOR_ID, PRODUCT_ID), true);
            IsConnected = true;
            //sendMessage(hidDevice);
        }
        else{
            WriteLog("Device NOT Connected...", true);
            IsConnected = false;
        }
    //ConnectAgain();
    }
//
    @Override
    public void hidDeviceDetached(HidServicesEvent event) {
        //System.out.println("Device Detached...");
        WriteLog("Device Detached... ", true);
        IsConnected = false;
        //throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
//
    @Override
    public void hidFailure(HidServicesEvent event) {
        //System.out.println("Device Failure!!!");
        WriteLog("Device Failure!!!", true);
        IsConnected = false;
        //throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
//
}
