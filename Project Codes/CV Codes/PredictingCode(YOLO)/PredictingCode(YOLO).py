from ultralytics import YOLO
import cv2 as cv
import serial
import time

LastTimeSent = 0
Port = "COM5"  # Actual Port
Camera = 0  # Actual Number To Capture Video Using OpenCV
Model = YOLO(r"C:\Users\LENOVO\runs\detect\train55\weights\best.pt")
Pyserial = serial.Serial(Port, 115200)
Class_Names = Model.names
Counts = {
    "Road1": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road2": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road3": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road4": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
}

Video = cv.VideoCapture(Camera)
while True:
    Flag, Frame = Video.read()
    if not Flag:
        break
    Frame = cv.flip(Frame, 1)
    Road1_X1, Road1_X2, Road1_Y1, Road1_Y2 = 0, 0, 0, 0  # Real Road1 Coords
    Road2_X1, Road2_X2, Road2_Y1, Road2_Y2 = 0, 0, 0, 0  # Real Road2 Coords
    Road3_X1, Road3_X2, Road3_Y1, Road3_Y2 = 0, 0, 0, 0  # Real Road3 Coords
    Road4_X1, Road4_X2, Road4_Y1, Road4_Y2 = 0, 0, 0, 0  # Real Road4 Coords
    Roads = {
        "Road1": (Road1_X1, Road1_X2, Road1_Y1, Road1_Y2),
        "Road2": (Road2_X1, Road2_X2, Road2_Y1, Road2_Y2),
        "Road3": (Road3_X1, Road3_X2, Road3_Y1, Road3_Y2),
        "Road4": (Road4_X1, Road4_X2, Road4_Y1, Road4_Y2),
    }
    for Road in Counts:
        for Vehicle in Counts[Road]:
            Counts[Road][Vehicle] = 0
    Results = Model(Frame)
    if Results[0].boxes.data is not None:
        Boxes = Results[0].boxes.xyxy.cpu()
        Class_Indices = Results[0].boxes.cls.int().cpu().tolist()
        Confidences = Results[0].boxes.conf.cpu()
        for Box, Class_Indice, TensorConfidence in zip(
            Boxes, Class_Indices, Confidences
        ):
            Confidence = float(TensorConfidence)
            if Confidence > 0.80:
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
                        Counts[Road][Class] += 1
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
    CurrentTime = time.time()
    if (CurrentTime - LastTimeSent) >= 5:
        Pyserial.write(f"{(Disparity)}\n".encode())
        LastTimeSent = CurrentTime
    if Pyserial.in_waiting > 0:
        data = Pyserial.readline().decode().strip()
    cv.imshow("Video", Frame)
    if cv.waitKey(1) & 0xFF == ord("g"):
        break
Video.release()
cv.destroyAllWindows()
