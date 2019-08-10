# 5G RAN - Random Access procedures implementation
The goal of the project is to develop and test Random Access procedures,a set of procedures that serve as a handshaking process between the user equipment (UE) and the base station (gNB). There are two kinds of Random Access : Contention-Based and Contention-Free. 

### Prerequisites

1. ASN.1 complier to execute `asn1c` commands
2. ASN.1 header files for creating required configuration files
3. ASN.1 generated header files


If ASN.1 compiler is not present, install it using the command:
```
sudo apt install asn1c
```
To get the ASN1 skeletons, go to [this repository](https://github.com/vlm/asn1c).
Download the folder named `skeletons`. Paste the contents of this folder in the same folder where the C standard library header files are present.

### Getting Started
Clone this repository on the current system. The next step is to generate the configuration files from the ASN.1 file `rach_headers.asn`. 
```
asn1c -R -fcompound-names rach_headers.asn
```
Observe that the configuration files (header files and `.c` files) have been generated in the same directory where the `rach_headers.asn` is present

Now that all the pre-requisites are ready, we are ready to move on to the actual code. 

### Primary programs
The 2 most important programs are :
* `gNB_procedures.c`
* `UE_procedures.c`

`gNB_procedures.c` - Contains the random access procedures to be done at gNB. This program is modeled as a server program
`UE_procedures.c` - Contains the random access procedures to be done at UE. This program is modeled as a client program
### Running the server program
Once the server program is compiled and executed, the output is : 
```
Socket successfully created..
Socket successfully binded..
Server listening..
```
Now the server is listening for a client program to connect.

### Running the client program
Once the client program is compiled and executed, the output is : 
```
Socket successfully created..
connected to the server..
RA-RNTI is: 32767 
38
Received RAR (e0|01.84.07.c4.29.01.2c) for preamble 56
TB decoded successfully
decoded header- E,T,RAPID, (0 | 0| 56)
decoded RAR- TAC:48
 Hopping  flag:1
 PUSCH_Freq:124
 PUSCH_Time:4
  MCS:2
 TPC:4
 CSI:1
 T_CRNTI:300
-202
```

The client side output displays various parameters of the Random Access Response (RAR) received from gNB, with a log message indicating that the transport block has been decoded successfully.

The corresponding output at the server is :
```
Server accept the UE_addr...
38
Received RAR (e0|01.84.07.c4.29.01.2c) for preamble 56/56
```
The above data is the actual data transmitted by gNB to UE.

### Remarks
The current outputs have clearly focussed on transmission and reception of RAR. In further updates, the subsequent steps shall be explained in detail.

## Authors

* **Abhay Vardhan** - *Intern* 
* **Monisha Yuvaraj** - *Project Associate* 
* **Pranav Kumar** - *Intern* 
* **Shujith** - *Project Associate* 

