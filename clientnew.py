#Base Python client for MEng in IoT Assignment
#consumes data from IoT Gateway
import urllib2
import numpy as np
import matplotlib.pyplot as plt

#evenly spaced time at 3ms intervals
t=np.arange(0.,30,3.)
#red dashes, blue squares, green triangles
#plt.show()
response = urllib2.urlopen('http://localhost:8080/')

resp = response.read()
print resp
print "hello"
print resp.split()
data = resp.split()
for i in data:
	print i
print "hello again"
plt.title ('Graph of Temperature versus Time(s)')
plt.ylabel('Temperature (degrees F)')
plt.xlabel('Time (s)')
time=np.arange(0.,39,3.)
for i in data: 
	plt.plot(time,data,'g^')
plt.show() 
# i think now it doesn't like the commas after the readings ?


#print resp