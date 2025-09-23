from ultralytics import YOLO
import cv2 as cv
import serial
import time
import numpy as np

LastTimeSent = time.time()
LastServo1, LastServo2, LastServo3, LastServo4 = 0, 0, 0, 0
LastDisparity = None
Servo1, Servo2, Servo3, Servo4 = 0, 0, 0, 0
data = 1
Model = YOLO(r"C:\Users\LENOVO\Downloads\best (3).pt")
Pyserial = serial.Serial("COM4", 115200)
Class_Names = Model.names
side = 0
curr = 4095
Counts = {
    "Road1": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road2": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road3": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road4": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
}

Video = cv.VideoCapture(1)

while True:
    Flag, Frame = Video.read()
    if not Flag:
        break
    Frame = cv.flip(Frame, 1)
    try:
        line = Pyserial.readline().decode(errors="ignore").strip()
        if line:
            parts = line.split(",")
            if len(parts) == 4:
                data, side, curr, road = map(int, parts)
    except:
        pass

    Roads = {
        "Road3": (0, 275, 150, 250),
        "Road4": (320, 430, 0, 223),
        "Road1": (330, 640, 145, 255),
        "Road2": (290, 430, 223, 600),
    }

    for Road in Counts:
        for Vehicle in Counts[Road]:
            Counts[Road][Vehicle] = 0

    Results = Model(Frame, conf=0.9, iou=0.5)
    if Results[0].boxes.data is not None:
        Boxes = Results[0].boxes.xyxy.cpu()
        Class_Indices = Results[0].boxes.cls.int().cpu().tolist()
        Confidences = Results[0].boxes.conf.cpu()
        for Box, Class_Indice, TensorConfidence in zip(
            Boxes, Class_Indices, Confidences
        ):
            Confidence = float(TensorConfidence)
            X1, Y1, X2, Y2 = map(int, Box)
            CircleX = (X1 + X2) / 2
            CircleY = (Y1 + Y2) / 2
            Class = Class_Names[Class_Indice]
            for Road, (XMin, XMax, YMin, YMax) in Roads.items():
                if XMin <= CircleX <= XMax and YMin <= CircleY <= YMax:
                    Counts[Road][Class] += 1

    Servo1 = 1 if Counts["Road1"]["Ambulance"] > 0 else 0
    Servo2 = 1 if Counts["Road2"]["Ambulance"] > 0 else 0
    Servo3 = 1 if Counts["Road3"]["Ambulance"] > 0 else 0
    Servo4 = 1 if Counts["Road4"]["Ambulance"] > 0 else 0

    RoadPoints = {}
    for Road, Vehicles in Counts.items():
        RoadPoints[Road] = (
            Vehicles["Ambulance"] * 10 * data
            + Vehicles["Firetruck"] * 12 * data
            + Vehicles["Car"]
        )

    Road1Points = RoadPoints["Road1"] + RoadPoints["Road3"]
    Road2Points = RoadPoints["Road2"] + RoadPoints["Road4"]
    Disparity = Road1Points - Road2Points

    if (
        Servo1 != LastServo1
        or Servo2 != LastServo2
        or Servo3 != LastServo3
        or Servo4 != LastServo4
        or LastDisparity != Disparity
    ):
        Pyserial.write(f"{Disparity},{Servo1},{Servo2},{Servo3},{Servo4}\n".encode())
        LastServo1, LastServo2, LastServo3, LastServo4 = Servo1, Servo2, Servo3, Servo4
        LastDisparity = Disparity
        LastTimeSent = time.time()

    cv.imshow("Vid Feed", Frame)
    if cv.waitKey(1) & 0xFF == ord("g"):
        break

cv.destroyAllWindows()
