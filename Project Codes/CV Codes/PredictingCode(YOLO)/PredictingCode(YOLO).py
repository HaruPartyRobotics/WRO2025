from ultralytics import YOLO
import cv2 as cv
import time
import serial

Pyserial = serial.Serial("COM3", 115200)
Model_File_Number = 0  # The Number After "train" In The Trained Model's Directory
Model = YOLO(rf"C:\Users\LENOVO\runs\detect\train{Model_File_Number}\weights\best.pt")
Class_Names = Model.names
Road1_X1, Road1_X2, Road1_Y1, Road1_Y2 = 0, 0, 0, 0  # Real Road1 Coords
Road2_X1, Road2_X2, Road2_Y1, Road2_Y2 = 0, 0, 0, 0  # Real Road2 Coords
Road3_X1, Road3_X2, Road3_Y1, Road3_Y2 = 0, 0, 0, 0  # Real Road3 Coords
Road4_X1, Road4_X2, Road4_Y1, Road4_Y2 = 0, 0, 0, 0  # Real Road4 Coords
Counts = {
    "Road1": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road2": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road3": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road4": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
}

Default_Timer1, Default_Timer2 = 10000, 10000
Road1Point = Road2Point = Road3Point = Road4Point = 0
Roads1Point = Roads2Point = Disparity = 0
Interval1 = Default_Timer1
Interval2 = Default_Timer2


def millis():
    return int(time.monotonic() * 1000)


Start_Time = millis()


def Reset():
    global Road1Point, Road2Point, Road3Point, Road4Point, Roads1Point, Roads2Point, Disparity, Interval1, Interval2
    Road1Point = Road2Point = Road3Point = Road4Point = Roads1Point = Roads2Point = (
        Disparity
    ) = Interval1 = Interval2 = 0
    for Road in Counts:
        for Vehicle in Counts[Road]:
            Counts[Road][Vehicle] = 0


Constant = 100
Video = cv.VideoCapture(0)
while True:
    Flag, Frame = Video.read()
    if not Flag:
        break
    Results = Model.predict(source=Frame)
    if Results[0].boxes.data is not None:
        Boxes = Results[0].boxes.xyxy.cpu()
        Class_Indices = Results[0].boxes.cls.int().cpu().tolist()
        Confidences = Results[0].boxes.conf.cpu()
        for Box, Class_Indice, TensorConfidence in zip(
            Boxes, Class_Indices, Confidences
        ):
            Confidence = float(TensorConfidence)
            if Confidence > 0.6:
                X1, Y1, X2, Y2 = map(int, Box)
                CircleX = (X1 + X2) / 2
                CircleY = (Y1 + Y2) / 2
                Class = Class_Names[Class_Indice]
                if (
                    CircleX >= Road1_X1
                    and CircleX <= Road1_X2
                    and CircleY >= Road1_Y1
                    and CircleY <= Road1_Y2
                ):
                    cv.rectangle(Frame, (X1, Y1), (X2, Y2), (255, 255, 0), 2)
                    Counts["Road1"][Class] += 1
                    cv.putText(
                        Frame,
                        f"Class: {Class}   Confidence: {Confidence * 100:.2f}%",
                        (X1, Y1 - 5),
                        cv.FONT_HERSHEY_COMPLEX,
                        0.6,
                        (0, 0, 255),
                        2,
                    )
                elif (
                    CircleX >= Road2_X1
                    and CircleX <= Road2_X2
                    and CircleY >= Road2_Y1
                    and CircleY <= Road2_Y2
                ):
                    cv.rectangle(Frame, (X1, Y1), (X2, Y2), (255, 255, 0), 2)
                    Counts["Road2"][Class] += 1
                    cv.putText(
                        Frame,
                        f"Class: {Class}   Confidence: {Confidence * 100:.2f}%",
                        (X1, Y1 - 5),
                        cv.FONT_HERSHEY_COMPLEX,
                        0.6,
                        (0, 0, 255),
                        2,
                    )
                elif (
                    CircleX >= Road3_X1
                    and CircleX <= Road3_X2
                    and CircleY >= Road3_Y1
                    and CircleY <= Road3_Y2
                ):
                    cv.rectangle(Frame, (X1, Y1), (X2, Y2), (255, 255, 0), 2)
                    Counts["Road3"][Class] += 1
                    cv.putText(
                        Frame,
                        f"Class: {Class}   Confidence: {Confidence * 100:.2f}%",
                        (X1, Y1 - 5),
                        cv.FONT_HERSHEY_COMPLEX,
                        0.6,
                        (0, 0, 255),
                        2,
                    )
                elif (
                    CircleX >= Road4_X1
                    and CircleX <= Road4_X2
                    and CircleY >= Road4_Y1
                    and CircleY <= Road4_Y2
                ):
                    cv.rectangle(Frame, (X1, Y1), (X2, Y2), (255, 255, 0), 2)
                    Counts["Road4"][Class] += 1
                    cv.putText(
                        Frame,
                        f"Class: {Class}   Confidence: {Confidence * 100:.2f}%",
                        (X1, Y1 - 5),
                        cv.FONT_HERSHEY_COMPLEX,
                        0.6,
                        (0, 0, 255),
                        2,
                    )
    Road1Ambulance = Counts["Road1"]["Ambulance"]
    Road1Firetruck = Counts["Road1"]["Firetruck"]
    Road1Car = Counts["Road1"]["Car"]
    Road2Ambulance = Counts["Road2"]["Ambulance"]
    Road2Firetruck = Counts["Road2"]["Firetruck"]
    Road2Car = Counts["Road2"]["Car"]
    Road3Ambulance = Counts["Road3"]["Ambulance"]
    Road3Firetruck = Counts["Road3"]["Firetruck"]
    Road3Car = Counts["Road3"]["Car"]
    Road4Ambulance = Counts["Road4"]["Ambulance"]
    Road4Firetruck = Counts["Road4"]["Firetruck"]
    Road4Car = Counts["Road4"]["Car"]
    Road1Point = Road1Ambulance * 10 + Road1Firetruck * 12 + Road1Car
    Road2Point = Road2Ambulance * 10 + Road2Firetruck * 12 + Road2Car
    Road3Point = Road3Ambulance * 10 + Road3Firetruck * 12 + Road3Car
    Road4Point = Road4Ambulance * 10 + Road4Firetruck * 12 + Road4Car
    Roads1Point = Road1Point + Road3Point
    Roads2Point = Road2Point + Road4Point
    Disparity = Roads1Point - Roads2Point
    Interval1 = Default_Timer1 - Constant * Disparity
    Interval2 = Default_Timer2 + Constant * Disparity
    Pyserial.write(str(Disparity).encode())
    if (millis() - Start_Time) > (Interval1 + Interval2):
        Reset()
        Start_Time = millis()
    cv.imshow("Video", Frame)
    if cv.waitKey(1) & 0xFF == ord("g"):
        break
Video.release()
cv.destroyAllWindows()
