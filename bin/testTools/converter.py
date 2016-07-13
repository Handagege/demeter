#!/usr/bin/python2.6
# -*- coding: utf-8 -*-
import time
import sys
import urllib2
import random
import socket
import struct
import threading
from time import sleep
try:
    import simplejson as json
except Exception,data:
    import json


rmidKey = 'rmid'
pmidKey = 'pmid'
kmidKey = 'kmid'
cmdKey = 'cmd'
weiboType = 'type'
ISOTIMEFORMAT='%Y-%m-%d %X'


'''
从firehose中读取数据
http://wiki.intra.weibo.com/Firehose%E6%8E%A5%E5%85%A5%E8%AF%B4%E6%98%8E
http://wiki.intra.weibo.com/Firehose_2.0%E6%8E%A5%E5%85%A5%E8%AF%B4%E6%98%8E

1、微博（）
type  = "status"
event = ["add"(发博，包括原创和转发), "update", "delete"]
"http://firehose0.i.api.weibo.com:8082/comet?appid=interestfeed"

'''
FIREHOSE_ID_LEN = 16


class Firehose(object):
    def __init__(self, baseurl):
        self.type = 'firehose'
        self.sinceId = "false"
        self.baseurl = baseurl
        self.open_firehose()
        
    def open_firehose(self):
        self.url = self.baseurl + self.sinceId
        req = urllib2.Request(self.url)
        self.conn = urllib2.urlopen( req )

    def parse_data(self):
        pass

    def read_data(self):
        st = time.time()
        # 如果连接发生错误，重连
        if not self.conn:
            self.open_firehose()

        # 如果返回码不是200,这是连接被firehose踢了，重连
        # 被踢了conn_stat_code也是200,这是什么机制??
        self.firehose_data = self.conn.readline()
        conn_stat_code = self.conn.getcode()
        if conn_stat_code != 200:
            self.open_firehose()
            self.firehose_data = self.conn.readline()
            
        # 有时候是空的数据,这是连接被firehose踢了，重连
        if not self.firehose_data:
            self.open_firehose()
            #self.firehose_data = self.conn.readline().strip()       # tail with \r\n ?
            self.firehose_data = self.conn.readline() 

        # 重连时需要记录sinceId
        try:
            self.firehose_dict = json.loads(self.firehose_data)
            self.sinceId = str(self.firehose_dict["id"])
        except Exception,data:
            print data


    def read_line(self):
        self.result_dict = {}
        self.read_data()
        return self.parse_data()


# 微博行为 
class WeiboFirehose(Firehose):
    def __init__(self):
        self.type = 'weibo'
        self.sinceId = "false"
        # 为了防止firehose真的读到了空的数据
        self.baseurl = "http://firehose0.i.api.weibo.com:8082/comet?appid=interestfeed&filter=status,*&loc="
        self.open_firehose()

    def parse_data(self):
        result = None
        try:
            weibo_content = self.firehose_dict['text']['status']
            result = weibo_content['text']
        #try:
        #    result_dict = {}
        #    weibo_content = self.firehose_dict['text']['status']
        #    event = self.firehose_dict['text']['event']
        #    if event == "add":
        #        result_dict[kmidKey] = weibo_content['mid']
        #        #result_dict['uid'] = weibo_content['user']['id']
        #        if 'parent_rt_id_db' in weibo_content:
        #                result_dict[weiboType] = 'transmit'
        #                result_dict[pmidKey] = str(weibo_content['parent_rt_id_db'])
        #                result_dict[rmidKey] = weibo_content['retweeted_status']['mid']
        #        else:
        #                result_dict[weiboType] = 'original'
        #                result_dict[rmidKey] = result_dict['kmid']
        except Exception, e:
            print e
        return result


    def getTest(self,num):
        weiboText = []
        while len(weiboText) < num:
            text = self.read_line()
            if text:
                weiboText.append(text.encode('utf8'))
        return weiboText 


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



if __name__ == '__main__':
        wf = WeiboFirehose()
        num = 10000
        weiboText = wf.getTest(num)
        with open('weibo_textAnalyze_'+str(num),'w') as fout:
            for i in weiboText:
                fout.write(i+'\n')
