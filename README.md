# Reproduce results of RACK

This repository has the upgraded implementation of RACK in ns-3 (https://github.com/ShikhaBakshi/Reproduce-RACK-Results).

## Steps to reproduce results:

1. Clone this repo:
```
git clone https://github.com/mjthewalker/Reproduce-RACK-Results-New.git
```

2. Configure and build the cloned repo.
```
./ns3 configure --enable-examples --enable-tests
./ns3 build
```

3. Run the script results.sh
```
./results.sh
```

On running this, a folder named ```"rack"``` will be created in the main ns-3 directory containing all the results.

4. Run the script http.sh

On running this script two folders ```rack_on``` and ```rack-off``` will be created. These folders contain the results of the
```Web Traffic Scenario``` run with 25 different seed values in both RACK enabled and RACK disabled scenario.

Steps To generate CDF graphs:

a. Each simulation result folder contains ```client-RTT``` files for all the five clients. We need to take the average of all the RTT columns (column 2) of all the 25 simulations for each client individually. <br>
b. We do this for both RACK ON and RACK OFF cases. <br>
c. At the end we generate the CDF graphs with the data obtained by averaging the RTTs for each client in both the scenarios
and overlap them.

5. To generate the results of Test 1 and Test 2, run the below command:

```
./test.py --suite="tcp-rack-test"
```
