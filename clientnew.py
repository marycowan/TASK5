#Base Python client for MEng in IoT Assignment
#consumes data from IoT Gateway
import urllib2
import time;
import numpy as np
import matplotlib.pyplot as plt

currenttime = time.asctime( time.localtime(time.time()))
print "current time is : ",currenttime
response = urllib2.urlopen('http://localhost:8080/')
resp = response.read()
go=0
if go!=1:
	go =raw_input ("If you would like to view a batch of temperature readings press 1 now:")
	print "you entered:",go
elif go==1:
	print "go now"
print resp
data = resp.split()
for i in data:
	print i
plt.title ('Graph of Temperature versus Time(s)')
plt.ylabel('Temperature (degrees F)')
plt.xlabel('Time (s)')
time=np.arange(0.,39,3.)
for i in data: 
	plt.plot(time,data,'g--')
plt.show() 
