<img src="assets/logo/mlog-logo-2.png" width="300"  alt="minilog-logo" align="center"/>   

> *A Minimalistic  log print using termcapabities for native support for almost all terminal.*


## About 

As its name suggests, minilog is a tool dedicated to native logging based on termcapabilities .
It adapts automatically to different terminals, thanks to its __support for termcapabilities__, and is often already present on most Unix systems.

Lightweight and easy to use, it has no external dependencies,
which reduces the memory footprint when used.

By default, for fast display, standard output is not buffered, i.e. it is not timed.
by the operating system, giving you instant display of messages.
It can be quickly integrated into your workflow, as the project is built into a single file called a shared object.

It can be integrated into your code by creating a link to it.

> [!IMPORTANT]
> For the moment, python support is under construction, and will be offered as a package using this shared object file.
> 


### How do I build it?
To install or build it, we used the build manager called [Meson Build System](https://mesonbuild.com/index.html).

And here are the commands to type: 

After cloning. 
1 -> Enter to project. 
```bash 
$ cd Minilog
``` 

2 -> start construction with meson
```bash
$ meson setup build      # build the project
$ meson compile -C build # compile project
```

At the end of compilation, you'll have a .so file that can be integrated into your environment. 


### OverView: 

For a quick appercu, a dockerfile is provided to build the environment and get an overview.
Or you can directly use the build project as described above (see the section on how to build)
and in the build folder you'll have the executable.

Feel free to play around with the main.c file, which contains code examples you can play with too. 

For the more timid, use Docker : 
``bash 
$ docker build . -t minilog 
$ docker run minilog # run it  
```

Here are a few screenshots to see what the logs look like 



> [!NOTE]
> This project is in its initial state and needs feedback or feature requests to improve it as much as possible.
> This project aims to satisfy a large number of users, so please feel free to submit pull requests or issues by placing a flag. 
>     feat: so I know it's a feature request,  
>     idea: to discuss an idea
>     question: if it's a question, I'll be happy to answer it. 
>


**_Happy Hacking Folks_** 

<img src="assets/logo/glls1-logo-svg.png" width="100"  height="" alt="gllogo"  align="right"/>

Feel free to contribute.

We ❤️  Computers at all Levels.

From Senegal. 🇸🇳


