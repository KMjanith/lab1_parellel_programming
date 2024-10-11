# lab1_parellel_programming

this repo containes the codes for insert, delete and member function for a simple liked list using parellel programming.we have used pthreads for thread executions and you can see the logics we have used by diving into the code.

## to compile the code 
```
gcc <filaName.c>
```
## to compile and to add a name you want compile with following command
```
gcc -o fileName <fileName.c> -lpthread
```
## to run the compiled code
```
<name of the compiled .exe> <thread_count> <n> <m> <m_member_fraction> <m_delete_fraction> <m_insert_fraction>
```

# way we find out the sample size to get execution times to achieve the desired accuracy level and confidence level of 95%.

* we ran the m operations 100 times.
* then collected the mean, standard deviation and the sample size
* then ran the m operations equals to that sample size and collected the data and calculated the mean and the standard deviation for specific thread counts.

# running codes and get the output
## windows
1. open a terminal and navigate to the project folder. 
2. run the .bat file.
```
run.bat
```
3. You can see a text file named `results.txt`. it containes all the results

## for lynux
1. open a terminal and navigate to the project folder. 
2. run the .sh file.
```
./run.sh
```
3. You can see a text file named `results.txt`. it containes all the results


