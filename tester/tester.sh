#!/bin/bash

# Server Details
SERVER=localhost
PORT=6667
DEFAULTNICK=opelser
PASSWORD=test

# Files
LOGFILE="output.log"

# Clear the log file
echo -e "[TESTER BEGIN]\n" | tee $LOGFILE

# ANSI escape codes for colors
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
BLUE='\033[34m'
MAGENTA='\033[35m'
CYAN='\033[36m'
RESET='\033[0m'

# Function to log with timestamp and color
log_with_timestamp() {
	local color=$1
	local message=$2
	echo -e "$(date '+%Y-%m-%d %H:%M:%S') - ${color}${message}${RESET}" | tee -a $LOGFILE
}

# Compile the server
log_with_timestamp "$BLUE" "Compiling the server..."
make DEBUG=1 -C ../
if [ $? -ne 0 ]; then
	log_with_timestamp "$RED" "Failed to compile the server"
	exit 1
fi

# Start the server in the background
log_with_timestamp "$BLUE" "Starting the server..."
../ircserv $PORT $PASSWORD &
SERVER_PID=$!
sleep 2

# Check if the server starting failed
if ! kill -0 $SERVER_PID 2>/dev/null; then
	log_with_timestamp "$RED" "Failed to start the server"
	exit 1
fi

# Loop through input files
for INPUTFILE in "$@"
do 
	# Create a named pipe (FIFO)
	PIPE=$(mktemp -u)
	mkfifo $PIPE

	# Start nc in the background, reading from the named pipe
	nc -C $SERVER $PORT -w2 < $PIPE | tee -a $LOGFILE &

	# Get the PID of the nc process
	NCPID=$!

	# Open the named pipe for writing
	exec 3> $PIPE

	# Log the input file being processed in blue
	log_with_timestamp "$BLUE" "Processing $INPUTFILE"

	# Check if the file exists
	if [ ! -f $INPUTFILE ]; then
		log_with_timestamp "$RED" "File $INPUTFILE not found"
		continue
	fi

	# Check if the file is empty
	if [ ! -s $INPUTFILE ]; then
		log_with_timestamp "$RED" "File $INPUTFILE is empty"
		continue
	fi

	# Loop through the file and send commands to the server
	while IFS= read -r line || [[ -n $line ]]
	do
		# Don't send empty lines or comments
		if [[ $line == \#* ]]; then
			log_with_timestamp "$YELLOW" "$line"
			continue
		fi

		if [[ $line == "" ]]; then
			echo -en "\n" >> $LOGFILE
			continue
		fi

		# Log the command being sent in green
		log_with_timestamp "$GREEN" "SEND: $line"

		# Send the command to the named pipe
		echo "$line" >&3

		# Sleep to ensure the server processes each command
		sleep 1

	done < $INPUTFILE

	# Close the named pipe
	exec 3>&-

	# Wait for the nc process to finish
	wait $NCPID

	# Remove the named pipe
	rm $PIPE

	# Log the input file being processed in blue
	log_with_timestamp "$BLUE" "Finished processing $INPUTFILE\n"
done

# Stop the server
log_with_timestamp "$BLUE" "Stopping the server..."
kill $SERVER_PID

# Add a break line at the end of the log
echo -e "\n\n\n[TESTER END]" | tee -a $LOGFILE
