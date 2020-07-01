#!/bin/bash

#https://github.com/HarryWei/research/commit/62860866a08a44899a6d8c6c40284f5df4ce0eed

#https://gist.github.com/niksumeiko/8972566

REMOTE=new-origin
BRANCH=master
#BRANCH1=batch
#BRANCH2=tailor-4.18
#BRANCH3=task-signal
#BRANCH4=sched
#BRANCH5=sched4.9
#BATCH_SIZE=20000
#change BATCH_SIZE manually

git rev-list --reverse $BRANCH | ruby -ne 'i ||= 0; i += 1; puts $_ if i % 20000 == 0' | xargs -I{} git push $REMOTE +{}:refs/heads/$BRANCH
#git rev-list --reverse $BRANCH1 | ruby -ne 'i ||= 0; i += 1; puts $_ if i % 20000 == 0' | xargs -I{} git push $REMOTE +{}:refs/heads/$BRANCH1
#git rev-list --reverse $BRANCH2 | ruby -ne 'i ||= 0; i += 1; puts $_ if i % 20000 == 0' | xargs -I{} git push $REMOTE +{}:refs/heads/$BRANCH2
#git rev-list --reverse $BRANCH3 | ruby -ne 'i ||= 0; i += 1; puts $_ if i % 20000 == 0' | xargs -I{} git push $REMOTE +{}:refs/heads/$BRANCH3
#git rev-list --reverse $BRANCH4 | ruby -ne 'i ||= 0; i += 1; puts $_ if i % 20000 == 0' | xargs -I{} git push $REMOTE +{}:refs/heads/$BRANCH4
#git rev-list --reverse $BRANCH5 | ruby -ne 'i ||= 0; i += 1; puts $_ if i % 20000 == 0' | xargs -I{} git push $REMOTE +{}:refs/heads/$BRANCH5

git push --all $REMOTE 

git push --tags new-origin
git remote -v
git remote rm origin
git remote rename new-origin origin
git branch --set-upstream-to=origin/master master 
git pull

