#Store array of arguments into an array.
args=("$@") 
#Check for exactly 4 arguments.
if [ "${#args[@]}" != "4" ]; then
    printf "You have to insert exactly these 4 arguments...\nNumber_Of_Query_Port ServerIP Number_Of_Workers Name_Of_Query_File(ex. infos/queries)\n"; exit 1
fi

make client
./executable/whoClient -sp $1 -sip $2 -w $3 -q $4