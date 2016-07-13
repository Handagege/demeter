#!/bin/sh

cd ..
#ulimit -c unlimited
./demeter_serv 12344 ../conf/demeter_server.conf > run.log 2>&1 &
#./demeter_cli 10.77.96.32:12344 --load demeter_resource_20160621
#./demeter_cli 10.77.96.32:12344 --analyze 心想的事都能成
#./demeter_cli 10.77.96.32:12344 --analyze-file weiboText10000
