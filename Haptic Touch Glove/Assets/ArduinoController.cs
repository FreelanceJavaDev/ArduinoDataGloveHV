using System.Threading;
using UnityEngine;

public class ArduinoController : MonoBehaviour
{
    [Tooltip("Port name of the Arduino's Serial Port.  Format is: COM#")]
    public string portName = "COM5"; //this will differ on different machines
    [Tooltip("Baud rate the serial device is using to send data in bits per second. See arduino serial for compatible baudrates.")]
    public int baudRate = 11520;

    [Tooltip("The object that interprets messages.")]
    public GameObject messageListener;
    [Tooltip("Reconnect delay in miliseconds")]
    public int reconnectDelay = 1000;

    [Tooltip("Maximum number of unread messages.")]
    public int maxUnreadMessages = 1;
    [Tooltip("Connection Message signal.")]
    public const string ARDUINO_DEVICE_CONNECTED = "__Conn__";
    [Tooltip("Disconnected Message signal,")]
    public const string ARDUINO_DEVICE_DISCONNECTED = "__Dconn__";

    private Thread thread;
    private ArduinoCommThread arduinoComm;
    
    //When enabled start connection, start thread.
    void OnEnable() {
        arduinoComm = new ArduinoCommThread(portName, baudRate, reconnectDelay, maxUnreadMessages);
        thread = new Thread(new ThreadStart(arduinoComm.RunForever));
        thread.Start();
    }

    //When disabled request stop, have thread rejoin main thread.
    void OnDisable() {
        if(arduinoComm != null) {
            arduinoComm.RequestStop();
            arduinoComm = null;
        }

        if(thread != null) {
            thread.Join();
            thread = null;
        }
    }

    //Trigger haptics on collision with object
    void OnCollisionEnter(Collision info)
    {
        Debug.Log("Hand colliding with " + info.collider.name);
        arduinoComm.WriteSerialMessage("1"); //Activate haptics
    }

    void OnCollisionStay(Collision info) { } //don't do anything

    //Release haptics on end with collision of object.
    void OnCollisionExit(Collision info)
    {
        Debug.Log("Hand done colliding with " + info.collider.name);
        arduinoComm.WriteSerialMessage("0"); //deactivate haptics
    }

    // Update is called once per frame
    void Update()
    {
      if(messageListener == null) { return; } //do nothing with no message listener.

      string recvMsg = arduinoComm.ReadSerialMessage();

      if(recvMsg == null) { return; } //no message
      switch(recvMsg) { //pass message to listener, in this case HandModel.cs
          case ARDUINO_DEVICE_CONNECTED:
            messageListener.SendMessage("OnConnectionEvent", true);
            break;
          case ARDUINO_DEVICE_DISCONNECTED:
            messageListener.SendMessage("OnConnectionEvent", false);
            break;
          default:
            messageListener.SendMessage("OnMessageArrived", recvMsg);
            break;
      }
    }


}
