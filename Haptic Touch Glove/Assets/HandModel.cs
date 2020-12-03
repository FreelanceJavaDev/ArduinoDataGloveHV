﻿using System.Text.RegularExpressions;
using UnityEngine;

public class HandModel : MonoBehaviour
{
    private string[] lastOrientation; //check if orientation has changed
    private string[] lastGyro; //check if gyro has changed
    private string[] lastAccel; //check if acceleration has changed
    private string[] lastPos; //check if position has changed.
    private bool updatePos, updateGyro, updateOrientation;
    private static float AXIS_CORRECTION = -90.0F; //because of unity's 3D axis
    private Vector3 INITAL_POS = new Vector3(0.2F, 0.9F, -9.5F);

    //Handles Messages sent from Arduino Controller.
    void OnMessageArrived(string msg) {
        //Debug.Log("Message arrived: " + msg);
        parseMessage(msg);
    }

    //handles Connection event and disconnection events from Arduino Controller
    void OnConnectionEvent(bool success) {
        if(success)
        { Debug.Log("Connection Established."); }
         else { Debug.Log("Connection Terminated or failed to connect."); }
    }


   


    // Start is called before the first frame update
    //ensure string arrays are valid and set to defaults. Defualts could be null or empty
    void Start()
    {
        lastOrientation = new string[3];
        lastAccel = new string[3];
        lastGyro = new string[3];
        lastPos = new string[3];
        updateOrientation = false;
        updateGyro = false;
        updatePos = false;
    }


    // Update is called once per frame
    void Update()
    {
        updateGameObject(updatePos, "_P");
        //updateGameObject(updateGyro, "_G"); //curse you unity.  I'll deal with this later.
    }

    //format is x,y,z from sensors, translate to x,z,y
    //x->x, z->y, y->z for no particular reason other than it's Unity
    private void parseMessage(string msg) {
        string updateType = msg.Substring(0,2);
        string temp = msg.Remove(msg.LastIndexOf('_'));
        string data = temp.Remove(0, 2);
        string[] vector = new string[3];
        switch(updateType) {
            case "_O": //orientation
                vector = Regex.Split(data, ",");
                Debug.Log("Orientation update: x:" + vector[0] + ", z:" + vector[1] + ", y:" + vector[2]);
                updateOrientation = updateLastOrientation(vector);
                break;
            case "_G": //gyroscope
                vector = Regex.Split(data, ",");
                Debug.Log("Gyro Update: x:" + vector[0] + ", z:" + vector[1] + ", y:" + vector[2]);
                updateGyro = updateLastGyro(vector);
 
                break;
            case "_A": //accelerometer
                vector = Regex.Split(data, ",");
                Debug.Log("Accelerometer Update:  x:" + vector[0] + ", z:" + vector[1] + ", y:" + vector[2]);
                updateLastAcceleration(vector);
                break;
            case "_P": //position
                vector = Regex.Split(data, ",");
                Debug.Log("Pos Update:  dx:" + vector[0] + ", dz:" + vector[1] + ", dy:" + vector[2]);
                updatePos = updateLastPosition(vector);
                break;
            default: break;
        }
    }

    //Update game object parameters, more may be added.
    //Position and orientation are for the transform object.
    //position is a delta offset, must be addes to object position.
    private void updateGameObject(bool needsUpdate, string updateContext) {
        if(needsUpdate) {
            Vector3 temp;
            switch (updateContext) {
                case "_O":
                    temp.x = float.Parse(lastOrientation[0]);
                    temp.z = float.Parse(lastOrientation[1]);
                    temp.y = float.Parse(lastOrientation[2]);
                    temp.x = (temp.x < 355.0F) ? temp.x : 0;
                    temp.y = (temp.y < 355.0F) ? temp.y : 0;
                    temp.z = (temp.z < 355.0F) ? temp.z : 0;

                    temp.y += AXIS_CORRECTION;
                    temp.z += AXIS_CORRECTION;
                    this.transform.rotation.eulerAngles.Set(temp.x, temp.y, temp.z);
                    updatePos = false;
                    break;
                case "_G":
                    temp.x = float.Parse(lastGyro[0]);
                    temp.z = float.Parse(lastGyro[1]);
                    temp.y = float.Parse(lastGyro[2]);
                    temp.x = (temp.x > 1.0F || temp.x < -1.0F) ? temp.x : 0;
                    temp.y = (temp.y > 1.0F || temp.y < -1.0F) ? temp.y : 0;
                    temp.z = (temp.z > 1.0F || temp.z < -1.0F) ? temp.z : 0;
                    this.transform.Rotate(temp);
                    updateGyro = false;
                    break;
                case "_P":
                    temp.x = (float.Parse(lastPos[0]))+ INITAL_POS.x; 
                    temp.z = (float.Parse(lastPos[1])) + INITAL_POS.z;
                    temp.y = (float.Parse(lastPos[2])) + INITAL_POS.y;

                    this.transform.position = temp;
                    updatePos = false;
                    break;
                default: break;
            }

        }
    }

