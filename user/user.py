import socket
import os
from ctypes import *
import struct
import psutil
# class POINT(Structure):
#     _fields_ = [
#         ("x",c_int),
#         ("y",c_int),
#         ("z",c_int),
#         ("w",c_int),
#         ("str",c_char_p)
#     ]

# point = POINT(26,2,3,4,c_char_p(b"abcdefghijklmnopqrst"))
# print(point.x,point.y,point.str)
# bt = bytes(point)
# print(bt)

#get pid
pids = psutil.pids()
test_name = "test"
test_pid = 0
for pid in pids:
    process = psutil.Process(pid)
    process_name = process.name()
    if process_name == "test" :
        test_pid = pid
        break
print("The Process name: %s, pid: %d",test_name,test_pid)
#create socket for intercommunication
try:
    sock = socket.socket(socket.AF_NETLINK,socket.SOCK_RAW,30)
    sock.bind((100,0))
    sock.setsockopt(socket.SOL_SOCKET,socket.SO_SNDBUF,65536)
    sock.setsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF,65536)
    str = str(test_pid).encode("utf-8")
    bt = struct.pack("4I%ds"%(len(str),),len(str),0,0,os.getpid(),bytes(str))
    bt = bytes(bt)
    sock.sendto(bt,(0,0))
except OSError:
    print("socket sent error")