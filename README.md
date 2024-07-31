# lab1_parellel_programming

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

# to compile linkedListWthR&D
gcc -o linkedListIWithRandWLock linkedListIWithRandWLock.c -lpthread -lm

./linkedListIWithRandWLock 1000 1000 0.5 0.3 0.2