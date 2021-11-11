# Q1.b
Based on my piper I would assume that the ps aux will run, generate its output and write it to a pipe (not stdout) where then, grep will
run, consume the output from pip (his now - stdin) and search for the phrase "pizza-margaritta". In this sequence of events, where
ps aux runs before grep -> the process "grep pizza-margaritta" should not show up in the ps aux output, however, in linux we do get this
match string, meaning grep ran before ps took a snapshot! 

## Possible Explations
This can be explained by the fact that linux ran grep *before* ps in order to be
more efficient in the search with grep - assume you have a 3Gb file you want to grep - it will be exaustive to cat the whole file to a pipe
and only then have grep go over it.
Another explanation can be that the pipe gets blocked after a certain amount of bytes and only when grep is reading from it, and running, the
 ps aux continues and eventually outputs the grep as a running process.
