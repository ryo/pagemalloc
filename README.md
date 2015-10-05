pagemalloc
=========================================

pagemalloc is malloc(3) debugging library


usage
-----
    env LD_PRELOAD=$YOUR_LOCAL_INSTALLED_PATH/malloc.so <your_program>
    
    e.g.)
       env LD_PRELOAD=$HOME/tmp/malloc.so emacs
