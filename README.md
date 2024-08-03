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

# sample codes for compilation and run the program
## for(n=1000,m=10000,member_fraction=0.99,insert_fraction=0.005,delete_fraction=0.005)
### Serial linkedlist
* for compile
```
gcc -o serial SerialLinkedList.c -lpthread
```
* to run
```
serial 1 1000 10000 0.99 0.005 0.005
```
### Linked List with Mutex
* for compile
```
gcc -o mutexLinkedList linkedListWithMutex.c -lpthread
```
* to run
```
mutexLinkedList 4 1000 10000 0.99 0.005 0.005
```
### Linked List with RW lock
* for compile
```
gcc -o linkedListWithRandWLock linkedListWithRandWLock.c -lpthread -lm
```
* to run
```
linkedListWithRandWLock 4 1000 10000 0.99 0.005 0.005
```
