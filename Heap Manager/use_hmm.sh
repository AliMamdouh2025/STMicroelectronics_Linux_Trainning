#

# Save the current directory
HMM_DIR="$PWD"

# Set up the environment
export LD_LIBRARY_PATH="$HMM_DIR:$LD_LIBRARY_PATH"
export LD_PRELOAD="$HMM_DIR/libhmm.so"

# Print a message to confirm the custom library is in use
echo "Custom HMM library activated for this session."

# Start a new shell
$SHELL

# When the shell exits, clean up the environment
unset LD_PRELOAD
echo "Custom HMM library deactivated."

