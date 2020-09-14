#Store array of arguments into an array.
args=("$@") 
#Check for exactly 4 arguments.
if [ "${#args[@]}" != "4" ]; then
    printf "You have to insert exactly these 4 arguments...\nNumber_Of_Query_Port Number_Of_Statistics_Port Number_Of_Threads BufferSize\n"; exit 1
fi
make server
./executable/whoServer -q $1 -s $2 -w $3 -b $4