    //if current orientation is different update lastOrientation
    //using strings so there are no floating point comparison issues.
    //returns true if any of the componets differ.  
    //At Start() the beginning the lastOrientation is empty/null
    private bool updateLastOrientation(string[] msgData) {
        bool update = false;
        if(lastOrientation[0] == null || lastOrientation[0] == "") {
            lastOrientation[0] = msgData[0];
            lastOrientation[1] = msgData[1];
            lastOrientation[2] = msgData[2];
            return true;      
        }
        else {
            if(!lastOrientation[0].Equals(msgData[0]))  { 
                lastOrientation[0] = msgData[0];
                update = true;
            }
            if(!lastOrientation[1].Equals(msgData[1])) { 
                lastOrientation[1] = msgData[1];
                update = true;
            }
            if(!lastOrientation[2].Equals(msgData[2])) 
            { 
                lastOrientation[2] = msgData[2];
                update = true; 
            }
            return update;

        }
    }

    //if current gyorscope reading is different update lastGyro
    //using strings so there are no floating point comparison issues.
    //returns true if any of the componets differ.  
    //At Start() the beginning the lastGyro is empty/null
    private bool updateLastGyro(string[] msgData) {
        bool update = false;
        if(lastGyro[0] == null || lastGyro[0] == "") {
            lastGyro[0] = msgData[0];
            lastGyro[1] = msgData[1];
            lastGyro[2] = msgData[2];
            return true;      
        }
        else {
            if(!lastGyro[0].Equals(msgData[0]))  { 
                lastGyro[0] = msgData[0];
                update = true;
            }
            if(!lastGyro[1].Equals(msgData[1])) { 
                lastGyro[1] = msgData[1];
                update = true;
            }
            if(!lastGyro[2].Equals(msgData[2])) 
            { 
                lastGyro[2] = msgData[2];
                update = true; 
            }
            return update;

        }
    }

    //if current acceleration is different update lastAcceleration
    //using strings so there are no floating point comparison issues.
    //returns true if any of the componets differ.  
    //At Start() the beginning the lastAccleration is empty/null
    //this is a critical update, changes in accleration are part of the physics engine.
    private bool updateLastAcceleration(string[] msgData) {
        bool update = false;
        if(lastAccel[0] == null || lastAccel[0] == "") {
            lastAccel[0] = msgData[0];
            lastAccel[1] = msgData[1];
            lastAccel[2] = msgData[2];
            return true;      
        }
        else {
            if(!lastAccel[0].Equals(msgData[0]))  { 
                lastAccel[0] = msgData[0];
                update = true;
            }
            if(!lastAccel[1].Equals(msgData[1])) { 
                lastAccel[1] = msgData[1];
                update = true;
            }
            if(!lastAccel[2].Equals(msgData[2])) 
            { 
                lastAccel[2] = msgData[2];
                update = true; 
            }
            return update;

        }
    }

    //update lastPos, delta_x, delta_y and delta_z are all reset after being sent.
    //using strings so there are no floating point comparison issues.
    //returns true if any of the componets differ.  
    //At Start() the beginning the lastPos is empty/null
    //Since lastPos is the delta_x, delta_y, and delta_z updates should be added reguardless of value.  Even if 0, 0, 0
    private bool updateLastPosition(string[] msgData) {
        lastPos[0] = msgData[0];
        lastPos[1] = msgData[1];
        lastPos[2] = msgData[2];
        return true;
    }
}
