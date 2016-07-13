import time

class CostStat:
	cnt_ = list()
	cost_ = list()	
	total_cnt_ = 0 
	failed_cnt_ = 0
	total_cost_ = 0.0 
	send_bytes_ = 0
	recv_bytes_ = 0
	begin_ = 0
	last_begin_ = 0
	last_total_cnt_ = 0

	def __init__(self):
		for i in range(0,10):
			self.cnt_.append(0)
			self.cost_.append(0.0)
		self.total_cnt_ = 0
		self.failed_cnt_ = 0
		self.total_cost_ = 0.0
		self.send_bytes_ = 0
		self.recv_bytes_ = 0
		self.begin_ = time.time()
		self.last_begin_ = time.time()
		self.last_total_cnt_ = 0
		self.sign_ = ["[0~10ms]   ", "[10~20ms]  ", "[20~30ms]  ", "[30~40ms]  ", "[40~50ms]  ", "[50~100ms] ", "[100~200ms]", "[200~300ms]", "[300~500ms]", "[>500ms]   "]

	def StatFailed(self, send_bytes, msec):
		self.total_cnt_ += 1
		self.total_cost_ += msec
		self.send_bytes_ += send_bytes
		self.failed_cnt_ += 1
		idx = 0	
		if msec < 10:
			idx = 0	
		elif msec < 20:
			idx = 1
		elif msec < 30:
			idx = 2
		elif msec < 40:
			idx = 3
		elif msec < 50:
			idx = 4
		elif msec < 100:
			idx = 5
		elif msec < 200:
			idx = 6
		elif msec < 300:
			idx = 7
		elif msec < 500:
			idx = 8
		else:
			idx = 9
		self.cnt_[idx] += 1
		self.cost_[idx] += msec

	def StatSuc(self, send_bytes, recv_bytes, msec):
		self.total_cnt_ += 1
		self.total_cost_ += msec
		self.send_bytes_ += send_bytes
		self.recv_bytes_ += recv_bytes
		idx = 0	
		if msec < 10:
			idx = 0	
		elif msec < 20:
			idx = 1
		elif msec < 30:
			idx = 2
		elif msec < 40:
			idx = 3
		elif msec < 50:
			idx = 4
		elif msec < 100:
			idx = 5
		elif msec < 200:
			idx = 6
		elif msec < 300:
			idx = 7
		elif msec < 500:
			idx = 8
		else:
			idx = 9
		self.cnt_[idx] += 1
		self.cost_[idx] += msec

	def PrintPercentToString(self, percent): 
		pcnt = int(30.0 * percent)
		str = ""
		for i in range(0, pcnt):
			str += "|"
		for i in range(pcnt, 30):
			str += " "
		s = "%"	
		str_out = format("%.2f%s\t%s" % (percent*100.0, s, str))
		return str_out

	def Print(self):
		send_mbytes = float(self.send_bytes_) / float(1024*1024)
		recv_mbytes = float(self.recv_bytes_) / float(1024*1024)
		avg_cost = 0.0
		cur = time.time()	
		qps = float(self.total_cnt_) / (float(cur - self.begin_) + 0.001) 
		inst_qps = float(self.total_cnt_ - self.last_total_cnt_) / (float(cur - self.last_begin_) + 0.001) 
		self.last_total_cnt_ = self.total_cnt_
		self.last_begin_ = cur
		if self.total_cnt_ > 0:
			avg_cost = self.total_cost_ / float(self.total_cnt_)
		print "--"
		print "send(MB): %0.3f, recv(MB): %0.3f" % (send_mbytes, recv_mbytes)
		print "req_cnt: %d, suc: %d, failed: %d, avg_cost(ms): %.5g" % (self.total_cnt_, self.total_cnt_ - self.failed_cnt_, self.failed_cnt_, avg_cost)
		print "inst-qps: %.5g, qps: %.5g" % (inst_qps, qps)
		print "--"
		print "stat:"
		print "-----------------------------------------------------------"
		for i in range(0, 10):
			percent = 0.0
			if self.total_cnt_ > 0:
				percent = float(self.cnt_[i]) / float(self.total_cnt_)
			stt = self.PrintPercentToString(percent)
			print "%s\t%s\t%d" % (self.sign_[i], stt, self.cnt_[i])
		print "-----------------------------------------------------------"

if __name__ == "__main__":
	import sys

	stat = CostStat()
	for i in range(1, 6000):
		stat.StatSuc(i*5, i*10, float(i))
	stat.Print()


