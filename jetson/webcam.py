import sys
import math
import cv2
import numpy as np
import serial
import time

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
arduino = serial.Serial(port='/dev/ttyUSB0', baudrate=9600, timeout=1)
time.sleep(2)
while True:
	ret, frame=cap.read()

	#binarizacion de la imagen
	src = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	src2 = cv2.GaussianBlur(src, (5, 5), 0)
	_, dst = cv2.threshold(src2,50,255,cv2.THRESH_BINARY_INV)

	#ignoramos la parte inferior de la camara para que no de vuelta antes de tiempo
	pts = np.array([[0,400],[640,400],[640,480],[0,480]], np.int32)
	pts = pts.reshape((-1,1,2))
	cv2.fillPoly(dst,[pts],0)

	#calculo de desfase
	M = cv2.moments(dst)

	#Si hay pixeles blancos en la imagen
	if M["m00"] != 0:

		cx = int(M["m10"] / M["m00"])  
		cy = int(M["m01"] / M["m00"])

		#Se dibuja un punto en la imagen correspondiente al centro
		cv2.circle(dst,(cx,cy),5,(0,0,255),-1)

		#Un ajuste para que el centro de la imagen sea 0
		giro = cx - 320
		
		#envio a arduino
		arduino.write(f"{giro}\n".encode('utf-8'))
		print(giro)

	cv2.imshow("Linea", dst)
	if cv2.waitKey(1) == 13: 
		break

cap.release() 
cv2.destroyAllWindows()
