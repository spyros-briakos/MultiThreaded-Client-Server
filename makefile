default: master executable/whoClient executable/whoServer

master: executable/master executable/inactive_worker executable/worker

server: executable/whoServer

client: executable/whoClient

executable/whoServer: build/server.o build/helpserver.o
	g++ -pthread -std=c++11 build/server.o build/helpserver.o -o executable/whoServer

executable/whoClient: build/client.o build/helpclient.o
	g++ -pthread -std=c++11 build/client.o build/helpclient.o -o executable/whoClient

executable/master: build/master.o build/helpmaster.o
	g++ -std=c++11 build/master.o build/helpmaster.o -o executable/master

executable/inactive_worker: build/inactive_worker.o
	g++ -std=c++11 build/inactive_worker.o -o executable/inactive_worker

executable/worker: build/worker.o build/helpworker.o build/binary_tree.o build/record_list.o build/bucket_list.o build/max_heap.o
	g++ -std=c++11  build/binary_tree.o build/record_list.o build/bucket_list.o build/max_heap.o build/helpworker.o build/worker.o -o executable/worker 

build/master.o: source/master.cpp
	g++ -std=c++11 -c source/master.cpp -o build/master.o 

build/helpmaster.o: source/helpmaster.cpp
	g++ -std=c++11 -c source/helpmaster.cpp -o build/helpmaster.o 

build/inactive_worker.o: source/inactive_worker.cpp
	g++ -std=c++11 -c source/inactive_worker.cpp -o build/inactive_worker.o 

build/worker.o: source/worker.cpp
	g++ -std=c++11 -c source/worker.cpp -o build/worker.o

build/helpworker.o: source/helpworker.cpp
	g++ -std=c++11 -c  source/helpworker.cpp -o build/helpworker.o

build/record_list.o: source/record_list.cpp
	g++ -std=c++11 -c  source/record_list.cpp -o build/record_list.o

build/bucket_list.o: source/bucket_list.cpp
	g++ -std=c++11 -c  source/bucket_list.cpp -o build/bucket_list.o

build/binary_tree.o:	source/binary_tree.cpp
	g++ -std=c++11 -c source/binary_tree.cpp -o build/binary_tree.o 

build/max_heap.o:	source/max_heap.cpp 
	g++ -std=c++11 -c source/max_heap.cpp -o build/max_heap.o

build/server.o: source/server.cpp
	g++ -std=c++11 -c source/server.cpp -o build/server.o

build/helpserver.o: source/helpserver.cpp
	g++ -std=c++11 -c source/helpserver.cpp -o build/helpserver.o 

build/client.o: source/client.cpp
	g++ -std=c++11 -c source/client.cpp -o build/client.o 

build/helpclient.o: source/helpclient.cpp
	g++ -std=c++11 -c source/helpclient.cpp -o build/helpclient.o 

clean:
	rm build/binary_tree.o build/bucket_list.o build/max_heap.o build/record_list.o build/helpworker.o build/worker.o build/diseaseaggregator.o build/helpdiseaseAggregator.o build/inactive_worker.o build/server.o build/helpserver.o build/client.o build/helpclient.o build/master.o build/helpmaster.o executable/master executable/worker executable/inactive_worker executable/whoServer executable/whoClient pipes/PCfifo*
