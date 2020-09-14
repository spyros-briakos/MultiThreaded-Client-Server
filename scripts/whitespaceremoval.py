import sys 

def main():

    file_name = sys.argv[1]
    file1 = open(file_name, 'r') 
    Lines = file1.readlines() 
    
    L = []
    for line in Lines: 
        L.append(line.rstrip())
        L.append('\n')

    # Writing to file 
    file1 = open(file_name, 'w') 
    file1.writelines(L) 
    file1.close() 

if __name__ == '__main__':
    main()