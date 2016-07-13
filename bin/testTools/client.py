import socket
import struct
#import logging
try:
	import simplejson
except Exception, data:
	import json as simplejson

#logger = logging.getLogger('root')

def reqbinserver(host, port, req_str, log_id = 0, timeout_usecs = 1000) :
	address = (host, port)
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try :
		s.settimeout(timeout_usecs / 1000.0)
		s.connect(address)
	
		head = struct.pack('IIII', len(req_str), log_id, 0, 0)
		ret = s.send(head)
		if ret != len(head) :
			s.close()
			return False
		send_num = 0
		while send_num < len(req_str) :
			ret = s.send(req_str[send_num:])
			if ret <= 0 :
				s.close()
				return False
			send_num += ret
        
		head = s.recv(16)
		body_len, log_id, tmp1, tmp2 = struct.unpack('IIII', head)
		if body_len == 0:
			data = ''
		else :
			recved_data = []
			recved_len = 0
			while recved_len < body_len:
				data = s.recv(body_len - recved_len)
				if len(data) <= 0:
					s.close()
					return False
				recved_data.append(data)
				recved_len += len(data)
			data = ''.join(recved_data)
		s.close()
		return data
	except Exception, e:
		pass
	s.close()
	return False


if __name__ == "__main__":
	import time
	import json
	import sys
	import random
    
	if len(sys.argv) != 4:
		print "Usage:%s <serv_host> <serv_port> <req_str>\n" % sys.argv[0]
		sys.exit(1)

	host = sys.argv[1]
	port = int(sys.argv[2])
	req_str = sys.argv[3]
	
	beg = time.time()
	log_id = random.randint(0, 99999999)
	resp_str = reqbinserver(host, port, req_str, log_id)
	end = time.time()

	if resp_str != False:
		print "send_bytes: %d; recv_bytes: %d" % (len(req_str), len(resp_str))
	else:
		print "send fail"

	print "cost %f secs" % (end - beg)


