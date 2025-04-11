
import cv2

import numpy as np
import os
import sys
import csv
import time
import math
import subprocess

import threading
import queue
import requests

import http.server
import socketserver
from http import server
# Configuration
PORT = 81  # Port number for the HTTP server
# jpgq = queue.Queue()

from collections import deque

# Create a deque with a maximum length of 2
jpgq = deque(maxlen=2)



width = 320  # Desired width
height = 240  # Desired height
mult = 1


class VideoCapture:

    def __init__(self, name):
        self.cap = cv2.VideoCapture(name, apiPreference=cv2.CAP_V4L2)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, width*mult)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height*mult)
        self.q = queue.Queue()
        t = threading.Thread(target=self._reader)
        t.daemon = True
        t.start()

    # read frames as soon as they are available, keeping only most recent one
    def _reader(self):
        while True:
            ret, frame = self.cap.read()
            if not ret:
                break
            if not self.q.empty():
                try:
                    self.q.get_nowait()   # discard previous (unprocessed) frame
                except queue.Empty:
                    pass
            self.q.put(frame)

    def set(self, width, height, mult=1):
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, mult*width)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, mult*height)
        

    def read(self):
        return True, self.q.get()

def get_auto_index(dataset_dir='./video_out/', dataset_name_prefix = '', data_suffix = 'jpg'):
    max_idx = 1000
    if not os.path.isdir(dataset_dir):
        os.makedirs(dataset_dir)
    for i in range(max_idx+1):
        if not os.path.isfile(os.path.join(dataset_dir, f'{dataset_name_prefix}fname_l_{i}.{data_suffix}')):
            return i
    raise Exception(f"Error getting auto index, or more than {max_idx} episodes")

def find_webcam_index(device_name):
    command = "v4l2-ctl --list-devices"
    output = subprocess.check_output(command, shell=True, text=True)
    devices = output.split('\n\n')
    
    for device in devices:
        if device_name in device:
            lines = device.split('\n')
            for line in lines:
                if "video" in line:
                    parts = line.split()
                    for part in parts:
                        if part.startswith('/dev/video'):
                            print(part)
                            return (part[10:])

webcam_index = int(find_webcam_index('C922')) #C922 #3D USB
# cap = cv2.VideoCapture(webcam_index, apiPreference=cv2.CAP_V4L2)
#use the VideoCapture class to stop it from lagging
cap = VideoCapture(webcam_index)

# Function to encode the image frames into JPEG format and store in queue
def stream_frames():
    global robot_moved
    global jpgq
    arm_down = True
    # Continuously get frames from the webcam
    while True:
        success, img = cap.read()
        h, w, _ = (img.shape)
        
        ret, jpeg = cv2.imencode('.jpg', img)
        if ret:
            # Put the encoded frame into the queue
            # jpgb = jpeg.tobytes()
            # jpgq.put(jpeg.tobytes())
            jpgq.append(jpeg.tobytes())
        
        
        
        
# HTTP Request Handler Class
class StreamHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type', 'multipart/x-mixed-replace; boundary=frame')
        self.end_headers()
        
        while True:
            try:
                # Get the JPEG frame from the queue
                success, img = cap.read()
                ret, frame = cv2.imencode('.jpg', img)
                if ret:
                # while not jpgq.empty():
                #     frame = jpgq.get()
                # if jpgq:
                #     frame = jpgq.pop()
                    self.wfile.write(b'--frame\r\n')
                    self.send_header('Content-Type', 'image/jpeg')
                    self.send_header('Content-Length', len(frame))
                    self.end_headers()
                    self.wfile.write(frame)
                    self.wfile.write(b'\r\n')
            except Exception as e:
                print(f"Error streaming frame: {e}")
                break

# Start the streaming thread
# stream_thread = threading.Thread(target=stream_frames)
# stream_thread.daemon = True
# stream_thread.start()
class StreamingServer(socketserver.ThreadingMixIn, server.HTTPServer):
    allow_reuse_address = True
    daemon_threads = True

# Start the HTTP server
class ReusableTCPServer(socketserver.TCPServer):
    allow_reuse_address = True  # Enable address reuse

# with ReusableTCPServer(("", PORT), StreamHandler) as httpd:
#     print(f"Serving HTTP stream on port {PORT}")
#     httpd.serve_forever()

with StreamingServer(("", PORT), StreamHandler) as httpd:
    print(f"Serving HTTP stream on port {PORT}")
    httpd.serve_forever()

# Keep the script running
try:
    while True:
        pass
except KeyboardInterrupt:
    print("Stopping server...")
    httpd.shutdown()
