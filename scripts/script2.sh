#Store array of arguments into an array.
args=("$@") 
#Check for exactly 5 arguments.
if [ "${#args[@]}" != "5" ]; then
    printf "You have to insert exactly these 5 arguments...\nServerIP Number_Of_Statistics_Port BufferSize Number_Of_Workers Name_Of_Dir(ex. dirs/main_dir)\n"; exit 1
fi

make master
./executable/master -s $1 -p $2 -b $3 -w $4 -i $5