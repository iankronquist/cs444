#!/bin/bash
OUTPUT=git_log.tex
# Find remote URL for hashes (designed for GitHub-hosted projects)
origin=`git config remote.origin.url`
base=`dirname "$origin"`/`basename "$origin" .git`

# Output LaTeX table in chronological order
printf '\\begin{tabular}{l l l}\\textbf{Detail} & \\textbf{Author} & \\textbf{Description}\\\\ \\hline' > $OUTPUT
git log --pretty=format:"\\href{$base/commit/%H}{%h} & %an & %s\\\\\\hline" --reverse | sed 's/_/\\_/g' >> $OUTPUT
printf '\\end{tabular}' >> $OUTPUT
