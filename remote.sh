#!\bin\sh
IP=pi@10.141.19.99
FPI="/home/pi/Desktop/scrivejack/main.cpp"
DPI="/home/pi/Desktop/scrivejack"
FPC="C:\Users\10jac\OneDrive\Documents\UNI\ENGR 101\AVC\avc\main.cpp"
echo Transferring
scp main.cpp $IP:$FPI
echo Compiling
#ssh $IP sh -c 'cd /home/pi/AVC/simple && ls && make'
ssh $IP make -c $DPI
echo Finished

raspberry


cd /mnt/c/Users/10jac/OneDrive/Documents/UNI/'ENGR 101'/AVC/avc/remote.sh


cp /mnt/c/Users/10jac/OneDrive/Documents/UNI/'ENGR 101'/AVC/avc/remote.sh /home/jack/avc