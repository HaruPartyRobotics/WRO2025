from ultralytics import YOLO
import cv2 as cv
import serial as pyserial
import time

vid = cv.VideoCapture(2)
roll = 0
pitch = 0
rspeed, lspeed, fspeed, bspeed = 0, 0, 0, 0
Pyserial = pyserial.Serial("COM4", 115200)
neutral_speed = 1500
Model = YOLO(r"C:\Users\LENOVO\Downloads\best (3).pt")
Class_Names = Model.names
smt = time.time()
pt = 0
rl = 0
detected = 0
X1min, X1max, Y1min, Y1max = 0, 275, 150, 250
X2min, X2max, Y2min, Y2max = 320, 430, 0, 223
X3min, X3max, Y3min, Y3max = 330, 640, 145, 255
X4min, X4max, Y4min, Y4max = 290, 430, 223, 600


def coord(xmaxf, xminf, xmaxt, xmint, ymaxf, yminf, ymaxt, ymint):
    xmax = (xmaxf - xminf + xmaxt - xmint) / 2
    xmin = (xmaxf - xminf - xmaxt + xmint) / 2
    ymax = (ymaxf - yminf + ymaxt - ymint) / 2
    ymin = (ymaxf - yminf - ymaxt + ymint) / 2
    return xmax, xmin, ymax, ymin


def constrain(val, min_val, max_val):
    if val < min_val:
        return min_val
    elif val > max_val:
        return max_val
    else:
        return val


while True:
    while Pyserial.in_waiting:
        line = Pyserial.readline().decode(errors="ignore").strip()
        if line.endswith(";"):
            raw = line[:-1]
            parts = raw.split(",")
            if len(parts) == 2:
                try:
                    rl, pt = map(int, parts)
                    print(f"From Arduino -> Roll: {rl}, Pitch: {pt}")
                except ValueError:
                    continue

    flag, frame = vid.read()
    if not flag:
        break

    height, width = frame.shape[:2]
    xmid = width / 2
    ymid = height / 2
    frame = cv.flip(frame, 1)
    frame = cv.resize(frame, (640, 640))
    detected = 0
    Results = Model(frame, conf=0.1, iou=0.5)

    if Results[0].boxes.data is not None:
        Boxes = Results[0].boxes.xyxy.cpu()
        Class_Indices = Results[0].boxes.cls.int().cpu().tolist()
        Confidences = Results[0].boxes.conf.cpu()
        for Box, Class_Indice, TensorConfidence in zip(
            Boxes, Class_Indices, Confidences
        ):
            Confidence = float(TensorConfidence)
            X1, Y1, X2, Y2 = map(int, Box)
            cx = (X1 + X2) / 2
            cy = (Y1 + Y2) / 2
            Class = Class_Names[Class_Indice]
            if Class == "Ambulance":
                detected = 1
                cv.rectangle(frame, (X1, Y1), (X2, Y2), (255, 255, 0), 2)
                cv.putText(
                    frame,
                    f"Class: {Class}   Confidence: {Confidence*100:.2f}%",
                    (X1, Y1 - 5),
                    cv.FONT_HERSHEY_COMPLEX,
                    0.6,
                    (0, 0, 255),
                    2,
                )
                xmax, xmin, ymax, ymin = coord(width, 0, X2, X1, height, 0, Y2, Y1)
                if xmin > X1:
                    lspeed = (100 / width) * (xmin - X1)
                    lspeed = constrain(lspeed, -100, 100)
                else:
                    rspeed = 0
                if xmax < X2:
                    rspeed = (-100 / width) * (X2 - xmax)
                    rspeed = constrain(rspeed, -100, 100)
                else:
                    rspeed = 0
                if ymin > Y1:
                    fspeed = (-100 / height) * (ymin - Y1)
                    fspeed = constrain(fspeed, -100, 100)
                else:
                    fspeed = 0
                if ymax < Y2:
                    bspeed = (100 / height) * (Y2 - ymax)
                    bspeed = constrain(bspeed, -100, 100)
                else:
                    fspeed = 0
                if lspeed == 0:
                    roll = rspeed
                else:
                    roll = lspeed
                if fspeed == 0:
                    pitch = bspeed
                else:
                    pitch = fspeed
            if not detected:
                fspeed = 0
                rspeed = 0
                lspeed = 0
                bspeed = 0

    if time.time() - smt > 0.1:
        Pyserial.write(f"{int(pitch)},{int(roll)};".encode())
        smt = time.time()
    print(f"{pitch},{roll};")

    cv.imshow("Vid Feed", frame)
    if cv.waitKey(1) & 0xFF == ord("q"):
        break

cv.destroyAllWindows()
