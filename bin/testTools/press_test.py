#!/data1/zhanghan/anaconda2/bin/python


import threading
import os
import sys
import client
import time
import json
from cost_stat import CostStat

mylock = threading.RLock()
#mylock = threading.allocate_lock()

g_cs = CostStat()
g_req_list = []
g_hostport_list = []
#g_host = ""
#g_port = 0
g_thread_cnt = 1
g_loops = 0
g_argv0 = ""
g_begin = 0
g_pid = 0

class ClientThread(threading.Thread):
	def __init__(self, id):
		threading.Thread.__init__(self)
		self.id = id
		self.stop_flag = False

	def run(self):
		self.stop_flag = False
		loop = 0
		loops = 999999999
		if g_loops > 0:
			loops = g_loops
		while loop < loops:
			if self.stop_flag == True:
				break
			for idx in range(0, len(g_req_list)):
				if idx % g_thread_cnt == self.id:  
					for hostport in g_hostport_list:
						beg = time.time()
						resp_str = client.reqbinserver(hostport[0], hostport[1], g_req_list[idx])
						end = time.time()
						mylock.acquire()
						if resp_str != False:	
							g_cs.StatSuc(len(g_req_list[idx]), len(resp_str), (end - beg) * 1000.0)
						else:
							g_cs.StatFailed(len(g_req_list[idx]), (end - beg) * 1000.0)
						mylock.release()

	def stop(self):
		self.stop_flag = True
	

class PrintThread(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.stop_flag = False
	
	def run(self):
		self.stop_flag = False
		while True:
			time.sleep(1)
			if self.stop_flag == True:
				break
			use_secs = time.time() - g_begin
			hr = use_secs / 3600
			min = use_secs % 3600 / 60
			sec = use_secs % 3600 % 60
			mylock.acquire()
			os.system("clear")
			print "[%s - %d]" % (g_argv0, g_pid)
			print "target:"
			for hostport in g_hostport_list:
				print "\t%s:%d" % (hostport[0], hostport[1]) 
			if g_loops == 0:
				print "threads: %d, loops: oo" % g_thread_cnt
			else:
				print "threads: %d, loops: %u" % (g_thread_cnt, g_loops)
			print "running time: %d hr %d min %d sec" % (hr, min, sec)	
			g_cs.Print()
			print "\n"
			mylock.release()

	def stop(self):
		self.stop_flag = True
	

def transToTextAnalyzeCMD(text,extend_topn=10,keyword_topn=5,topic_topn=3):
        req_dict = {}
        req_dict['cmd'] = 'text_analyze'
        body_dict = {}
        body_dict['extend_topn'] = extend_topn
        body_dict['keyword_topn'] = keyword_topn
        body_dict['topic_topn'] = topic_topn
        body_dict['text'] = text
        req_dict['body'] = body_dict
        return json.dumps(req_dict)


def ReadReqsFromFile(filename):
	req_list = list()
        with open(filename,'r') as fp:
                for line in fp:
                        #line = line.strip()
                        if len(line) == 0:
                                break
                        if len(line) > 3:
                                req_list.append(transToTextAnalyzeCMD(line))
	return req_list
	

if __name__ == "__main__":
	if len(sys.argv) < 4:
		print "Usage:%s <host:port,host:port...> <req_list_file> <thread_cnt> <|loops>\n" % sys.argv[0]
		sys.exit(1)
	
	g_argv0 = sys.argv[0]
	g_hostport_list = map((lambda x: (x[0], int(x[1]))), map((lambda x : x.split(":")), sys.argv[1].split(",")))
	g_req_list = ReadReqsFromFile(sys.argv[2])
	g_thread_cnt = int(sys.argv[3])
	if len(sys.argv) >= 5:
		g_loops = int(sys.argv[4])
        print g_loops

        
        #loadResourceCMD = '{"body":{"resource_name":"demeter_resource_20160621"},"cmd":"load_resource"}'
        #hostport_main = g_req_list[0]
        #loadRespStr = client.reqbinserver(hostport_main[0], hostport_main[1],loadResourceCMD,timeout_usecs=60000)
        #print loadRespStr
	threads = []
	g_begin = time.time()
	g_pid = os.getpid()
	for id in range(0, g_thread_cnt): 
		th_client = ClientThread(id)
		threads.append(th_client)
	th_print = PrintThread()
	threads.append(th_print)
	
	for th in threads:
		th.setDaemon(True)
		th.start()

	for th in threads:
		th.join()

