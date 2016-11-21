import socket
from threading import Thread
import array


BIND = ("0.0.0.0", 8000)
TARGET = ("192.168.56.1", 27000)
gamer = None
address = None

ok = False

while 1:

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.bind(BIND)
        s.listen(1)
        (gamer, address) = s.accept()
    except socket.error, err:
        print "%r" % (err, )
        raise SystemExit

    
    print 'Gamer connected %r' % str(address)

    #socks handshake
    data = gamer.recv(128)
    print data
    gamer.send(array.array('B', [5, 0]).tostring())
    data = gamer.recv(128)
    print data
    gamer.send(array.array('B', [5, 0, 1]).tostring())

    ok = True
    break    

if ok == False:
    s.close()
    gamer.close()
    print 'Handshake failed'
    exit()


while 1:

    datagram = None
    try:
        datagram = gamer.recv(256)            
    except socket.error, err:
        pass

    if datagram:
        print 'gamer: ' + datagram
        #length = len(datagram)
        #sent = target.sendto(datagram, TARGET)
        #print 'sent to qserver: %r' % sent
        #if length != sent:
        #    print 'cannot send to server %r != %r' % (length, sent)
        #    break
        


'''

s.close()

target = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
target.bind(TARGET)
gamer.setblocking(0)
target.setblocking(0)


try:
    while 1:

        datagram = None
        try:
            datagram = gamer.recv(1024)            
        except socket.error, err:
            pass

        if datagram:
            print 'gamer: ' + datagram
            length = len(datagram)
            sent = target.sendto(datagram, TARGET)
            print 'sent to qserver: %r' % sent
            if length != sent:
                print 'cannot send to server %r != %r' % (length, sent)
                break
        
        datagram = None
        try:
            datagram, addr = target.recvfrom(1024)
        except socket.error, err:
            pass

        if datagram:
            print 'qserver: %r' % datagram
            length = len(datagram)
            sent = gamer.send(datagram)
            if length != sent:
                print 'cannot send to gamer %r != %r' % (length, sent)

except:
    gamer.close()
    target.close()
    raise

'''

