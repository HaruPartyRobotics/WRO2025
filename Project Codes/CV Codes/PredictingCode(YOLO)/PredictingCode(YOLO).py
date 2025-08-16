from ultralytics import YOLO
import cv2 as cv
import time
import serial

Port = "COM5"
Camera = 0  # Actual DroidCam's Number To Capture Video Using OpenCV
Model_File_Number = 0  # The Number After "train" In The Trained Model's Directory
Model = YOLO(rf"C:\Users\LENOVO\runs\detect\train{Model_File_Number}\weights\best.pt")
Pyserial = serial.Serial(Port, 115200)
Class_Names = Model.names
Road1_X1, Road1_X2, Road1_Y1, Road1_Y2 = 0, 0, 0, 0  # Real Road1 Coords
Road2_X1, Road2_X2, Road2_Y1, Road2_Y2 = 0, 0, 0, 0  # Real Road2 Coords
Road3_X1, Road3_X2, Road3_Y1, Road3_Y2 = 0, 0, 0, 0  # Real Road3 Coords
Road4_X1, Road4_X2, Road4_Y1, Road4_Y2 = 0, 0, 0, 0  # Real Road4 Coords
Min_Interval1 = 3000
Max_Interval1 = 15000
Min_Interval2 = 3000
Max_Interval2 = 15000
Counts = {
    "Road1": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road2": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road3": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road4": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
}
Counted_IDs = {
    "Road1": {"Ambulance": set(), "Firetruck": set(), "Car": set()},
    "Road2": {"Ambulance": set(), "Firetruck": set(), "Car": set()},
    "Road3": {"Ambulance": set(), "Firetruck": set(), "Car": set()},
    "Road4": {"Ambulance": set(), "Firetruck": set(), "Car": set()},
}
Roads = {
    "Road1": (Road1_X1, Road1_X2, Road1_Y1, Road1_Y2),
    "Road2": (Road2_X1, Road2_X2, Road2_Y1, Road2_Y2),
    "Road3": (Road3_X1, Road3_X2, Road3_Y1, Road3_Y2),
    "Road4": (Road4_X1, Road4_X2, Road4_Y1, Road4_Y2),
}
Default_Timer1, Default_Timer2 = 10000, 10000
Road1Point = Road2Point = Road3Point = Road4Point = 0
Roads1Point = Roads2Point = Disparity = 0
Interval1 = Default_Timer1
Interval2 = Default_Timer2


def millis():
    return int(time.monotonic() * 1000)


Start_Time = millis()


def constrain(Val, Min, Max):
    if Val < Min:
        return Min
    elif Val > Max:
        return Max
    else:
        return Val


def Reset():
    global Road1Points, Road2Points, Disparity, Interval1, Interval2
    Road1Points = Road2Points = Disparity = Interval1 = Interval2 = 0
    for Road in Counts:
        for Vehicle in Counts[Road]:
            Counts[Road][Vehicle] = 0


Constant = 100
Video = cv.VideoCapture(Camera)
while True:
    Flag, Frame = Video.read()
    if not Flag:
        break
    Results = Model.track(source=Frame, persist=True, tracker="bytetrack.yaml")
    if Results[0].boxes.data is not None:
        Boxes = Results[0].boxes.xyxy.cpu()
        Class_Indices = Results[0].boxes.cls.int().cpu().tolist()
        Track_IDs = Results[0].boxes.id.int().cpu().tolist()
        Confidences = Results[0].boxes.conf.cpu()
        for Box, Class_Indice, TensorConfidence, Track_ID in zip(
            Boxes, Class_Indices, Confidences, Track_IDs
        ):
            Confidence = float(TensorConfidence)
            if Confidence > 0.6:
                X1, Y1, X2, Y2 = map(int, Box)
                CircleX = (X1 + X2) / 2
                CircleY = (Y1 + Y2) / 2
                Class = Class_Names[Class_Indice]
                for Road, (XMin, XMax, YMin, YMax) in Roads.items():
                    if XMin <= CircleX <= XMax and YMin <= CircleY <= YMax:
                        cv.rectangle(Frame, (X1, Y1), (X2, Y2), (255, 255, 0), 2)
                        cv.putText(
                            Frame,
                            f"Class: {Class}   Confidence: {Confidence*100:.2f}%",
                            (X1, Y1 - 5),
                            cv.FONT_HERSHEY_COMPLEX,
                            0.6,
                            (0, 0, 255),
                            2,
                        )
                        if Track_ID not in Counted_IDs[Road][Class]:
                            Counts[Road][Class] += 1
                            Counted_IDs[Road][Class].add(Track_ID)
    RoadPoints = {}
    for Road, Vehicles in Counts.items():
        RoadPoints[Road] = (
            Vehicles["Ambulance"] * 10 + Vehicles["Firetruck"] * 12 + Vehicles["Car"]
        )
    Road1Points = RoadPoints["Road1"] + RoadPoints["Road3"]
    Road2Points = RoadPoints["Road2"] + RoadPoints["Road4"]
    Disparity = Road1Points - Road2Points
    Interval1 = Default_Timer1 - Constant * Disparity
    Interval2 = Default_Timer2 + Constant * Disparity
    Interval1 = constrain(Interval1, Min_Interval1, Max_Interval1)
    Interval2 = constrain(Interval2, Min_Interval2, Max_Interval2)
    Pyserial.write(str(Disparity).encode())
    if (millis() - Start_Time) > (Interval1 + Interval2):
        Reset()
        Start_Time = millis()
    cv.imshow("Video", Frame)
    if cv.waitKey(1) & 0xFF == ord("g"):
        break
Video.release()
cv.destroyAllWindows()
