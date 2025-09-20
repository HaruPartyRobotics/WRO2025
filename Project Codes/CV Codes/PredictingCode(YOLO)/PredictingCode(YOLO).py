from ultralytics import YOLO
import cv2 as cv
import serial
import time
import easyocr
import numpy as np
import gspread
from google.oauth2.service_account import Credentials

LastTimeSent = time.time()
LastParkingCheck = time.time()
lastpositions = {}
violations = {}
data = 1
Model = YOLO(r"C:\Users\LENOVO\Downloads\best (2).pt")
reader = easyocr.Reader(["en", "bn"])
Pyserial = serial.Serial("COM4", 115200)
grace = 20
Class_Names = Model.names
lastseentime = time.time()
side = 0
curr = 4095
greenroad = None
greenroad1 = None
currentroad = None
Counts = {
    "Road1": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road2": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road3": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
    "Road4": {"Ambulance": 0, "Firetruck": 0, "Car": 0},
}

Video = cv.VideoCapture(3)

scope = [
    "https://www.googleapis.com/auth/spreadsheets",
    "https://www.googleapis.com/auth/drive",
]
creds = Credentials.from_service_account_file(
    r"C:\Users\LENOVO\Downloads\dumb-potatoes-wro2025-48f0c720ee71.json", scopes=scope
)
client = gspread.authorize(creds)
sheet = client.open("Dumb Potatoes WRO2025").sheet1

LastSheetUpdate = time.time()

while True:
    Flag, Frame = Video.read()
    line = Pyserial.readline().decode(errors="ignore").strip()
    if line:
        parts = line.split(",")
        if len(parts) == 4:
            data, side, curr, road = map(int, parts)
            if road == 0:
                greenroad = "Road1"
                greenroad1 = "Road3"
            elif road == 1:
                greenroad = "Road2"
                greenroad1 = "Road4"
    if not Flag:
        break
    Frame = cv.flip(Frame, 1)
    Frame = cv.resize(Frame, (640, 640))
    Roads = {
        "Road1": (0, 223, 250, 355),
        "Road2": (250, 390, 85, 223),
        "Road3": (417, 640, 245, 345),
        "Road4": (260, 395, 417, 640),
    }
    for Road in Counts:
        for Vehicle in Counts[Road]:
            Counts[Road][Vehicle] = 0
    Results = Model(Frame, conf=0.1, iou=0.5)
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
    if (time.time() - LastTimeSent) >= 5:
        Pyserial.write(f"{(Disparity)}\n".encode())
        LastTimeSent = time.time()
    print(f"Disparity:  {Disparity}")
    print(f"Side:  {side}")
    print(f"Time Left:  {curr}")
    print(f"Road {greenroad} is Green Now.")
    cv.rectangle(Frame, (0, 250), (223, 355), (255, 255, 0), 2)
    cv.rectangle(Frame, (250, 0), (390, 223), (255, 255, 0), 2)
    cv.rectangle(Frame, (640, 245), (417, 345), (255, 255, 0), 2)
    cv.rectangle(Frame, (260, 640), (395, 417), (255, 255, 0), 2)
    results = reader.readtext(Frame)
    visibletexts = set()
    for bbox, text, prob in results:
        boxes = np.array(bbox, dtype=np.int32)
        visibletexts.add(text)
        cv.polylines(Frame, [boxes], isClosed=True, color=(0, 0, 255), thickness=2)
        cv.putText(
            Frame,
            f"Text:  {text},  Confidence:  {prob}",
            (boxes[0][0], boxes[0][1] - 5),
            cv.FONT_HERSHEY_COMPLEX,
            0.6,
            (255, 0, 0),
            2,
        )
        cx = int(np.mean(boxes[:, 0]))
        cy = int(np.mean(boxes[:, 1]))
        for Road, (XMin, XMax, YMin, YMax) in Roads.items():
            if XMin <= cx <= XMax and YMin <= cy <= YMax:
                currentroad = Road
                break
        if text not in lastpositions:
            lastpositions[text] = {
                "pos": (cx, cy),
                "timer": 0,
                "missingtimer": 0,
                "lastseen": lastseentime,
                "road": currentroad,
            }
        if time.time() - LastParkingCheck >= 10:
            lastposx, lastposy = lastpositions[text]["pos"]
            dx = abs(lastposx - cx)
            dy = abs(lastposy - cy)
            if dx <= 10 and dy <= 10:
                lastpositions[text]["timer"] += 10
            lastpositions[text]["pos"] = (cx, cy)
            LastParkingCheck = time.time()
        currenttime = time.time()
        for text in list(lastpositions.keys()):
            if (
                lastpositions[text]["road"] == greenroad
                or lastpositions[text]["road"] == greenroad1
            ):
                if text not in visibletexts:
                    lastpositions[text]["missingtimer"] = currenttime - lastpositions[
                        text
                    ].get("lastseen", currenttime)
                    if lastpositions[text]["missingtimer"] >= grace:
                        violations[text] = {
                            "stalled_time": lastpositions[text]["timer"]
                        }
                        del lastpositions[text]
                else:
                    lastpositions[text]["missingtimer"] = 0
                    lastpositions[text]["lastseen"] = currenttime

    if (time.time() - LastSheetUpdate) >= 10:
        combined = {}
        for car_id, info in lastpositions.items():
            combined[car_id] = info["timer"]
        for car_id, info in violations.items():
            if car_id not in combined:
                combined[car_id] = info["stalled_time"]

        sheet_data = [
            [car_id, stalled_time] for car_id, stalled_time in combined.items()
        ]
        sheet.clear()
        if sheet_data:
            sheet.update("A1", [["Car ID", "Stalled Time"]])
            sheet.update("A2", sheet_data)

        LastSheetUpdate = time.time()

    cv.imshow("Vid Feed", Frame)
    if cv.waitKey(1) & 0xFF == ord("g"):
        break
cv.destroyAllWindows()
