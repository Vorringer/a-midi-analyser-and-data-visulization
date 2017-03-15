#encoding:UTF-8
import urllib.request
import re
import os
import socket

socket.setdefaulttimeout(6)
content=urllib.request.urlopen("http://www.gangqinpu.com/pux/list.aspx?zhuid=1&typeid=0&runsystem=0&ordersint=0&currentPage=1").read()
content=content.decode('GBK')
match=re.compile(r'(?<=共)\d*?(?=页)')
rawlv2=re.findall(match,content)
numOfPage=int(rawlv2[0])
for i in range(111,numOfPage+1):
    try:
        url="http://www.gangqinpu.com/pux/list.aspx?zhuid=1&typeid=0&runsystem=0&ordersint=0&currentPage="+str(i)
        musicContent=urllib.request.urlopen(url,data=None,timeout=5).read()
        musicContent=musicContent.decode('GBK')
        musicMatch=re.compile(r'(?<=href=).*?(?=target)')
        musicList=re.findall(musicMatch,musicContent)
        print(i)
        for ele in musicList:
            try:
                        pUrl="http://www.gangqinpu.com"+ele
                        Pcontent=urllib.request.urlopen(pUrl,data=None,timeout=5).read()
                        Pcontent=Pcontent.decode('GBK')
                        match=re.compile(r'(?<=play.asp[?]id=).*?(?=\')')
                        matchInfo=re.compile(r'(?<=; 名人名曲)[\s\S]*?(?=[<])')
                        targetList=re.findall(match,Pcontent)
                        infoList=re.findall(matchInfo,Pcontent)
                        infoList=infoList[0].replace(" ","").replace('\n',"").replace('\r',"").split("&gt;&gt;")
                        folderName=infoList[1]
                        fileName=infoList[2]+".mid"
                        
                        filePath="E://python33//music//"+folderName
                        if (not os.path.exists(filePath)):
                            os.mkdir(filePath)
                        targetUrl="http://www.gangqinpu.com"+targetList[0]
                        urllib.request.urlretrieve(targetUrl,filePath+"//"+fileName)
            except Exception as e:
                continue
    except Exception as ee:
        continue


