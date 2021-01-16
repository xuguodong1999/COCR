from PyQt5.QtCore import QSharedMemory
from sip import voidptr
import numpy as np

# qsm = QSharedMemory('hello.jpg')
# qsm.create(4)
# ptr=qsm.data()

# array=np.zeros(1).astype(np.uint8)
# array[0]=3
# arr=voidptr(array.data) 
# ptr[0:1]=arr[0:1]


# print(qsm.isAttached())
# print(qsm.error())

qsm2 = QSharedMemory('hello.jpg')
qsm2.attach()

array2=np.zeros(1).astype(np.uint8)
print(array2)

ptr2=qsm2.data()

arr2=voidptr(array2.data) 
arr2[0:1]=ptr2[0:1]

print(array2)

print(qsm2.key())
