from ultralytics import YOLO
import cv2 as cv
import serial as pyserial

vid = cv.VideoCapture(0)
direction = None
rspeed, lspeed, uspeed, dspeed = 0, 0, 0, 0
Pyserial = pyserial.Serial("COM4", 115200)
neutral_speed = 1500
Model = YOLO(r"C:\Users\LENOVO\Downloads\best (3).pt")
Class_Names = Model.names
detected = 0
X1min, X1max, Y1min, Y1max = 0, 0, 0, 0
X2min, X2max, Y2min, Y2max = 0, 0, 0, 0
X3min, X3max, Y3min, Y3max = 0, 0, 0, 0
X4min, X4max, Y4min, Y4max = 0, 0, 0, 0


def coord(xmaxf, xminf, xmaxt, xmint, ymaxf, yminf, ymaxt, ymint):
    xmax = (xmaxf - xminf + xmaxt - xmint) / 2
    xmin = (xmaxf - xminf - xmaxt + xmint) / 2
    ymax = (ymaxf - yminf + ymaxt - ymint) / 2
    ymin = (ymaxf - yminf - ymaxt + ymint) / 2
    return xmax, xmin, ymax, ymin


def constrain(val, min, max):
    if val < min:
        return min
    elif val > max:
        return max
    else:
        return val


while True:
    flag, frame = vid.read()
    if not flag:
        break
    height, width = frame.shape[:2]
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
            if Class == "Ambulance" and (
                (X1min <= cx <= X1max and Y1min <= cy <= Y1max)
                or (X2min <= cx <= X2max and Y2min <= cy <= Y2max)
                or (X3min <= cx <= X3max and Y3min <= cy <= Y3max)
                or (X4min <= cx <= X4max and Y4min <= cy <= Y4max)
            ):
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
                fdiagonal = (width**2 + height**2) ** 0.5
                if xmin > X1:
                    rspeed = (1700 / fdiagonal) * (xmin - X1)
                    rspeed = constrain(rspeed, 1300, 1700)
                else:
                    rspeed = neutral_speed
                if xmax < X2:
                    lspeed = (1700 / fdiagonal) * (X2 - xmax)
                    lspeed = constrain(lspeed, 1300, 1700)
                else:
                    lspeed = neutral_speed
                if ymin > Y1:
                    dspeed = (1700 / fdiagonal) * (ymin - Y1)
                    dspeed = constrain(dspeed, 1300, 1700)
                else:
                    dspeed = neutral_speed
                if ymax < Y2:
                    uspeed = (1700 / fdiagonal) * (Y2 - ymax)
                    uspeed = constrain(uspeed, 1300, 1700)
                else:
                    uspeed = neutral_speed
        if not detected:
            uspeed = neutral_speed
            rspeed = neutral_speed
            lspeed = neutral_speed
            dspeed = neutral_speed
        Pyserial.write(
            f"{int(rspeed)} {int(lspeed)} {int(dspeed)} {int(uspeed)}\n".encode()
        )
    cv.imshow(" ", frame)
    if cv.waitKey(1) & 0xFF == ord("q"):
        break
cv.destroyAllWindows()
