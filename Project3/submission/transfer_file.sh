file=$1

if [ -z "$2" ]
	then
		dest="~"
	else
		dest=$2
fi


scp -P 8022 $file root@localhost:$dest
