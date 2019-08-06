# SOSAssignment
2nd Year Secure Operating Systems Assignment based on Multi-threading and Virtual Memory Management in C. 

Operating Systems (IE2050) / Operating Systems Security (IE2032) Assignment - 2019

TASK 01
Introduction

Develop a program which implements an application to calculate the average of some values obtained
from a file which contains a dataset.

Deliverables

• Write a C program to perform the following functions

➢ Read line by line from the file (dataset.txt)

➢ Once a record of the dataset is taken by a thread, the same dataset should not be taken
by another thread.

(Hint: To do perform this section, you should add a flag (some value) to mention that
you have used the line)

➢ Calculate the average of each line and write to another file (average.txt)
Use printf statements wherever necessary, all possible errors should be handled by printf statements,
state any assumptions made and submit it as a hard copy.
_______________________________________________________________

TASK 02
Introduction
Develop a program which implement an application which contain two entities: Lecturer and student.
The two entities should access a shared location to view, upload, download and delete case studies.
The lecturer should be able to perform the following task
➢ View the available case studies
➢ Upload new case studies
➢ Download available case studies
➢ Edit / Delete case studies
The students should be able to perform the following tasks
➢ View the available case studies
➢ Download available case studies
There should not be any interruptions for any process that takes place.
Deliverables
Write a C program to cater the above-mentioned requirements of the 02 entities.
The program execution should have 02 command line argument.
Argument :- action
./<executable_file_name> <Argument>
Example:- . / lecturer upload
. / student download
Use printf statements wherever necessary, all possible errors should be handled by printf statements,
state any assumptions made and submit it as a hard copy.
__________________________________________________________
