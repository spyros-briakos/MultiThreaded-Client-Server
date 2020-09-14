# MultiThreaded-Client-Server
In this project server is gonna have multiple server threads, whose main goal is to serve client-threads.

# Summary

The main idea of this project is that Server firstly runs and then accesively program called 'Master' does, whose main goal is to help Server, because Master has children processes, called Workers, where each Worker plays the role of 'mini-server' in order to help with answering Server's queries. Then the only remaining program is Client, who is going to make multiple queries to Server (with help of client-threads). The result is to see in terminal queries-answers in extremelly quick time.

# Execution 

1) For server's execution:

**bash scripts/script1.sh Number_Of_Query_Port Number_Of_Statistics_Port Number_Of_Threads BufferSize**

2) For master's execution:

**bash scripts/script2.sh ServerIP Number_Of_Statistics_Port BufferSize Number_Of_Workers Name_Of_Dir(ex. dirs/main_dir)**

3) For client's execution

**bash scripts/script3.sh Number_Of_Query_Port ServerIP Number_Of_Workers Name_Of_Query_File(ex. infos/queries)**

4) For cleaning:

**bash scripts/script4.sh**

# query_file.py 
This is just a script that creates a file with queries (more information through runtime). If you wanna run this python script, notice this example:

**python scripts/query_file.py --queryFile infos/new_queries --numQueries 1000 --commandsFile infos/commands.txt 
--countriesFile infos/countries --diseasesFile infos/diseases --maxK 6 --maxRecordID 1000 --startDate 10-10-2000 --endDate 10-10-2020**

# create_infiles.sh 
This is just a script that creates a directory with records which is gonna be the input of our main program. 
If you wanna make one directory you should run something like this:

**./scripts/create_infiles.sh File_with_diseases File_with_countries Name_of_dir numFilesPerDirectory numRecordsPerFile**

