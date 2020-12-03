using System;
using System.Collections;
using System.IO;
using System.IO.Ports;
using System.Threading;
using UnityEngine;

public class ArduinoCommThread
{
    private string portName; //COM5
    private int baudRate; //115200
    private int reconnectDelay;
    private int messageQueueSize; //1 for serial
    private SerialPort comPort;
    private const int readTimeout = 11; //8 for 120fps, 11 for 90 fps
    private const int writeTimeout = 11; //8 for 120fps, 11 for 90 fps
    private Queue readQueue, writeQueue;
    private bool stopRequest = false;
    private bool sendStatusMessages = false;
    private bool dropOldestMessage = true;
    //create Communication thread for reading and writing
    public ArduinoCommThread(string portName, int baudRate, int reconnectDelay, int messageQueueSize) {
        this.portName = portName;
        this.baudRate = baudRate;
        this.reconnectDelay = reconnectDelay;
        this.messageQueueSize = messageQueueSize;
        readQueue = Queue.Synchronized(new Queue());
        writeQueue = Queue.Synchronized(new Queue());
    }

    //Read serial messsages recived from Queue.
    public string ReadSerialMessage() {
        if (readQueue.Count == 0) { return null; }
        return (string)readQueue.Dequeue();
    }

    //Write serial message to queue
    public void WriteSerialMessage(string message) {
        writeQueue.Enqueue(message);
    }
    
    //makes a stop request, thread locked
    public void RequestStop() {
        lock(this) { stopRequest = true; }
    }

    //Runs forever, attemps to get connection then calls run once
    public void RunForever() {
        try { //debug Log for unexpected error
             while(!IsStopRequested()) {
                try {
                    AttemptConnection();
                    while(!IsStopRequested()) { RunOnce(); }
                } 
                catch(Exception ioe) {
                    Debug.LogWarning("Exception: " + ioe.Message + " in " + ioe.StackTrace);
                    readQueue.Enqueue(ArduinoController.ARDUINO_DEVICE_DISCONNECTED);
                    ClosePort();
                    Thread.Sleep(reconnectDelay);
                }
            }
            while(writeQueue.Count != 0) {
                string writeMessage = (string)(writeQueue.Dequeue());
                comPort.WriteLine(writeMessage);
            }

        } catch(Exception e) { Debug.LogError("Unknown exception: " + e.Message + " " + e.StackTrace); }
    }


    //try and open connection
    private void AttemptConnection() {
        if (comPort == null)
        {
            comPort = new SerialPort(portName, baudRate);
            comPort.ReadTimeout = readTimeout;
            comPort.WriteTimeout = writeTimeout;
            comPort.Open();
            if (sendStatusMessages)
            { readQueue.Enqueue(ArduinoController.ARDUINO_DEVICE_CONNECTED); }
        }
    }

    //close connection
    private void ClosePort() {
        if(comPort == null) { return; }    
        try { comPort.Close(); } catch (IOException) {}
        comPort = null;
    }

    //check if stop has been requested
    private bool IsStopRequested() {
        lock(this) { return stopRequest; }
    }

   //check if anything to read or write
   //used inside of RunForever
    private void RunOnce() {
        try {
            if(writeQueue.Count > 0) {
                comPort.WriteLine((string)(writeQueue.Dequeue()));
            }
            string recvMessage = comPort.ReadLine();
            if(recvMessage != null){
                if (readQueue.Count < messageQueueSize) { readQueue.Enqueue(recvMessage); }
                else
                {
                    object droppedMsg;
                    if (dropOldestMessage)
                    {
                        droppedMsg = readQueue.Dequeue();
                        readQueue.Enqueue(recvMessage);
                    }
                    else { droppedMsg = recvMessage; }
                }
            }
        } catch (TimeoutException) {
            //just means nothing to read
        }
    }
}
