+-----------------------+
               |      User Input       |
               +-----------------------+
                           |
                           v
                     parse(input)
                           |
                           v
                +---------------------+
                | handleBuiltin()?    |-- YES --> [Execute builtin: cd/pwd/exit]
                +---------------------+
                           |
                          NO
                           v
                +---------------------+
                |     hasPipe()?      |
                +---------------------+
                       /       \
                     YES        NO
                     /            \
                    v              v
         executePipe()          execute()
         [Fork Process 1]       [Handle > or < Redirection]
         [Fork Process 2]              |
         [Connect via pipe()]          v
                    \           fork() + execvp()
                     \                 |
                      v                v
            [Return to Prompt] <---+---+
