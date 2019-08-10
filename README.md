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
Compile the program  


## Authors

* **Abhay Vardhan** - *Intern* 
* **Monisha Yuvaraj** - *Project Associate* 
* **Pranav Kumar** - *Intern* 
* **Shujith** - *Project Associate* 

