import cv2
import cv2.aruco as aruco
import time 
import serial
import math
import numpy as np
from aruco_lib import *




cap1 = cv2.VideoCapture(1)
ser = serial.Serial('/dev/ttyUSB0',9600)
#define second xbee termanial here 
count = 0
countx =0  
county = 0 
x_dest = 190 #for ID 9 
y_dest = 125

x_dest_new = 356  #for ID 10 
y_dest_new = 60  #for 2nd robot destinations

left_region = 0 
right_region = 0
threshold = 0  
set = 1 

dest_angle = 35   #for ID 10  
deviation_cali = 3 #check weather robot reached requied angle within bound 
det_aruco_list = {}
last_time=0
dest_angle_2 = 25 #for ID 9 

#detecting location left/right  
while(True):
    ret,frame = cap1.read()
	#print frame.shape
    det_aruco_list = detect_Aruco(frame)
    cur_time = int(round(time.time() * 1000))
 
 
	
    if(det_aruco_list):
            img = mark_Aruco(frame,det_aruco_list)
            robot_state = calculate_Robot_State(img,det_aruco_list)
	    x_int_send = robot_state[9][0] 
             
	    print "X : " + str(x_int_send)
            count = count + 1 
             

	    y_int_send = robot_state[9][1] 
	    print "Y :" + str(y_int_send)
            

            if(count == 2):
                if(x_int_send < x_dest):
                    left_region = 1 
                    print "Bot is in left region"
                else:
                    right_region = 1
                    print "Bot is in right region"
                break  
          #region recognized now 
            
            cv2.imshow('image',frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        
	    time.sleep(0.1)
            
            

                   

#orientation section code First aruco 
while (True):
	ret,frame = cap1.read()
	#print frame.shape
	det_aruco_list = detect_Aruco(frame)
	cur_time = int(round(time.time() * 1000))
	if(det_aruco_list):

		img = mark_Aruco(frame,det_aruco_list)
		robot_state = calculate_Robot_State(img,det_aruco_list)
		print robot_state
		
		#angle depending on right/left position of robot 
		if(left_region):
			deviation = abs(robot_state[9][2] - dest_angle_2); 
                
		if(right_region):
			deviation = abs(robot_state[9][2] - (180 + dest_angle_2))
 
		if(deviation > deviation_cali):
			ser.write('A')
		else:
			break
		#print min(robot_state.keys()), robot_state[(min(robot_state.keys()))]
		#cv2.circle(img,(255,50),1,(0,0,255),2)
		#print cur_time - last_time
		last_time = cur_time
	cv2.imshow('image',frame)
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break
        time.sleep(0.1)




#movement stage first aruco bot 
while(True):
	ret,frame = cap1.read()
	#print frame.shape
	det_aruco_list = detect_Aruco(frame)
	cur_time = int(round(time.time() * 1000))
	
        if(det_aruco_list):
	    img = mark_Aruco(frame,det_aruco_list)
            robot_state = calculate_Robot_State(img,det_aruco_list)
	       
	    x_int_send = robot_state[9][0] # x coordinate 
             
	    print "X : " + str(x_int_send)
            
	    y_int_send = robot_state[9][1] #y coordinate 
	    print "Y :" + str(y_int_send)
            

             
          #region recognized now 
            if(left_region):
                 if((x_int_send < x_dest) and (threshold == 0)):
                     ser.write('F')
                 elif((x_int_send > x_dest) and (threshold == 0)):
                     threshold = 1
                     ser.write('L')
                 elif((threshold == 1) and (y_int_send > y_dest)):
                     ser.write('F')
                 elif((threshold == 1) and (y_int_send < y_dest)):
                    break;

            if(right_region):
                 if((x_int_send > x_dest) and (threshold == 0)):
                     ser.write('F')
                 elif((x_int_send < x_dest) and (threshold == 0)):
                     threshold = 1
                     ser.write('R')
                 elif((threshold == 1) and (y_int_send > y_dest)):
                     ser.write('F')
                 elif((threshold == 1) and (y_int_send < y_dest)):
                    break;

        
	    time.sleep(0.1)
           
        cv2.imshow('image',frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        time.sleep(0.1) 
	
# Robot 1 is navigated to its destination now look for robot 2 

#reset state variables 
left_region = 0 
right_region = 0
threshold = 0  
count = 0 
#time.sleep(5)

ser2 = serial.Serial('/dev/ttyUSB1',9600)
#detecting location left/right second robot   

#detecting location left/right  
while(True):
    ret,frame = cap1.read()
	#print frame.shape
    det_aruco_list = detect_Aruco(frame)
    cur_time = int(round(time.time() * 1000))
 
 
	
    if(det_aruco_list):
            img = mark_Aruco(frame,det_aruco_list)
            robot_state = calculate_Robot_State(img,det_aruco_list)
	    x_int_send = robot_state[10][0] ;
             
	    print "X : " + str(x_int_send)
            count = count + 1 
             

	    y_int_send = robot_state[10][1] 
	    print "Y :" + str(y_int_send)
            

            if(count == 5):
                if(x_int_send < x_dest_new):
                    left_region = 1 
                    print "Bot is in left region"
                else:
                    right_region = 1
                    print "Bot is in right region"
                break  
          #region recognized now 
            
            cv2.imshow('image',frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        
	    time.sleep(0.1)
            
            

                   

#orientation section code First aruco 
while (True):
	ret,frame = cap1.read()
	#print frame.shape
	det_aruco_list = detect_Aruco(frame)
	cur_time = int(round(time.time() * 1000))
	if(det_aruco_list):

		img = mark_Aruco(frame,det_aruco_list)
		robot_state = calculate_Robot_State(img,det_aruco_list)
		print robot_state
		
		#angle depending on right/left position of robot 
		if(left_region):
			deviation = abs(robot_state[10][2] - dest_angle); 
                
		if(right_region):
			deviation = abs(robot_state[10][2] - (180 + dest_angle))
 
		if(deviation > deviation_cali):
			ser2.write('A')
		else:
			break
		#print min(robot_state.keys()), robot_state[(min(robot_state.keys()))]
		#cv2.circle(img,(255,50),1,(0,0,255),2)
		#print cur_time - last_time
		last_time = cur_time
	cv2.imshow('image',frame)
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break
        time.sleep(0.1)




#movement stage first aruco bot 
while(True):
	ret,frame = cap1.read()
	#print frame.shape
	det_aruco_list = detect_Aruco(frame)
	cur_time = int(round(time.time() * 1000))
	
        if(det_aruco_list):
	    img = mark_Aruco(frame,det_aruco_list)
            robot_state = calculate_Robot_State(img,det_aruco_list)
	       
	    x_int_send = robot_state[10][0] # x coordinate 
             
	    print "X : " + str(x_int_send)
            
	    y_int_send = robot_state[10][1] #y coordinate 
	    print "Y :" + str(y_int_send)
            

             
          #region recognized now 
            if(left_region):
                 if((x_int_send < x_dest_new) and (threshold == 0)):
                     ser2.write('F')
                 elif((x_int_send > x_dest_new) and (threshold == 0)):
                     threshold = 1
                     ser2.write('L')
                 elif((threshold == 1) and (y_int_send > y_dest_new)):
                     ser2.write('F')
                 elif((threshold == 1) and (y_int_send < y_dest_new)):
                    break;

            if(right_region):
                 if((x_int_send > x_dest_new) and (threshold == 0)):
                     ser2.write('F')
                 elif((x_int_send < x_dest_new) and (threshold == 0)):
                     threshold = 1
                     ser2.write('R')
                 elif((threshold == 1) and (y_int_send > y_dest_new)):
                     ser2.write('F')
                 elif((threshold == 1) and (y_int_send < y_dest_new)):
                    break;

        
	    time.sleep(0.1)
           
        cv2.imshow('image',frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        time.sleep(0.1) 















cap1.release()

cv2.destroyAllWindows()
