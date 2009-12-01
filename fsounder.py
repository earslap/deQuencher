import urllib, urllib2, socket
import xml.dom.minidom
import cookielib
import random
import sys, os
import osc

osc.init()

#setting username and password for freesound access
#you should put your username and address between the quotes below.
username = ""
password = ""
keyword = str(sys.argv[1])
uniqueID = int(sys.argv[2])
port = 9211
ip = "127.0.0.1"
baseurl = "http://www.freesound.org"
socket.setdefaulttimeout(10)



def failed():
    print "Quitting..."
    osc.sendMsg("/freeSounder", [0, uniqueID, "noname", -1], ip, port)
    os._exit(1)
    
def success(dlfilename, chanCount):
    print "Ciao.."
    osc.sendMsg("/freeSounder", [1, uniqueID, dlfilename, chanCount], ip, port)    
    os._exit(0)
    
if((username == "") or (password == "")):
    print "You need to enter your username and password info in fsounder.py file."
    failed()

#installing the cookie handler to urllib2
cj = cookielib.LWPCookieJar()
opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
urllib2.install_opener(opener)

#logging in to freesound

print "Trying to login to Freesound..."
values = {'username' : username,
          'password' : password,
          'redirect' : '../index.php',
          'login'    : 'login',
          'autologin': '0'}

try:
    urllib2.urlopen(baseurl + "/forum/login.php", urllib.urlencode(values))
    if(urllib2.urlopen(baseurl + "/searchTextXML.php").info()['content-type'].startswith('text/xml')):
        print "Login was successful."
    else:
        print "There was an error logging into site. Check your username and password."
        failed()        
except:
    print "There was an error accessing " + baseurl
    failed()
    
#Executing the search

sValues = {'search': keyword, 
           'start': '0', 
           'searchDescriptions': '1', 
           'searchTags': '1', 
           'searchFilenames': '0', 
           'searchUsernames': '0', 
           'durationMin': '1',
           'durationMax': '20'}

sData = urllib.urlencode(sValues)

print "Performing the search with keyword: " + keyword
resultBucket = []
numResults = 0
parsed = None
chosen = None
fType = None
numChannels = None
found = False

try:
    searched = urllib2.urlopen(baseurl + "/searchTextXML.php", sData)
    parsed = xml.dom.minidom.parseString(searched.read())
    numResults = len(parsed.getElementsByTagName("sample"))
    if(numResults == 0):
        print "No results returned with the keyword: " + keyword
        failed()
    print "Free sound returned %i results. I'll choose one." % numResults
except:
    print "There was an error in searching..."
    failed()

for node in parsed.getElementsByTagName("sample"):
    resultBucket.append(int(node.getAttribute('id')))

while(len(resultBucket) > 0):
    randIndex = random.randint(0, len(resultBucket) - 1)
    print "Trying sample id: %i" % resultBucket[randIndex]
    canditate = resultBucket[randIndex]
    
    print "Getting info for file..."
    try:
        info = urllib2.urlopen(baseurl + "/samplesViewSingleXML.php?id=" + str(canditate))
        parsed = xml.dom.minidom.parseString(info.read())
    except:
        print "Unable to determine file type."
        failed()
        
    fType = str(parsed.getElementsByTagName("extension")[0].firstChild.data)
    numChannels = int(parsed.getElementsByTagName("channels")[0].firstChild.data)
    if ((fType != 'wav') and (fType != 'aiff') and (fType != 'aif')):
        print "Not a wav or aiff file, will try another."
        resultBucket.pop(randIndex)
        continue
    if ((numChannels != 1) and (numChannels != 2)):
        print "File should be mono or stereo, will try another."
        resultBucket.pop(randIndex)
        continue
    
    print "File type is: " + fType
    print "Number of channels: " + str(numChannels)
    chosen = canditate
    found = True
    break

if(found):
    print "Downloading this file..."
    try:
        farFile = urllib2.urlopen(baseurl + "/samplesDownload.php?id=" + str(chosen))
        fileName = farFile.geturl().split("/")[-1]
        if(os.path.exists("../../../freeSound/" + fileName) == False):
        	locFile = open("../../../freeSound/" + fileName, "wb")
        	locFile.write(farFile.read())
        	farFile.close()
        	locFile.close()
        	print "File %s was downloaded successfully." % fileName
        	success(fileName, numChannels)
        else:
        	print "File %s already exists. Will use that." % fileName
        	success(fileName, numChannels)
    except:
        print "There was an error downloading file."
        failed()
        
print "Could not find any suitable file for downloading."
failed()
    