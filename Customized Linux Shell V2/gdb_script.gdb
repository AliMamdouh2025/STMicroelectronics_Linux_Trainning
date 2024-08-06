# Define a GDB function to print process history and count
define print_process_history
  # Print the process_history_count variable
  printf "Process History Count: %d\n", process_history_count

  # Print the header for the process history table
  printf "%-10s  %-20s  %s\n", "Index", "Command", "Exit State"
  printf "%-10s  %-20s  %s\n", "-----", "-------", "----------"

  set $i = 0
  while $i < process_history_count
    printf "%-10d  %-20s  %d\n", $i, process_history[$i].command, process_history[$i].exit_status
    set $i = $i + 1
  end
end

# Add a command alias for easier access
alias printd = print_process_history

