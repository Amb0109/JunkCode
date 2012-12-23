'''
Created on 2012-2-15

@author: Amb
'''

import socket
import thread
import select

print 'Initializing...'
sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
sock.connect(("127.0.0.1", 2626))
print 'Already connected to the server! ( ":q" for quit )'

def sendthd():
    while True:
        msg = raw_input()
        sock.send(msg)
        if msg == ':q':
            break
        
def recvthd():
    while True:
        msg = sock.recv(1024)
        if msg == ':q':
            print 'Bye!'
            break
        print msg
        
thread.start_new_thread(sendthd, ())

recvthd()
    
    