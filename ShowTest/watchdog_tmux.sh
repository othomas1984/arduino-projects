#!/bin/bash

SESSION="lightshow"
SCRIPT="start_show.py"
PYTHON="python"  # Or use `which python3`

# Create tmux session if it doesn't exist
if ! tmux has-session -t "$SESSION" 2>/dev/null; then
    echo "Creating tmux session: $SESSION"
    tmux new-session -d -s "$SESSION"
fi

while true; do
    if ! tmux list-panes -t "$SESSION" -F '#{pane_current_command}' | grep -q "python"; then
        echo "🔁 Restarting $SCRIPT in tmux session $SESSION at $(date)"
        tmux send-keys -t "$SESSION" C-c
        sleep 1
        tmux send-keys -t "$SESSION" "$PYTHON $SCRIPT" Enter
    fi
    sleep 10
done
