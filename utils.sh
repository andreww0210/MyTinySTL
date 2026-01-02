# use ' ' for dynamic get next hash code
alias show='git show tiny-stl'
alias show_next='git show $(git rev-list tiny-stl..origin/master | tail -n 1)'
alias next='git switch tiny-stl && git reset --hard $(git rev-list tiny-stl..origin/master | tail -n 1)'
