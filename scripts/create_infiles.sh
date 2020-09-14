#!/bin/bash

#Store array of arguments into an array.
args=("$@") 
#Check for exactly 5 arguments.
if [ "${#args[@]}" != "5" ]; then
    echo "You have to insert exactly 5 arguments!" 
#Case: Everything is fine.
else 
    #Check for two last arguments so as to be numbers.
    re='^[0-9]+$'
    if ! [[ $4 =~ $re ]] ; then
        echo "numFilesperDirectory must be a number!"; exit 1
    fi
    if ! [[ $5 =~ $re ]] ; then
        echo "numRecordsPerFile must be a number!"; exit 1
    fi

    # Make main directory.
    if [ ! -d ./$3 ]; then
        mkdir ./$3
    fi
    
    #Make subdirectories with names from countriesFile.
    while IFS= read -r line
    do
        if [ ! -d ./$3/$line ]; then
            mkdir ./$3/$line
        fi
    done <"$2"
    
    #Store to array disease names of diseasesFile
    disease_array=()
    while IFS= read -r line; do
        disease_array+=("$line")
    done < "$1"

    #Create some useful arrays...
    enter_exit_array=(ENTER EXIT)
    day_array=(01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30)
    month_array=(01 02 03 04 05 06 07 08 09 10 11 12)
    fname_array=(Damon David Troy Rita Fraklin Tomas Arthur Macarena Soledad Nick Abigail Celine Ingrid Laura Sylvia Yolanda Marcus Benjamin Vincent Peter)
    lname_array=(Lopez Rodriguez Alonso Visser Caputo Martinez Yilmaz Graham Smith Owen Adilovic Novak Nielsen Copernicus Matsuda Bandara Herrera Vargas Scott Fisher)

    #Initialization of significant variables...
    #############
    DAY_RANGE=30
    day=0
    #############
    MONTH_RANGE=12 
    month=0
    #############
    YEAR_FLOOR=2000
    YEAR_RANGE=21
    year=0
    ############# 
    AGE_FLOOR=1
    AGE_RANGE=120
    age=0
    #############
    DISEASE_RANGE=${#disease_array[@]}
    disease=0
    #############
    FNAME_RANGE=${#fname_array[@]}
    fname=0
    #############
    LNAME_RANGE=${#lname_array[@]}
    lname=0
    #############
    ENTER_EXIT_RANGE=2
    enter_exit=0
    rID=0
    #############
    
    #find==0: even ? find==1: odd
    find=$((($5*$4)%2))
    if [ "$find" == 1 ]; then
        num_of_times=$((($5*$4)/2 +1))
    else
        num_of_times=$((($5*$4)/2))
    fi

    #Firstly get access into input_dir.
    cd ./"$3"
    #Loop through subdirectories of input_dir.
    for subdir in */
    do
        records_array=()
        w=0
        #Finally create numRecordsPerFile records in each date-format empty file.
        until [ ! $w -lt "$num_of_times" ] 
        do
            #Random age
            age=$RANDOM
            let "age %= $AGE_RANGE"
            age=$(($age+$AGE_FLOOR))
            #Random disease
            disease=$RANDOM
            let "disease %= $DISEASE_RANGE"
            #Random first name
            fname=$RANDOM
            let "fname %= $FNAME_RANGE"  
            #Random last name
            lname=$RANDOM
            let "lname %= $LNAME_RANGE"

            #Prepare format of a single random Record, so as to put it into date file.
            enter_record="$rID ${enter_exit_array[0]} ${fname_array[$fname]} ${lname_array[$lname]} ${disease_array[$disease]} $age"
            exit_record="$rID ${enter_exit_array[1]} ${fname_array[$fname]} ${lname_array[$lname]} ${disease_array[$disease]} $age"
            records_array+=("$enter_record")
            records_array+=("$exit_record")
            w=`expr $w + 1` 
            rID=`expr $rID + 1` 
        done 
        #Delete last element cause we want exactly $4*$5 records. (Case:odd)
        if [ "$find" == 1 ]; then
            records_array=( "${records_array[@]:0:((${#records_array[@]})-1)}" )
        fi
    
        i=0
        #Afterwards create numFilesPerDirectory empty files in each subdirectory.
        until [ ! $i -lt "$4" ] 
        do 
            #Random day
            day=$RANDOM
            let "day %= $DAY_RANGE"  
            #Random month
            month=$RANDOM
            let "month %= $MONTH_RANGE"  
            #Random year
            year=$RANDOM
            let "year %= $YEAR_RANGE"  
            year=$(($year+$YEAR_FLOOR))

            FILE=./"$subdir""${day_array[$day]}-${month_array[$month]}-$year" 
            if [ -f "$FILE" ]; then
                j=0
            else 
                touch ./"$subdir""${day_array[$day]}-${month_array[$month]}-$year"
                j=0
                #Finally insert numRecordsPerFile records in each date-format empty file.
                until [ ! $j -lt "$5" ] 
                do
                    #Pick a random number from 0 to records_array size -1.
                    rec_index=$RANDOM
                    ARRAY_SIZE=${#records_array[@]}
                    let "rec_index %= $ARRAY_SIZE"
                    
                    #Pick a random record from records_array.
                    record=${records_array[$rec_index]}
            
                    #Fill empty file with random records.
                    echo "$record" >> ./"$subdir""${day_array[$day]}-${month_array[$month]}-$year"
                    
                    #Delete the appropriate random record from records_array.
                    records_array=( "${records_array[@]:0:$rec_index}" "${records_array[@]:$(($rec_index + 1))}" )
                    j=`expr $j + 1` 
                done
                i=`expr $i + 1` 
            fi
        done
    done
    cd ..
fi